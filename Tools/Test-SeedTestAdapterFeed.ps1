param(
    [Parameter(Mandatory)][string] $Source,
    [Parameter(Mandatory)][string] $Version,
    [Parameter(Mandatory)][string] $NuGetExe,
    [string] $ApiKey = 'az'
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedSourceUri = [Uri]$Source
if ($seedSourceUri.Scheme -ne 'https' -or $seedSourceUri.UserInfo)
{
    throw 'Use an HTTPS feed URL without embedded credentials.'
}

if ($Version -notmatch '^\d+\.\d+\.\d+-[0-9A-Za-z.-]+$')
{
    throw 'Feed tests require a unique prerelease version.'
}

$seedPackage = Join-Path $seedRoot "Artifacts/packages/Seed.Testing.TestAdapter.$Version.nupkg"
if (-not (Test-Path -LiteralPath $seedPackage))
{
    throw 'Create and inspect this exact prerelease package first with Pack-SeedTestAdapter.ps1.'
}

# Calling this script explicitly authorizes publication of this package to Source.
# Supply authentication through NuGet's configured credential provider/environment.
& $NuGetExe push $seedPackage -Source $Source -ApiKey $ApiKey -NonInteractive
if ($LASTEXITCODE -ne 0)
{
    throw 'Feed publication failed; inspect authentication locally before retrying.'
}

$seedRestored = Join-Path $seedRoot "Artifacts/feed-test/$Version/$([Guid]::NewGuid().ToString('N'))"
New-Item -ItemType Directory -Path $seedRestored -Force | Out-Null

& $NuGetExe install Seed.Testing.TestAdapter -Version $Version -Source $Source -OutputDirectory $seedRestored -DirectDownload -NoCache -NonInteractive -PackageSaveMode nupkg
if ($LASTEXITCODE -ne 0)
{
    throw 'Fresh package restore from the test feed failed.'
}

$seedInstalled = Join-Path $seedRestored "Seed.Testing.TestAdapter.$Version"
$seedArchive = Join-Path $seedInstalled "Seed.Testing.TestAdapter.$Version.nupkg"
if ((Get-FileHash -LiteralPath $seedArchive -Algorithm SHA256).Hash -ne (Get-FileHash -LiteralPath $seedPackage -Algorithm SHA256).Hash)
{
    throw 'Restored package bytes differ from the tested local archive.'
}

& (Join-Path $PSScriptRoot 'Test-SeedTestAdapter.ps1') -Configuration Release -AdapterDirectory (Join-Path $seedInstalled 'tools/adapter')

Write-Output "Test feed package restore and native execution passed; artifacts: $seedRestored"
