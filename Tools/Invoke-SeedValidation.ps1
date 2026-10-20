param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [switch] $TlsProbe
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path

if ($env:GITHUB_ACTIONS -ceq 'true')
{
    $seedCiHome = Join-Path $seedRoot 'Artifacts/ci-home'
    $seedCiTemp = Join-Path $seedRoot 'Artifacts/ci-temp'
    New-Item -ItemType Directory -Path $seedCiHome, $seedCiTemp -Force | Out-Null
    [Environment]::SetEnvironmentVariable('DOTNET_CLI_HOME', $seedCiHome)
    [Environment]::SetEnvironmentVariable('NUGET_PACKAGES', (Join-Path $seedRoot 'Artifacts/ci-packages'))
    [Environment]::SetEnvironmentVariable('TEMP', $seedCiTemp)
    [Environment]::SetEnvironmentVariable('TMP', $seedCiTemp)
}

if ($TlsProbe)
{
    try
    {
        Write-Host ('Schannel cipher suites: ' + ((Get-TlsCipherSuite).Name -join ', '))
        Write-Host ('Schannel curves: ' + ((Get-TlsEccCurve) -join ', '))
        Write-Host ('Windows FIPS policy: ' + (Get-ItemPropertyValue -LiteralPath 'HKLM:\SYSTEM\CurrentControlSet\Control\Lsa\FipsAlgorithmPolicy' -Name Enabled))
    }
    catch
    {
        Write-Host "Schannel capability diagnostics unavailable: $_"
    }

    $seedPeer = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Tls.TestPeer/Seed.Tls.TestPeer.exe"
    & $seedPeer probe (Join-Path $seedRoot 'Artifacts/tls-negotiation')

    if ($LASTEXITCODE -ne 0)
    {
        throw 'TLS ClientHello negotiation probe failed.'
    }

    return
}

& (Join-Path $PSScriptRoot 'Test-SeedSource.ps1')

& (Join-Path $PSScriptRoot 'Test-SeedE2E.ps1') -Configuration $Configuration -NoBuild

& (Join-Path $PSScriptRoot 'Test-SeedTestAdapter.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Tests -Configuration $Configuration -NoBuild

$seedConfigurationDirectory = Join-Path $seedRoot ("Artifacts/integration/$Configuration/configuration-" + [Guid]::NewGuid().ToString('N'))

& (Join-Path $PSScriptRoot 'Test-SeedIntegration.ps1') -Configuration $Configuration -ConfigurationDirectory $seedConfigurationDirectory

& (Join-Path $PSScriptRoot 'Test-SeedConfigurationStartup.ps1') -Configuration $Configuration -FixturePath (Join-Path $seedConfigurationDirectory 'core.v1.bin')

& (Join-Path $PSScriptRoot 'Test-SeedHostConfiguration.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedRsaSigning.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsIdentity.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsClient.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsServer.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Documentation -Configuration $Configuration -NoBuild

& (Join-Path $PSScriptRoot 'Test-SeedDocumentation.ps1')

& (Join-Path $PSScriptRoot 'Write-SeedDependencyInventory.ps1')
