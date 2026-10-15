param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [string] $ConfigurationDirectory
)

$ErrorActionPreference = 'Stop'

$seedRoot        = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedRuntime     = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedConsole     = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Console/Seed.Console.exe"
$seedStarter     = Join-Path $seedRoot "Artifacts/x64/$Configuration/API/Starter/Seed.Starter.dll"
$seedRoundTrip   = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Managed.RoundTrip/Seed.Managed.RoundTrip.exe"
$seedKmsVerifier = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.KMS.Verifier/Seed.KMS.Verifier.exe"
$seedOutput      = Join-Path $seedRoot 'Artifacts/integration'

$seedConfigurationDirectory = if ($ConfigurationDirectory)
{
    [IO.Path]::GetFullPath($ConfigurationDirectory)
}
else
{
    Join-Path $seedOutput ('configuration-' + [Guid]::NewGuid().ToString('N'))
}

New-Item -ItemType Directory -Path $seedOutput, $seedConfigurationDirectory -Force | Out-Null

if ([Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    throw 'Run standard Seed integration under a non-elevated account.'
}

foreach ($seedFile in @($seedRuntime, $seedConsole, $seedStarter, $seedRoundTrip, $seedKmsVerifier))
{
    if (-not (Test-Path -LiteralPath $seedFile))
    {
        throw "Build the matching solution first: $seedFile"
    }
}

Add-Type @'
using System.Runtime.InteropServices;
public static class SeedIntegrationStarter {
    [DllImport("Seed.Starter.dll", CharSet=CharSet.Unicode, CallingConvention=CallingConvention.Cdecl)]
    public static extern int SeedStartRuntime(string path, string commandLine, out ulong processId);
}
'@
$seedLibrary = [Runtime.InteropServices.NativeLibrary]::Load($seedStarter)

$seedEnvironmentNames = @('SEED_CONFIG_DIRECTORY', 'SEED_RUN_TESTS', 'SEED_TEST_REPORT', 'SEED_RTTI_EXPORT',
    'SEED_HTTP_URL', 'SEED_HTTP_OUTPUT', 'SEED_HTTP_TRUST_ROOT', 'SEED_HTTP_PROTOCOL', 'SEED_HTTP_CONNECT_ADDRESS')

$seedEnvironment = @{}

foreach ($seedName in $seedEnvironmentNames)
{
    $seedEnvironment[$seedName] = [Environment]::GetEnvironmentVariable($seedName)
    [Environment]::SetEnvironmentVariable($seedName, $null)
}

[Environment]::SetEnvironmentVariable('SEED_CONFIG_DIRECTORY', $seedConfigurationDirectory)
[UInt64]$seedProcessId = 0

$seedProcess = $null

function Invoke-SeedCommand([string] $Command, [string[]] $Arguments = @(), [int] $ExpectedExit = 0)
{
    $seedCommandOutput = & $seedConsole $Command @Arguments 2>&1

    if ($LASTEXITCODE -ne $ExpectedExit)
    {
        throw "KMS command failed: $Command (exit $LASTEXITCODE)`n$seedCommandOutput"
    }

    return $seedCommandOutput
}

function Assert-SeedHttpExchange([string] $Request, [int] $Status, [string] $Body = '')
{
    $seedSocket = [Net.Sockets.TcpClient]::new()

    try
    {
        $seedSocket.Connect('127.0.0.1', 8080)

        $seedStream = $seedSocket.GetStream()

        $seedStream.ReadTimeout = 5000
        $seedStream.WriteTimeout = 5000

        $seedRequestBytes = [Text.Encoding]::ASCII.GetBytes($Request)

        $seedStream.Write($seedRequestBytes, 0, $seedRequestBytes.Length)

        $seedResponseBytes = [IO.MemoryStream]::new()

        try
        {
            $seedBuffer = [byte[]]::new(4096)

            while (($seedRead = $seedStream.Read($seedBuffer, 0, $seedBuffer.Length)) -gt 0)
            {
                $seedResponseBytes.Write($seedBuffer, 0, $seedRead)

                if ($seedResponseBytes.Length -gt 65536)
                {
                    throw 'HTTP test response exceeded its limit.'
                }
            }

            $seedResponseText = [Text.Encoding]::UTF8.GetString($seedResponseBytes.ToArray())

            if (-not $seedResponseText.StartsWith("HTTP/1.1 $Status ", [StringComparison]::Ordinal))
            {
                throw "Unexpected HTTP status: $seedResponseText"
            }

            if ($Status -eq 200 -and $seedResponseText.Split(@("`r`n`r`n"), 2, [StringSplitOptions]::None)[1] -cne $Body)
            {
                throw 'Incorrect HTTP response body.'
            }
        }
        finally
        {
            $seedResponseBytes.Dispose()
        }
    }
    finally
    {
        $seedSocket.Dispose()
    }
}

function Assert-RejectsPacket([byte[]] $Packet)
{
    $seedPipe = [IO.Pipes.NamedPipeClientStream]::new('.', 'Seed.KMS.v1', [IO.Pipes.PipeDirection]::InOut, [IO.Pipes.PipeOptions]::Asynchronous)

    try
    {
        $seedPipe.Connect(5000)
        $seedPipe.ReadMode = [IO.Pipes.PipeTransmissionMode]::Message
        $seedBuffer = [byte[]]::new(65535)

        do
        {
            $seedRead = $seedPipe.Read($seedBuffer, 0, $seedBuffer.Length)
        } while (-not $seedPipe.IsMessageComplete)

        if ($seedRead -lt 5)
        {
            throw 'Missing KMS initialization marker.'
        }

        $seedPipe.Write($Packet, 0, $Packet.Length)
        $seedDeadline = [Threading.CancellationTokenSource]::new([TimeSpan]::FromSeconds(5))

        try
        {
            $seedRead = $seedPipe.ReadAsync($seedBuffer, 0, $seedBuffer.Length, $seedDeadline.Token).GetAwaiter().GetResult()

            if ($seedRead -ne 0)
            {
                throw 'Malformed KMS packet was accepted.'
            }
        }
        catch [IO.IOException]
        {
        }
        finally
        {
            $seedDeadline.Dispose()
        }
    }
    finally
    {
        $seedPipe.Dispose()
    }

    Invoke-SeedCommand version | Out-Null
}

try
{
    $seedStatus = [SeedIntegrationStarter]::SeedStartRuntime($seedRuntime, 'Seed.Runtime.exe', [ref]$seedProcessId)
    if ($seedStatus -lt 0)
    {
        throw ('Native startup failed: {0:X8}' -f $seedStatus)
    }

    $seedProcess = [Diagnostics.Process]::GetProcessById([int]$seedProcessId)

    Invoke-SeedCommand version
    Invoke-SeedCommand status.kernel
    Invoke-SeedCommand exit | Out-Null
    Invoke-SeedCommand version | Out-Null
    Invoke-SeedCommand unavailable.command -ExpectedExit 1 | Out-Null

    $seedCommands = (Invoke-SeedCommand commands) -join "`n"

    if ($seedCommands -match '(?i)auth|login|password')
    {
        throw 'Unexpected authentication commands.'
    }

    $seedExpectedCommands = @(
        'help', 
        'commands',
        'version', 
        'exit', 
        'status.kernel',
        'shutdown', 
        'reload',
        'configurations.list', 
        'configurations.get', 
        'configurations.set', 
        'configurations.save', 
        'configurations.reset',
        'logging.view', 
        'logging.get', 
        'logging.set', 
        'logging.follow', 
        'tests.list', 
        'tests.run', 
        'tests.result.store',
        'di.rtti.list', 
        'di.rtti.save', 
        'network.interfaces', 
        'network.dns.resolve',
        'network.dhcp.start', 
        'network.dhcp.status', 
        'network.dhcp.stop')

    $seedActualCommands = @($seedCommands -split "`r?`n" | Where-Object `
    {
        $_
    })

    if ($seedActualCommands.Count -ne $seedExpectedCommands.Count -or
        @(Compare-Object ($seedExpectedCommands | Sort-Object) ($seedActualCommands | Sort-Object)).Count -ne 0)
    {
        throw 'KMS command discovery does not match the implemented allowlist.'
    }

    $seedConfigurationName = 'core.memory.stringReserveCharacters'

    Invoke-SeedCommand configurations.list
    Invoke-SeedCommand configurations.set @($seedConfigurationName, '73') | Out-Null

    if ((Invoke-SeedCommand configurations.get @($seedConfigurationName)) -notmatch '= 73 ')
    {
        throw 'Live configuration update failed.'
    }

    Invoke-SeedCommand configurations.set @($seedConfigurationName, '-1') -ExpectedExit 1 | Out-Null
    Invoke-SeedCommand configurations.set @($seedConfigurationName, '18446744073709551616') -ExpectedExit 1 | Out-Null

    if ((Invoke-SeedCommand configurations.get @($seedConfigurationName)) -notmatch '= 73 ')
    {
        throw 'Rejected configuration modified live state.'
    }

    Invoke-SeedCommand configurations.save | Out-Null
    Invoke-SeedCommand reload | Out-Null

    if ((Invoke-SeedCommand configurations.get @($seedConfigurationName)) -notmatch '= 73 ')
    {
        throw 'Saved configuration did not survive reload.'
    }

    Invoke-SeedCommand configurations.reset @($seedConfigurationName) | Out-Null

    if ((Invoke-SeedCommand configurations.get @($seedConfigurationName)) -notmatch '= 50 ')
    {
        throw 'Compiled configuration defaults were not restored.'
    }

    Invoke-SeedCommand configurations.save | Out-Null

    if ((Invoke-SeedCommand network.dhcp.status) -notmatch 'DHCP maintenance is idle')
    {
        throw 'DHCP started without an explicit eligible-interface request.'
    }

    foreach ($seedRejectedIndex in @('0', '1', '4294967296'))
    {
        Invoke-SeedCommand network.dhcp.start @($seedRejectedIndex) -ExpectedExit 1 | Out-Null
    }

    Invoke-SeedCommand network.dhcp.status @('unexpected') -ExpectedExit 1 | Out-Null
    Invoke-SeedCommand network.dhcp.stop @('0') | Out-Null

    if ((Invoke-SeedCommand network.dhcp.status) -notmatch 'DHCP maintenance is idle')
    {
        throw 'Rejected DHCP selection created a maintenance operation.'
    }

    Invoke-SeedCommand logging.get
    Invoke-SeedCommand logging.set @('28', 'HTTP client') | Out-Null

    if ((Invoke-SeedCommand logging.get @('HTTP client')) -notmatch '= 28')
    {
        throw 'Live logger change failed.'
    }

    Invoke-SeedCommand logging.set @('32', 'HTTP client') -ExpectedExit 1 | Out-Null
    Invoke-SeedCommand logging.set @('31', 'HTTP client') | Out-Null

    & $seedKmsVerifier

    if ($LASTEXITCODE -ne 0)
    {
        throw 'KMS streaming and live logging verification failed.'
    }

    Invoke-SeedCommand tests.run @('*Crypt*')
    $seedReport = Join-Path $seedOutput 'kms-native.xml'
    Invoke-SeedCommand tests.result.store @($seedReport)

    [xml]$seedResults = Get-Content -LiteralPath $seedReport -Raw

    if ($seedResults.'test-run'.failed -ne '0' -or [int]$seedResults.'test-run'.testcasecount -le 0)
    {
        throw 'Native cryptography KMS report failed.'
    }

    $seedSelectedTypes = @('Seed::Core::Testing::Test::Crypt::Aes256::Aes256BlockBoundaryTest', 'Seed::Core::Testing::Test::Configurations::SnapshotValidationTest')

    if ((Invoke-SeedCommand tests.run $seedSelectedTypes) -notmatch '2 native tests passed')
    {
        throw 'Multiple explicit KMS test selections failed.'
    }

    Invoke-SeedCommand tests.run @($seedSelectedTypes[0], 'Seed::Core::Testing::UnavailableTest') -ExpectedExit 1 | Out-Null
    Invoke-SeedCommand tests.run @('"unterminated') -ExpectedExit 1 | Out-Null
    Invoke-SeedCommand logging.view | Out-Null
    Invoke-SeedCommand network.interfaces

    $seedRtti = Join-Path $seedOutput 'seed-rtti.dat'

    Invoke-SeedCommand di.rtti.save @($seedRtti)

    & $seedRoundTrip $seedRtti 'http://127.0.0.1:8080/' --verify-malformed

    if ($LASTEXITCODE -ne 0)
    {
        throw 'Managed/native interoperability failed.'
    }

    $seedHttp = [Net.Http.HttpClient]::new()

    try
    {
        if ($seedHttp.GetStringAsync('http://127.0.0.1:8080/health').GetAwaiter().GetResult() -ne "Seed is running`n")
        {
            throw 'HTTP health failed.'
        }

        $seedBody = [Net.Http.StringContent]::new('Seed echo')
        $seedResponse = $seedHttp.PostAsync('http://127.0.0.1:8080/echo', $seedBody).GetAwaiter().GetResult()

        try
        {
            if (-not $seedResponse.IsSuccessStatusCode -or $seedResponse.Content.ReadAsStringAsync().GetAwaiter().GetResult() -ne 'Seed echo')
            {
                throw 'HTTP echo failed.'
            }
        }
        finally
        {
            $seedResponse.Dispose()
            $seedBody.Dispose()
        }
    }
    finally
    {
        $seedHttp.Dispose()
    }

    Assert-SeedHttpExchange "POST /echo HTTP/1.1`r`nHost: localhost`r`nTransfer-Encoding: chunked`r`n`r`n4`r`nSeed`r`n5`r`n echo`r`n0`r`n`r`n" 200 'Seed echo'
    Assert-SeedHttpExchange "POST /echo HTTP/1.1`r`nHost: localhost`r`nContent-Length: 4`r`nContent-Length: 4`r`n`r`nsame" 200 'same'

    foreach ($seedFields in @("Content-Length: 4`r`nContent-Length: 5", "Transfer-Encoding: chunked`r`nContent-Length: 4", 'Content-Length: 18446744073709551615', 'Content-Length : 4'))
    {
        Assert-SeedHttpExchange "POST /echo HTTP/1.1`r`nHost: localhost`r`n$seedFields`r`n`r`nsame" 400
    }

    Assert-RejectsPacket ([byte[]]@(1, 6, 0))
    Assert-RejectsPacket ([byte[]]@(1, 6, 0, 0, 0, 0))
    Assert-RejectsPacket ([byte[]]@(1, 2, 0, 0, 0))
    Assert-RejectsPacket ([byte[]]@(1, 6, 0, 0, 0, 0, 216))
    Assert-RejectsPacket ([byte[]]::new(65536))
    
    $seedModules = @($seedProcess.Modules | Select-Object -ExpandProperty ModuleName)

    if ($seedModules.Count -ne 2 -or $seedModules -notcontains 'ntdll.dll')
    {
        throw "Unexpected native modules: $seedModules"
    }

    Invoke-SeedCommand shutdown

    if (-not $seedProcess.WaitForExit(10000))
    {
        throw 'Orderly shutdown timed out.'
    }

    Write-Output "Saved configuration fixture: $(Join-Path $seedConfigurationDirectory 'core.v1.bin')"
    Write-Output 'Seed local KMS, native cryptography, RTTI, managed JSON interoperability, HTTP, packet limits and NTAPI module checks passed.'
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

    foreach ($seedName in $seedEnvironmentNames)
    {
        [Environment]::SetEnvironmentVariable($seedName, $seedEnvironment[$seedName])
    }

    [Runtime.InteropServices.NativeLibrary]::Free($seedLibrary)
}
