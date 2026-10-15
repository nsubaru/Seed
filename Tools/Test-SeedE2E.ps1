param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [string] $Class,
    [string] $Method,
    [string] $OutputDirectory,
    [switch] $NoBuild
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
if ($Method -and -not $Class)
{
    throw '-Method requires -Class.'
}

if (-not $NoBuild)
{
    $seedVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $seedMsbuild = & $seedVswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/**/Bin/MSBuild.exe' | Select-Object -First 1

    if (-not $seedMsbuild)
    {
        throw 'Visual Studio MSBuild is unavailable.'
    }

    & $seedMsbuild (Join-Path $seedRoot 'Seed.slnx') /m /restore /nologo /p:Platform=x64 "/p:Configuration=$Configuration"

    if ($LASTEXITCODE -ne 0)
    {
        throw 'Seed build failed.'
    }
}

$seedRuntime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedTests   = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.E2ETests/Seed.E2ETests.exe"

$seedOutputRoot = if ($OutputDirectory)
{
    [IO.Path]::GetFullPath($OutputDirectory)
}
else
{
    Join-Path $seedRoot "Artifacts/e2e/$Configuration"
}

$seedOutput = Join-Path $seedOutputRoot ([Guid]::NewGuid().ToString('N'))

New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null

$seedPreviousRuntime = [Environment]::GetEnvironmentVariable('SEED_E2E_RUNTIME')
$seedPreviousOutput = [Environment]::GetEnvironmentVariable('SEED_E2E_OUTPUT')

try
{
    [Environment]::SetEnvironmentVariable('SEED_E2E_RUNTIME', $seedRuntime)
    [Environment]::SetEnvironmentVariable('SEED_E2E_OUTPUT', $seedOutput)

    $seedArguments = @('--timeout', '15m', '--report-trx', '--results-directory', $seedOutput)

    if ($Class)
    {
        $seedFilter = if ($Method)
        {
            "/$Class/$Method"
        }
        else
        {
            "/$Class/*"
        }

        # xUnit v3's native query filter: assembly/namespace/class/method.
        $seedArguments += @('--filter-query', "/*/*$seedFilter")
    }

    & $seedTests @seedArguments

    if ($LASTEXITCODE -ne 0)
    {
        throw "Seed E2E tests failed; artifacts: $seedOutput"
    }

    $seedReports = @(Get-ChildItem -LiteralPath $seedOutput -Filter '*.trx' -File)

    if ($seedReports.Count -ne 1)
    {
        throw 'Expected one complete E2E TRX report.'
    }

    [xml]$seedReport = Get-Content -LiteralPath $seedReports[0].FullName -Raw
    
    if ([int]$seedReport.TestRun.ResultSummary.Counters.total -le 0 -or [int]$seedReport.TestRun.ResultSummary.Counters.failed -ne 0)
    {
        throw 'The E2E selection was empty or contained failed tests.'
    }

    Write-Output "Seed E2E report: $($seedReports[0].FullName)"
}
finally
{
    [Environment]::SetEnvironmentVariable('SEED_E2E_RUNTIME', $seedPreviousRuntime)
    [Environment]::SetEnvironmentVariable('SEED_E2E_OUTPUT', $seedPreviousOutput)
}
