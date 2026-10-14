param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'
if ($env:GITHUB_ACTIONS -ceq 'true')
{
    $seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
    $seedCiHome = Join-Path $seedRoot 'Artifacts/ci-home'
    $seedCiTemp = Join-Path $seedRoot 'Artifacts/ci-temp'
    New-Item -ItemType Directory -Path $seedCiHome, $seedCiTemp -Force | Out-Null
    [Environment]::SetEnvironmentVariable('DOTNET_CLI_HOME', $seedCiHome)
    [Environment]::SetEnvironmentVariable('NUGET_PACKAGES', (Join-Path $seedRoot 'Artifacts/ci-packages'))
    [Environment]::SetEnvironmentVariable('TEMP', $seedCiTemp)
    [Environment]::SetEnvironmentVariable('TMP', $seedCiTemp)
}

& (Join-Path $PSScriptRoot 'Test-SeedSource.ps1')

& (Join-Path $PSScriptRoot 'Test-SeedE2E.ps1') -Configuration $Configuration -NoBuild

& (Join-Path $PSScriptRoot 'Test-SeedTestAdapter.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Tests -Configuration $Configuration -NoBuild

& (Join-Path $PSScriptRoot 'Test-SeedIntegration.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedConfigurationStartup.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedHostConfiguration.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedRsaSigning.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsIdentity.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsClient.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Test-SeedTlsServer.ps1') -Configuration $Configuration

& (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Documentation

& (Join-Path $PSScriptRoot 'Test-SeedDocumentation.ps1')

& (Join-Path $PSScriptRoot 'Write-SeedDependencyInventory.ps1')
