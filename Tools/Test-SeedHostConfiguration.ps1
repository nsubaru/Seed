param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedRuntime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedConsole = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Console/Seed.Console.exe"
$seedRunner = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Managed.TestRunner/Seed.Managed.TestRunner.exe"
$seedOutput = Join-Path $seedRoot ("Artifacts/host-configuration/$Configuration/" + [Guid]::NewGuid().ToString('N'))
$seedDirectory = Join-Path $seedOutput 'live'
New-Item -ItemType Directory -Path $seedDirectory -Force | Out-Null
if ([Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    throw 'Run host configuration validation under a non-elevated account.'
}

$seedSavedDirectory = [Environment]::GetEnvironmentVariable('SEED_CONFIG_DIRECTORY')
[Environment]::SetEnvironmentVariable('SEED_CONFIG_DIRECTORY', $seedDirectory)
$seedProcess = $null
$seedPipe = 'Seed.KMS.v1'
function Invoke-HostCommand([string] $Command, [string[]] $Arguments = @(), [int] $ExpectedExit = 0)
{
    $seedText = & $seedConsole --pipe $seedPipe $Command @Arguments 2>&1
    if ($LASTEXITCODE -ne $ExpectedExit)
    {
        throw "Host command failed: $Command`n$seedText"
    }

    return $seedText
}

function Invoke-HostReload([string] $NextPipe = '')
{
    $seedPrevious = [regex]::Match(((Invoke-HostCommand status.kernel) -join "`n"), 'Composition: (\d+)').Groups[1].Value
    if (-not $seedPrevious)
    {
        throw 'The runtime did not identify its current composition.'
    }

    Invoke-HostCommand reload | Out-Null
    if ($NextPipe)
    {
        $script:seedPipe = $NextPipe
    }

    $seedDeadline = [DateTime]::UtcNow.AddSeconds(15)
    do
    {
        $seedCurrent = [regex]::Match(((Invoke-HostCommand status.kernel) -join "`n"), 'Composition: (\d+)').Groups[1].Value
        if ($seedCurrent -and $seedCurrent -cne $seedPrevious)
        {
            return
        }

        Start-Sleep -Milliseconds 100
    } while ([DateTime]::UtcNow -lt $seedDeadline)
    throw 'The requested runtime reload did not complete.'
}

function Assert-Health([int] $Port)
{
    $seedClient = [Net.Http.HttpClient]::new()
    $seedClient.Timeout = [TimeSpan]::FromSeconds(5)

    try
    {
        if ($seedClient.GetStringAsync("http://127.0.0.1:$Port/health").GetAwaiter().GetResult() -cne "Seed is running`n")
        {
            throw 'Host-configured HTTP endpoint returned the wrong body.'
        }
    }
    finally
    {
        $seedClient.Dispose()
    }
}

function Assert-Closed([int] $Port)
{
    $seedSocket = [Net.Sockets.TcpClient]::new()

    try
    {
        try
        {
            $seedSocket.Connect('127.0.0.1', $Port)
        }
        catch [Net.Sockets.SocketException]
        {
            return
        }

        throw "Disabled or replaced HTTP listener still accepts connections on $Port."
    }
    finally
    {
        $seedSocket.Dispose()
    }
}

try
{
    $seedStart = & $seedConsole --start $seedRuntime version 2>&1
    $seedStartExit = $LASTEXITCODE
    $seedPidMatch = [regex]::Match(($seedStart -join "`n"), 'Started Seed runtime process (\d+)')
    if ($seedPidMatch.Success)
    {
        $seedProcess = [Diagnostics.Process]::GetProcessById([int]$seedPidMatch.Groups[1].Value)
    }

    if ($seedStartExit -ne 0 -or -not $seedProcess)
    {
        throw "Host fixture startup failed: $seedStart"
    }

    Assert-Health 8080

    foreach ($seedValue in @('0', '65536', '-1'))
    {
        Invoke-HostCommand configurations.set @('host.http.port', $seedValue) -ExpectedExit 1 | Out-Null
    }

    Invoke-HostCommand configurations.set @('host.http.bindAddress', 'localhost') -ExpectedExit 1 | Out-Null
    Invoke-HostCommand configurations.set @('host.kms.pipeName', 'Seed.remote/path') -ExpectedExit 1 | Out-Null
    Invoke-HostCommand configurations.save @('unexpected') -ExpectedExit 1 | Out-Null
    if ((Invoke-HostCommand configurations.get @('host.http.port')) -notmatch '= 8080 ')
    {
        throw 'Rejected host value changed pending state.'
    }

    $seedPortReservation = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)
    $seedPortReservation.Start()
    $seedPort = $seedPortReservation.LocalEndpoint.Port
    $seedPortReservation.Stop()
    $seedPending = Invoke-HostCommand configurations.set @('host.http.port', "$seedPort")
    if ($seedPending -notmatch 'save and reload required')
    {
        throw 'Host setting did not identify its reload requirement.'
    }

    Assert-Health 8080
    Assert-Closed $seedPort
    if (Test-Path -LiteralPath (Join-Path $seedDirectory 'host.v1.bin'))
    {
        throw 'A pending host edit was persisted without save.'
    }

    Invoke-HostReload
    if ((Invoke-HostCommand configurations.get @('host.http.port')) -notmatch '= 8080 ')
    {
        throw 'Unsaved host options survived reload.'
    }

    Invoke-HostCommand configurations.set @('host.http.port', "$seedPort") | Out-Null
    Invoke-HostCommand logging.set @('28', 'HTTP client') | Out-Null
    Invoke-HostCommand configurations.save | Out-Null
    Invoke-HostReload
    Assert-Health $seedPort
    Assert-Closed 8080
    if ((Invoke-HostCommand logging.get @('HTTP client')) -notmatch '= 28')
    {
        throw 'Saved logger mask did not survive reload.'
    }

    Invoke-HostCommand logging.set @('31', 'HTTP client') | Out-Null
    Invoke-HostReload
    if ((Invoke-HostCommand logging.get @('HTTP client')) -notmatch '= 28')
    {
        throw 'Unsaved logger mask survived reload.'
    }

    Invoke-HostCommand configurations.set @('host.http.enabled', 'false') | Out-Null
    Invoke-HostCommand configurations.save | Out-Null
    Invoke-HostReload
    Assert-Closed $seedPort
    Invoke-HostCommand version | Out-Null
    Invoke-HostCommand configurations.reset | Out-Null
    Invoke-HostCommand configurations.save | Out-Null
    Invoke-HostReload
    Assert-Health 8080
    if ((Invoke-HostCommand logging.get @('HTTP client')) -notmatch '= 30')
    {
        throw 'Reset host configuration retained its logger override.'
    }

    $seedCustomPipe = 'Seed.KMS.HostTest.' + [Guid]::NewGuid().ToString('N')
    Invoke-HostCommand configurations.set @('host.kms.pipeName', $seedCustomPipe) | Out-Null
    Invoke-HostCommand configurations.save | Out-Null
    Invoke-HostReload -NextPipe $seedCustomPipe
    Invoke-HostCommand version | Out-Null
    Invoke-HostCommand configurations.reset @('host.kms.pipeName') | Out-Null
    Invoke-HostCommand configurations.save | Out-Null
    Invoke-HostReload -NextPipe 'Seed.KMS.v1'
    Invoke-HostCommand version | Out-Null
    Invoke-HostCommand shutdown | Out-Null
    if (-not $seedProcess.WaitForExit(10000))
    {
        throw 'Host fixture shutdown timed out.'
    }

    $seedBytes = [IO.File]::ReadAllBytes((Join-Path $seedDirectory 'host.v1.bin'))
    $seedHeaderBytes = $seedBytes.Length - [BitConverter]::ToUInt32($seedBytes, 12)

    foreach ($seedCase in @('valid', 'bad-magic', 'bad-version', 'old-version', 'bad-type', 'truncated', 'extended', 'huge-string', 'huge-loggers', 'bad-port'))
    {
        $seedCaseDirectory = Join-Path $seedOutput $seedCase
        New-Item -ItemType Directory -Path $seedCaseDirectory -Force | Out-Null
        [byte[]]$seedCaseBytes = $seedBytes.Clone()
        switch ($seedCase)
        {
            'bad-magic'
            {
                $seedCaseBytes[0] = $seedCaseBytes[0] -bxor 255
            }

            'bad-version'
            {
                [BitConverter]::GetBytes([uint32]3).CopyTo($seedCaseBytes, 8)
            }

            'old-version'
            {
                [BitConverter]::GetBytes([uint32]1).CopyTo($seedCaseBytes, 8)
            }

            'bad-type'
            {
                $seedCaseBytes[16] = $seedCaseBytes[16] -bxor 255
            }

            'truncated'
            {
                $seedCaseBytes = $seedCaseBytes[0..($seedCaseBytes.Length - 2)]
            }

            'extended'
            {
                $seedCaseBytes = $seedCaseBytes + [byte]0
            }

            'huge-string'
            {
                [BitConverter]::GetBytes([uint64]::MaxValue).CopyTo($seedCaseBytes, $seedHeaderBytes + 1)
            }

            'huge-loggers'
            {
                [BitConverter]::GetBytes([uint64]1024).CopyTo($seedCaseBytes, $seedCaseBytes.Length - 8)
            }

            'bad-port'
            {
                $seedCharacters = [BitConverter]::ToUInt64($seedCaseBytes, $seedHeaderBytes + 1)
                $seedPortOffset = $seedHeaderBytes + 1 + 8 + 2 * $seedCharacters
                $seedCaseBytes[$seedPortOffset] = 0
                $seedCaseBytes[$seedPortOffset + 1] = 0
            }
        }

        $seedConfigurationFile = Join-Path $seedCaseDirectory 'host.v1.bin'
        [IO.File]::WriteAllBytes($seedConfigurationFile, $seedCaseBytes)
        $seedBefore = (Get-FileHash -LiteralPath $seedConfigurationFile -Algorithm SHA256).Hash
        $seedReport = Join-Path $seedCaseDirectory 'native.xml'

        & $seedRunner --runtime $seedRuntime --wildcard '*HostConfigurationValidation' --report $seedReport --configuration-directory $seedCaseDirectory --timeout-seconds 30 *> (Join-Path $seedCaseDirectory 'console.log')
        $seedExit = $LASTEXITCODE
        if ($seedCase -eq 'valid')
        {
            if ($seedExit -ne 0)
            {
                throw 'Startup rejected the valid reflected Seed host configuration.'
            }

            [xml]$seedResult = Get-Content -LiteralPath $seedReport -Raw
            if ($seedResult.'test-run'.passed -ne '1' -or $seedResult.'test-run'.failed -ne '0')
            {
                throw 'The host validation self-test did not pass.'
            }

            $seedOtherConfiguration = if ($Configuration -eq 'Debug')
            {
                'Release'
            }
            else
            {
                'Debug'
            }

            $seedOtherRuntime = Join-Path $seedRoot "Artifacts/x64/$seedOtherConfiguration/Runtime/Seed.Runtime.exe"
            $seedOtherRunner = Join-Path $seedRoot "Artifacts/x64/$seedOtherConfiguration/Seed.Managed.TestRunner/Seed.Managed.TestRunner.exe"
            if ((Test-Path -LiteralPath $seedOtherRuntime) -and (Test-Path -LiteralPath $seedOtherRunner))
            {
                $seedOtherReport = Join-Path $seedCaseDirectory 'compatible-runtime.xml'

                & $seedOtherRunner --runtime $seedOtherRuntime --wildcard '*HostConfigurationValidation' --report $seedOtherReport --configuration-directory $seedCaseDirectory --timeout-seconds 30 *> (Join-Path $seedCaseDirectory 'compatible-runtime.log')
                if ($LASTEXITCODE -ne 0)
                {
                    throw "The saved host schema was not compatible with the $seedOtherConfiguration runtime."
                }

                Write-Host "Host schema accepted by the $seedOtherConfiguration runtime."
            }
        }

        elseif ($seedExit -eq 0 -or (Test-Path -LiteralPath $seedReport))
        {
            throw "Startup accepted malformed host configuration: $seedCase"
        }

        if ((Get-FileHash -LiteralPath $seedConfigurationFile -Algorithm SHA256).Hash -cne $seedBefore)
        {
            throw "Startup modified host input: $seedCase"
        }

        Write-Host "Host startup scenario passed: $seedCase"
    }

    Write-Host "Host configuration validation passed: pending edits, explicit save, reload, HTTP disable/port, local pipe and logger persistence. Reports: $seedOutput"
}
finally
{
    if ($seedProcess)
    {
        $seedProcess.Refresh()
        if (-not $seedProcess.HasExited -and $seedProcess.MainModule.FileName -eq $seedRuntime)
        {
            $seedProcess.Kill()
            $seedProcess.WaitForExit()
        }

        $seedProcess.Dispose()
    }

    [Environment]::SetEnvironmentVariable('SEED_CONFIG_DIRECTORY', $seedSavedDirectory)
}
