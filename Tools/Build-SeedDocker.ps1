param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Release',
    [ValidateSet('runtime', 'test', 'build', 'artifacts')][string] $Target = 'runtime',
    [ValidateSet('Native', 'Console', 'Windows')][string] $Subsystem = 'Native',
    [string] $Image = 'seed:runtime-local',
    [string] $BuildToolsImage = 'seed-build-tools:vs2026',
    [string] $SdkImage = 'mcr.microsoft.com/dotnet/sdk:10.0-windowsservercore-ltsc2025',
    [string] $RuntimeImage = 'mcr.microsoft.com/dotnet/runtime:10.0-windowsservercore-ltsc2025',
    [ValidateSet('process', 'hyperv')][string] $Isolation = 'process',
    [string] $BuildMemory,
    [switch] $SkipBuildTools
)

$ErrorActionPreference = 'Stop'

$seedRoot   = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedDocker = Get-Command docker.exe -ErrorAction Stop
$seedOs     = & $seedDocker.Source info --format '{{.OSType}}'

if ($LASTEXITCODE -ne 0 -or $seedOs -cne 'windows')
{
    throw 'Use a running Docker Windows-container engine.'
}

if (-not $BuildMemory -and $Isolation -eq 'hyperv')
{
    $BuildMemory = '32g'
}

$seedResourceArguments = @()
if ($BuildMemory)
{
    $seedResourceArguments = @('--memory', $BuildMemory)
}

if (-not $SkipBuildTools)
{
    & $seedDocker.Source build @seedResourceArguments --isolation $Isolation --file (Join-Path $seedRoot 'Docker/BuildTools.Dockerfile') --build-arg "SDK_IMAGE=$SdkImage" --tag $BuildToolsImage $seedRoot

    if ($LASTEXITCODE -ne 0)
    {
        throw 'Seed Windows build-tools image creation failed.'
    }
}

& $seedDocker.Source build @seedResourceArguments --isolation $Isolation --file (Join-Path $seedRoot 'Docker/Seed.Dockerfile') `
    --target $Target --build-arg "BUILD_TOOLS_IMAGE=$BuildToolsImage" --build-arg "RUNTIME_IMAGE=$RuntimeImage" `
    --build-arg "CONFIGURATION=$Configuration" --build-arg "SUBSYSTEM=$Subsystem" --tag $Image $seedRoot
    
if ($LASTEXITCODE -ne 0)
{
    throw 'Seed Docker source build failed.'
}

Write-Output "Seed $Target image built: $Image"
