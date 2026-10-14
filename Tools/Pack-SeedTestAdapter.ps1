param([string] $Version = '0.1.0-local', [string] $NuGetExe, [switch] $NoBuild)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
if ($Version -notmatch '^\d+\.\d+\.\d+(?:-[0-9A-Za-z.-]+)?$')
{
    throw 'Use a NuGet semantic version.'
}

if (-not $NuGetExe)
{
    $seedNugetCommand = Get-Command nuget.exe -ErrorAction SilentlyContinue
    if ($seedNugetCommand)
    {
        $NuGetExe = $seedNugetCommand.Source
    }
}

if (-not $NuGetExe -or -not (Test-Path -LiteralPath $NuGetExe))
{
    throw 'Supply -NuGetExe from the official NuGet distribution: https://www.nuget.org/downloads.'
}

if (-not $NoBuild)
{
    $seedVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    $seedMsbuild = & $seedVswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/**/Bin/MSBuild.exe' | Select-Object -First 1

    & $seedMsbuild (Join-Path $seedRoot 'Seed.slnx') /m /restore /nologo /p:Platform=x64 /p:Configuration=Release
    if ($LASTEXITCODE -ne 0)
    {
        throw 'Release build failed.'
    }
}

& (Join-Path $PSScriptRoot 'Write-SeedDependencyInventory.ps1')
$seedPackages = Join-Path $seedRoot 'Artifacts/packages'
New-Item -ItemType Directory -Path $seedPackages -Force | Out-Null

& $NuGetExe pack (Join-Path $seedRoot 'Tools/Seed.Testing.TestAdapter/Seed.Testing.TestAdapter.nuspec') -BasePath $seedRoot -Version $Version -OutputDirectory $seedPackages -NonInteractive
if ($LASTEXITCODE -ne 0)
{
    throw 'Test adapter package creation failed.'
}

$seedPackage = Join-Path $seedPackages "Seed.Testing.TestAdapter.$Version.nupkg"
$seedArchive = [IO.Compression.ZipFile]::OpenRead($seedPackage)

try
{
    $seedEntries = @($seedArchive.Entries.FullName)
    $seedRequiredEntries = @('tools/adapter/Seed.Testing.TestAdapter.dll',
        'build/native/Seed.Testing.TestAdapter.props', 'build/native/Seed.Testing.TestAdapter.targets',
        'README.md', 'LICENSE', 'NOTICE', 'THIRD-PARTY-NOTICES.md', 'licenses/LICENSE.phnt', 'dependencies.json')
    $seedHostDirectory = Join-Path $seedRoot 'Artifacts/x64/Release/Seed.Testing.TestAdapter/host'
    $seedRequiredEntries += @(Get-ChildItem -LiteralPath $seedHostDirectory -File | ForEach-Object `
        {
            'tools/adapter/host/' + $_.Name
        })

    foreach ($seedRequired in $seedRequiredEntries)
    {
        if ($seedRequired -cnotin $seedEntries)
        {
            throw "Invalid package layout; missing: $seedRequired"
        }
    }

    Write-Output "Verified adapter package layout: $($seedEntries.Count) entries."
}
finally
{
    $seedArchive.Dispose()
}

Get-FileHash -LiteralPath $seedPackage -Algorithm SHA256 | Format-List

Write-Output "Created local package: $seedPackage"
