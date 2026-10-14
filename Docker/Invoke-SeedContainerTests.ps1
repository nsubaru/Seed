param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = $env:SEED_BUILD_CONFIGURATION,
    [string] $Class,
    [string] $Method,
    [string] $NativeWildcard = '*',
    [switch] $SkipNativeTests
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedOutput = 'C:/results'
New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null
$seedIdentity = [Security.Principal.WindowsIdentity]::GetCurrent()
$seedElevated = [Security.Principal.WindowsPrincipal]::new($seedIdentity).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
"User: $($seedIdentity.Name)`nElevated: $seedElevated" | Set-Content -LiteralPath (Join-Path $seedOutput 'identity.txt') -Encoding utf8
if ($seedElevated)
{
    throw 'Seed container tests must run as ContainerUser.'
}

$seedArguments =
@{ Configuration = $Configuration
    NoBuild = $true
    OutputDirectory = (Join-Path $seedOutput 'e2e')
}

if ($Class)
{
    $seedArguments.Class = $Class
}

if ($Method)
{
    $seedArguments.Method = $Method
}

& (Join-Path $seedRoot 'Tools/Test-SeedE2E.ps1') @seedArguments
if (-not $SkipNativeTests)
{
    $seedArtifacts = Join-Path $seedRoot "Artifacts/x64/$Configuration"

    & (Join-Path $seedArtifacts 'Seed.Managed.TestRunner/Seed.Managed.TestRunner.exe') `
        --runtime (Join-Path $seedArtifacts 'Runtime/Seed.Runtime.exe') `
        --wildcard $NativeWildcard --report (Join-Path $seedOutput 'native.xml') `
        --configuration-directory (Join-Path $seedOutput 'native-configuration')
    if ($LASTEXITCODE -ne 0)
    {
        throw 'Container native tests failed; inspect native.xml.'
    }
}

Copy-Item -LiteralPath (Join-Path $seedRoot 'Artifacts/native-imports.txt') -Destination (Join-Path $seedOutput 'native-imports.txt')
Copy-Item -LiteralPath (Join-Path $seedRoot 'Artifacts/container-build.json') -Destination (Join-Path $seedOutput 'container-build.json')

Write-Output 'Seed container E2E and selected native verification passed.'
