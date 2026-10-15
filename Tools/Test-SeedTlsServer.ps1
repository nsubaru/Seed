param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [string] $OpenSslPath
)

$ErrorActionPreference = 'Stop'

$seedRoot     = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedRuntime  = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedConsole  = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Console/Seed.Console.exe"
$seedVerifier = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Tls.ServerVerifier/Seed.Tls.ServerVerifier.exe"
$seedOutput   = Join-Path $seedRoot ("Artifacts/tls-server/$Configuration/" + [Guid]::NewGuid().ToString('N'))

if ([Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    throw 'Run TLS server verification under a non-elevated account.'
}

foreach ($seedFile in @($seedRuntime, $seedConsole, $seedVerifier))
{
    if (-not (Test-Path -LiteralPath $seedFile))
    {
        throw "Build the matching Seed solution first: $seedFile"
    }
}

New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null

& (Join-Path $PSScriptRoot 'New-SeedDevelopmentIdentity.ps1') -Directory (Join-Path $seedOutput 'identity')

$seedSavedDirectory = [Environment]::GetEnvironmentVariable('SEED_CONFIG_DIRECTORY')

[Environment]::SetEnvironmentVariable('SEED_CONFIG_DIRECTORY', (Join-Path $seedOutput 'configuration'))

$seedProcess = $null

function Invoke-TlsCommand([string] $Command, [string[]] $Arguments = @())
{
    $seedText = & $seedConsole $Command @Arguments 2>&1
    if ($LASTEXITCODE -ne 0)
    {
        throw "TLS fixture command failed: $Command`n$seedText"
    }

    return $seedText
}

function Invoke-TlsReload
{
    $seedPrevious = [regex]::Match(((Invoke-TlsCommand status.kernel) -join "`n"), 'Composition: (\d+)').Groups[1].Value

    Invoke-TlsCommand reload | Out-Null

    $seedDeadline = [DateTime]::UtcNow.AddSeconds(15)
    do
    {
        # A pending transport operation can still reach its bounded timeout during shutdown.
        # Reconnect attempts inside the overall deadline may briefly find no console endpoint.
        $seedStatus = & $seedConsole status.kernel 2>&1

        $seedCurrent = if ($LASTEXITCODE -eq 0)
        {
            [regex]::Match(($seedStatus -join "`n"), 'Composition: (\d+)').Groups[1].Value
        }
        else
        {
            ''
        }

        if ($seedCurrent -and $seedCurrent -cne $seedPrevious)
        {
            return
        }

        Start-Sleep -Milliseconds 100
    } while ([DateTime]::UtcNow -lt $seedDeadline)
    throw 'HTTPS runtime reload did not finish.'
}

function Invoke-OpenSslChecks([int] $Port)
{
    if (-not $OpenSslPath)
    {
        $seedCommand = Get-Command openssl -ErrorAction SilentlyContinue

        if ($seedCommand)
        {
            $OpenSslPath = $seedCommand.Source
        }
        else
        {
            $OpenSslPath = Join-Path $env:ProgramFiles 'Git/usr/bin/openssl.exe'
        }
    }

    if (-not (Test-Path -LiteralPath $OpenSslPath))
    {
        Write-Host 'OpenSSL peer unavailable; the additional cipher/signature/retry matrix was not run.'
        return
    }

    foreach ($seedCase in @(
            @('tls12-aes128-pkcs1', '-tls1_2', '-cipher', 'ECDHE-RSA-AES128-GCM-SHA256', '-groups', 'X25519', '-sigalgs', 'rsa_pkcs1_sha256'),
            @('tls12-aes256', '-tls1_2', '-cipher', 'ECDHE-RSA-AES256-GCM-SHA384', '-groups', 'X25519'),
            @('tls12-legacy-master', '-tls1_2', '-cipher', 'ECDHE-RSA-AES128-GCM-SHA256', '-groups', 'X25519', '-no_ems'),
            @('tls13-aes128-retry', '-tls1_3', '-ciphersuites', 'TLS_AES_128_GCM_SHA256', '-groups', 'P-256:X25519', '-msg'),
            @('tls13-aes256-retry', '-tls1_3', '-ciphersuites', 'TLS_AES_256_GCM_SHA384', '-groups', 'P-256:X25519', '-msg')
        ))
    {
        $seedInfo = [Diagnostics.ProcessStartInfo]::new($OpenSslPath)

        $seedInfo.UseShellExecute        = $false
        $seedInfo.CreateNoWindow         = $true
        $seedInfo.RedirectStandardInput  = $true
        $seedInfo.RedirectStandardOutput = $true
        $seedInfo.RedirectStandardError  = $true

        foreach ($seedArgument in @('s_client', '-connect', "127.0.0.1:$Port", '-servername', 'localhost', '-verify_hostname', 'localhost',
                '-verify_return_error', '-CAfile', (Join-Path $seedOutput 'identity/root.pem'), '-alpn', 'http/1.1', '-no_ticket', '-brief', '-ign_eof') + $seedCase[1..($seedCase.Count - 1)])
        {
            $seedInfo.ArgumentList.Add($seedArgument)
        }

        $seedPeer = [Diagnostics.Process]::new()
        $seedPeer.StartInfo = $seedInfo

        try
        {
            [void]$seedPeer.Start()

            $seedOutTask   = $seedPeer.StandardOutput.ReadToEndAsync()
            $seedErrorTask = $seedPeer.StandardError.ReadToEndAsync()

            $seedPeer.StandardInput.Write("GET /health HTTP/1.1`r`nHost: localhost`r`nConnection: close`r`n`r`n")
            $seedPeer.StandardInput.Close()

            if (-not $seedPeer.WaitForExit(15000))
            {
                $seedPeer.Kill()
                $seedPeer.WaitForExit()

                throw 'OpenSSL peer deadline exceeded.'
            }

            $seedOutputText = $seedOutTask.GetAwaiter().GetResult() + $seedErrorTask.GetAwaiter().GetResult()
            $seedOutputText | Set-Content -LiteralPath (Join-Path $seedOutput ($seedCase[0] + '.log'))

            if ($seedPeer.ExitCode -ne 0 -or $seedOutputText -notmatch 'Seed is running' -or $seedOutputText -notmatch 'Verification: OK')
            {
                throw "OpenSSL scenario failed: $($seedCase[0]); inspect its log."
            }

            if ($seedCase[0].EndsWith('-retry') -and ($seedOutputText -replace '\s', '') -notmatch 'cf21ad74e59a6111be1d8c021e65b891c2a211167abb8c5e079e09e2c8a8339c')
            {
                throw 'The successful TLS 1.3 scenario did not exercise HelloRetryRequest.'
            }

            Write-Host "OpenSSL scenario passed: $($seedCase[0])"
        }
        finally
        {
            if ($seedPeer.Id -and -not $seedPeer.HasExited)
            {
                $seedPeer.Kill()
                $seedPeer.WaitForExit()
            }

            $seedPeer.Dispose()
        }
    }
}

try
{
    $seedStart     = & $seedConsole --start $seedRuntime version 2>&1
    $seedStartExit = $LASTEXITCODE
    $seedPidMatch  = [regex]::Match(($seedStart -join "`n"), 'Started Seed runtime process (\d+)')

    if ($seedPidMatch.Success)
    {
        $seedProcess = [Diagnostics.Process]::GetProcessById([int]$seedPidMatch.Groups[1].Value)
    }

    if ($seedStartExit -ne 0 -or -not $seedProcess)
    {
        throw "TLS fixture startup failed: $seedStart"
    }

    $seedReservation = [Net.Sockets.TcpListener]::new([Net.IPAddress]::Loopback, 0)

    $seedReservation.Start()

    $seedPort = $seedReservation.LocalEndpoint.Port

    $seedReservation.Stop()

    Invoke-TlsCommand configurations.set @('host.tls.certificateFile', (Join-Path $seedOutput 'identity/chain.pem')) | Out-Null
    Invoke-TlsCommand configurations.set @('host.tls.privateKeyFile', (Join-Path $seedOutput 'identity/key.pem')) | Out-Null
    Invoke-TlsCommand configurations.set @('host.https.port', "$seedPort") | Out-Null
    Invoke-TlsCommand configurations.set @('host.https.enabled', 'true') | Out-Null
    Invoke-TlsCommand configurations.save | Out-Null
    
    Invoke-TlsReload

    & $seedVerifier "$seedPort" (Join-Path $seedOutput 'identity/root.der') *> (Join-Path $seedOutput 'peer.log')
    if ($LASTEXITCODE -ne 0)
    {
        throw "Independent HTTPS client failed; see $seedOutput/peer.log"
    }

    Invoke-OpenSslChecks $seedPort

    # Runtime operations retain their native NTAPI-only module boundary after serving TLS.
    $seedProcess.Refresh()

    $seedModules = @($seedProcess.Modules | ForEach-Object ModuleName | Sort-Object)

    $seedModules | Set-Content -LiteralPath (Join-Path $seedOutput 'modules.txt')

    if (($seedModules -join ',') -ine 'ntdll.dll,Seed.Runtime.exe')
    {
        throw "Unexpected native server modules: $seedModules"
    }

    # Reload while a peer has left an incomplete handshake outstanding.
    $seedStalled = [Net.Sockets.TcpClient]::new()

    try
    {
        $seedStalled.Connect('127.0.0.1', $seedPort)
        $seedBytes = [byte[]]@(22, 3, 3, 0, 32, 1)
        $seedStalled.GetStream().Write($seedBytes)

        Invoke-TlsReload
    }
    finally
    {
        $seedStalled.Dispose()
    }

    & $seedVerifier "$seedPort" (Join-Path $seedOutput 'identity/root.der') *> (Join-Path $seedOutput 'after-reload.log')

    if ($LASTEXITCODE -ne 0)
    {
        throw "HTTPS failed after reload; see $seedOutput/after-reload.log"
    }

    Invoke-TlsCommand logging.view @('100') | Set-Content -LiteralPath (Join-Path $seedOutput 'runtime.log')
    Invoke-TlsCommand shutdown | Out-Null

    if (-not $seedProcess.WaitForExit(10000))
    {
        throw 'HTTPS runtime shutdown did not finish.'
    }

    Write-Host "Independent TLS 1.2/1.3 server, fragmented echo, reflected JSON, failure and reload checks passed. Reports: $seedOutput"
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

$global:LASTEXITCODE = 0
