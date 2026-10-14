param([string] $Output)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
if (-not $Output)
{
    $Output = Join-Path $seedRoot 'Artifacts/dependencies.json'
}

$seedIntermediate = Join-Path $seedRoot 'Artifacts/Intermediate'
if (-not (Test-Path -LiteralPath $seedIntermediate))
{
    throw 'Restore or build Seed projects before generating their package inventory.'
}

$seedPackages =
@{}

foreach ($seedAssetsFile in Get-ChildItem -LiteralPath $seedIntermediate -Filter 'project.assets.json' -File -Recurse)
{
    $seedAssets = Get-Content -LiteralPath $seedAssetsFile.FullName -Raw | ConvertFrom-Json -AsHashtable

    foreach ($seedIdentity in $seedAssets.libraries.Keys)
    {
        $seedLibrary = $seedAssets.libraries[$seedIdentity]
        if ($seedLibrary.type -ne 'package' -or $seedPackages.ContainsKey($seedIdentity))
        {
            continue
        }

        $seedParts = $seedIdentity.Split('/')
        $seedLicense = $null
        $seedLicenseType = $null
        $seedLicenseUrl = $null
        $seedRepository = $null

        foreach ($seedFolder in $seedAssets.packageFolders.Keys)
        {
            $seedNuspec = Join-Path (Join-Path $seedFolder $seedLibrary.path) ($seedParts[0].ToLowerInvariant() + '.nuspec')
            if (-not (Test-Path -LiteralPath $seedNuspec))
            {
                continue
            }

            [xml]$seedSpecification = Get-Content -LiteralPath $seedNuspec -Raw
            $seedMetadata = $seedSpecification.package.metadata
            if ($seedMetadata.license)
            {
                $seedLicense = $seedMetadata.license.InnerText
                $seedLicenseType = $seedMetadata.license.type
            }

            $seedLicenseUrl = [string]$seedMetadata.licenseUrl
            $seedRepository = [string]$seedMetadata.repository.url
            break
        }

        $seedPackages[$seedIdentity] = [ordered]@{
            Name = $seedParts[0]
            Version = $seedParts[1]
            NuGetSha512 = $seedLibrary.sha512
            License = $seedLicense
            LicenseType = $seedLicenseType
            LicenseUrl = $seedLicenseUrl
            Repository = $seedRepository
        }
    }
}

$seedInventory =
[ordered]@{
    FormatVersion = 1
    NativeRuntimeModules = @('Seed.Runtime.exe', 'ntdll.dll')
    NativeThirdPartyHttpTlsCryptoLibraries = @()
    ManagedPackages = @($seedPackages.Values | Sort-Object `
        {
            $_['Name']
        }, `
        {
            $_['Version']
        })
    PendingProvenance = @('Imported SDK/compiler support headers', 'Certificate bundle data', 'Retained project contributions and historical revisions')
}

$seedOutputPath = [IO.Path]::GetFullPath($Output)
New-Item -ItemType Directory -Path ([IO.Path]::GetDirectoryName($seedOutputPath)) -Force | Out-Null
$seedInventory | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath $seedOutputPath -Encoding utf8

Write-Output "Managed dependency inventory written: $($seedPackages.Count) package versions; $seedOutputPath"
