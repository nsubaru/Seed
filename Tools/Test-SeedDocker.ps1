param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Release',
    [ValidateSet('Native', 'Console', 'Windows')][string] $Subsystem = 'Native',
    [string] $Image = 'seed:tests-local',
    [string] $BuildToolsImage = 'seed-build-tools:vs2026',
    [ValidateSet('process', 'hyperv')][string] $Isolation = 'process',
    [string] $BuildMemory,
    [string] $TestMemory,
    [string] $Class,
    [string] $Method,
    [string] $NativeWildcard = '*',
    [switch] $SkipNativeTests,
    [switch] $SkipBuildTools,
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
    & (Join-Path $PSScriptRoot 'Build-SeedDocker.ps1') -Configuration $Configuration -Target test -Image $Image `
        -BuildToolsImage $BuildToolsImage -Isolation $Isolation -BuildMemory $BuildMemory -Subsystem $Subsystem -SkipBuildTools:$SkipBuildTools
}

$seedName = 'seed-tests-' + [Guid]::NewGuid().ToString('N')
$seedOutput = Join-Path $seedRoot "Artifacts/docker/$Configuration/$seedName"
New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null
$seedArguments = @('-Configuration', $Configuration, '-NativeWildcard', $NativeWildcard)
if ($Class)
{
    $seedArguments += @('-Class', $Class)
}

if ($Method)
{
    $seedArguments += @('-Method', $Method)
}

if ($SkipNativeTests)
{
    $seedArguments += '-SkipNativeTests'
}

$seedCreated = $false
if (-not $TestMemory -and $Isolation -eq 'hyperv')
{
    $TestMemory = '8g'
}

$seedResourceArguments = @()
if ($TestMemory)
{
    $seedResourceArguments = @('--memory', $TestMemory)
}

try
{
    & docker create @seedResourceArguments --name $seedName --label "seed.test-run=$seedName" --isolation $Isolation $Image @seedArguments | Out-Null
    if ($LASTEXITCODE -ne 0)
    {
        throw 'Cannot create Seed test container.'
    }

    $seedCreated = $true

    & docker start -a $seedName
    $seedAttachExit = $LASTEXITCODE
    $seedState = (& docker inspect $seedName --format '{{json .State}}') | ConvertFrom-Json
    if ($LASTEXITCODE -ne 0 -or $seedState.Running)
    {
        throw 'Seed test container has not completed.'
    }

    if ($seedState.ExitCode -ne 0)
    {
        throw "Seed container tests failed with exit $($seedState.ExitCode); artifacts: $seedOutput"
    }

    if ($seedAttachExit -ne 0)
    {
        throw "Docker could not attach to the Seed test container; artifacts: $seedOutput"
    }
}
finally
{
    if ($seedCreated)
    {
        & docker logs $seedName *> (Join-Path $seedOutput 'container.log')

        & docker cp "${seedName}:C:/results" (Join-Path $seedOutput 'results')
        $seedCopyExit = $LASTEXITCODE

        & docker rm -f $seedName | Out-Null
        if ($seedCopyExit -ne 0)
        {
            Write-Warning "Cannot copy all container reports; inspect $seedOutput/container.log."
        }
    }
}

$seedReports = @(Get-ChildItem -LiteralPath $seedOutput -Filter '*.trx' -Recurse -File)
if ($seedReports.Count -ne 1)
{
    throw 'Docker E2E run did not preserve exactly one TRX report.'
}

if (-not $SkipNativeTests -and -not (Test-Path -LiteralPath (Join-Path $seedOutput 'results/native.xml')))
{
    throw 'Docker native report was not preserved.'
}

Write-Output "Seed Docker verification passed; reports: $seedOutput"
