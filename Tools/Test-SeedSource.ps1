param()

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedFiles = & git -c "safe.directory=$seedRoot" -C $seedRoot ls-files --cached --others --exclude-standard
if ($LASTEXITCODE -ne 0)
{
    throw 'Cannot enumerate Seed source files.'
}

$seedExcluded = '(?<![A-Za-z0-9_])(?:Rem|Seed)(?:::|\.)(?:AccessControl|SecurityAuthority|Storage|Registry|ObjectNamespace|ObjNamespace|Cardinal|HostOC|EventFS|Executable|TaskScheduler|UIIO|Languages|Llm|Speech|UpdateManager)(?:::|\.|\b)'
$seedOldBranding = '\bRem(?=[.:])|\bNatsuki Rem\b|\b[A-Za-z_][A-Za-z0-9_]*(?:Rem(?=[A-Z_]|\b)|REM(?=_|\b))[A-Za-z0-9_]*\b'
$seedCodeExtensions = @('.h', '.cpp', '.cs', '.asm', '.natvis', '.csproj', '.vcxproj', '.vcxitems', '.filters')
$seedErrors = [Collections.Generic.List[string]]::new()
$seedChecked = 0

function Get-SeedNativeItemKey
{
    param([string] $ItemType, [string] $Include, [string] $ProjectDirectory)

    $seedInput = $Include.Replace('$(MSBuildThisFileDirectory)', $ProjectDirectory + [IO.Path]::DirectorySeparatorChar)
    if (-not [IO.Path]::IsPathRooted($seedInput))
    {
        $seedInput = Join-Path $ProjectDirectory $seedInput
    }

    return "${ItemType}:$([IO.Path]::GetFullPath($seedInput))"
}

foreach ($seedRelative in $seedFiles | Sort-Object -Unique)
{
    $seedPath = Join-Path $seedRoot $seedRelative
    if (-not (Test-Path -LiteralPath $seedPath -PathType Leaf))
    {
        continue
    }

    if ($seedRelative -match '^(?:Storage|ObjectNamespace|HostOC|EventFS|UI|IoT|Deployment|ThirdParty)/')
    {
        $seedErrors.Add("Excluded directory: $seedRelative")
    }

    $seedExtension = [IO.Path]::GetExtension($seedPath)
    if ($seedExtension -notin $seedCodeExtensions)
    {
        continue
    }

    $seedText = [IO.File]::ReadAllText($seedPath)

    # External support headers are checked by the separate provenance audit.
    if ($seedRelative -notmatch '^Lib/(?:NtApi|Intrinsics|NtIntSafe|WinApiHeaders)/')
    {
        if ($seedText -match $seedExcluded)
        {
            $seedErrors.Add("Excluded code dependency: $seedRelative")
        }

        $seedNonAttribution = ($seedText -split '\r?\n' | Where-Object `
            {
                $_ -notmatch '(?i)copyright|attribution'
            }) -join "`n"
        if ($seedNonAttribution -cmatch $seedOldBranding)
        {
            $seedErrors.Add("Old product identifier: $seedRelative")
        }
    }

    if ($seedExtension -in @('.csproj', '.vcxproj', '.vcxitems', '.filters'))
    {
        [xml]$seedProject = $seedText

        foreach ($seedNode in $seedProject.SelectNodes('//*[@Include] | //*[local-name()="Import"][@Project]'))
        {
            if ($seedExtension -eq '.filters' -and $seedNode.LocalName -ne 'Import')
            {
                continue
            }

            if ($seedNode.LocalName -notin @('ProjectReference', 'ClCompile', 'ClInclude', 'MASM', 'EmbeddedResource', 'Import'))
            {
                continue
            }

            $seedReference = if ($seedNode.LocalName -eq 'Import')
            {
                if ($seedNode.Project -match '\.vcxitems$' -and $seedNode.Label -ne 'Shared')
                {
                    $seedErrors.Add("Missing shared import label: $seedRelative -> $($seedNode.Project)")
                }

                $seedNode.Project
            }
            else
            {
                $seedNode.Include
            }

            if ($seedReference -match '\*|\?')
            {
                continue
            }

            $seedProjectDirectory = [IO.Path]::GetDirectoryName($seedPath) + [IO.Path]::DirectorySeparatorChar
            $seedReference = $seedReference.Replace('$(MSBuildThisFileDirectory)', $seedProjectDirectory)
            if ($seedReference.Contains('$('))
            {
                continue
            }

            $seedTarget = [IO.Path]::GetFullPath($(if ([IO.Path]::IsPathRooted($seedReference))
                    {
                        $seedReference
                    }
                    else
                    {
                        Join-Path $seedProjectDirectory $seedReference
                    }))
            if (-not $seedTarget.StartsWith($seedRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase))
            {
                $seedErrors.Add("Project reference escapes the checkout: $seedRelative -> $seedReference")
            }

            elseif (-not (Test-Path -LiteralPath $seedTarget))
            {
                $seedErrors.Add("Missing project input: $seedRelative -> $seedReference")
            }
        }
    }

    if ($seedExtension -in @('.vcxproj', '.vcxitems'))
    {
        $seedFiltersPath = "$seedPath.filters"
        if (-not (Test-Path -LiteralPath $seedFiltersPath -PathType Leaf))
        {
            $seedErrors.Add("Missing native project filters: $seedRelative")
        }
        else
        {
            [xml]$seedFilters = [IO.File]::ReadAllText($seedFiltersPath)
            $seedFilterNames = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
            $seedFilterItems = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
            $seedProjectItems = [Collections.Generic.HashSet[string]]::new([StringComparer]::OrdinalIgnoreCase)
            $seedNativeAliases = [Collections.Generic.Dictionary[string, string]]::new([StringComparer]::OrdinalIgnoreCase)
            $seedNativeDeclarations = [Collections.Generic.Dictionary[string, Xml.XmlElement]]::new([StringComparer]::OrdinalIgnoreCase)
            $seedItemTypes = @('ClCompile', 'ClInclude', 'Natvis', 'MASM', 'None', 'Text', 'ResourceCompile', 'Image')

            foreach ($seedFilter in $seedFilters.SelectNodes('//*[local-name()="Filter"][@Include]'))
            {
                if (-not $seedFilterNames.Add($seedFilter.Include))
                {
                    $seedErrors.Add("Duplicate native filter: $seedRelative -> $($seedFilter.Include)")
                }
            }

            foreach ($seedFilterName in $seedFilterNames)
            {
                $seedParentFilter = [IO.Path]::GetDirectoryName($seedFilterName)
                if ($seedParentFilter -and -not $seedFilterNames.Contains($seedParentFilter))
                {
                    $seedErrors.Add("Missing native filter parent: $seedRelative -> $seedParentFilter")
                }
            }

            $seedOwningDirectory = [IO.Path]::GetDirectoryName($seedPath)
            $seedNativeInputs = @([pscustomobject]@{ Document = $seedProject; Directory = $seedOwningDirectory })
            foreach ($seedImport in $seedProject.SelectNodes('//*[local-name()="Import"][@Project]'))
            {
                if ($seedExtension -eq '.vcxproj' -or $seedImport.Project -notmatch '\.vcxitems$')
                {
                    continue
                }

                $seedSharedPath = [IO.Path]::GetFullPath((Join-Path $seedOwningDirectory $seedImport.Project))
                [xml]$seedSharedProject = [IO.File]::ReadAllText($seedSharedPath)
                $seedNativeInputs += [pscustomobject]@{ Document = $seedSharedProject; Directory = [IO.Path]::GetDirectoryName($seedSharedPath) }
            }

            foreach ($seedNativeInput in $seedNativeInputs)
            {
                foreach ($seedItem in $seedNativeInput.Document.SelectNodes('//*[@Include]'))
                {
                    if ($seedItem.LocalName -in $seedItemTypes)
                    {
                        $seedInputKey = Get-SeedNativeItemKey $seedItem.LocalName $seedItem.Include $seedNativeInput.Directory
                        [void]$seedProjectItems.Add($seedInputKey)
                        $seedNativeDeclarations[$seedInputKey] = $seedItem
                        $seedAlias = "$($seedItem.LocalName):$($seedItem.Include)"
                        if ($seedNativeAliases.ContainsKey($seedAlias) -and $seedNativeAliases[$seedAlias] -ne $seedInputKey)
                        {
                            $seedErrors.Add("Ambiguous shared native item: $seedRelative -> $seedAlias")
                        }

                        $seedNativeAliases[$seedAlias] = $seedInputKey
                    }
                }
            }

            foreach ($seedComponent in $seedProject.SelectNodes('//*[local-name()="ItemGroup"][@Label="SeedSourceComponents"]/*[@Include]'))
            {
                $seedComponentPath = [IO.Path]::GetFullPath((Join-Path $seedOwningDirectory $seedComponent.Include))
                $seedComponentDirectory = [IO.Path]::GetDirectoryName($seedComponentPath)
                [xml]$seedComponentProject = [IO.File]::ReadAllText($seedComponentPath)
                foreach ($seedItem in $seedComponentProject.SelectNodes('//*[@Include]'))
                {
                    if ($seedItem.LocalName -notin $seedItemTypes)
                    {
                        continue
                    }

                    $seedInputKey = Get-SeedNativeItemKey $seedItem.LocalName $seedItem.Include $seedComponentDirectory
                    if (-not $seedNativeDeclarations.ContainsKey($seedInputKey))
                    {
                        $seedErrors.Add("Missing SDK component input: $seedRelative -> $seedInputKey")
                        continue
                    }

                    $seedDeclaration = $seedNativeDeclarations[$seedInputKey]
                    foreach ($seedMetadata in $seedItem.SelectNodes('*'))
                    {
                        $seedIndexedMetadata = $seedDeclaration.SelectSingleNode("*[local-name()='$($seedMetadata.LocalName)']")
                        if (-not $seedIndexedMetadata -or $seedIndexedMetadata.InnerText -ne $seedMetadata.InnerText -or $seedIndexedMetadata.GetAttribute('Condition') -ne $seedMetadata.GetAttribute('Condition'))
                        {
                            $seedErrors.Add("SDK compiler metadata differs: $seedRelative -> $seedInputKey / $($seedMetadata.LocalName)")
                        }
                    }
                }
            }

            foreach ($seedItem in $seedFilters.SelectNodes('//*[@Include]'))
            {
                if ($seedItem.LocalName -notin $seedItemTypes)
                {
                    continue
                }

                $seedAlias = "$($seedItem.LocalName):$($seedItem.Include)"
                $seedItemKey = if ($seedNativeAliases.ContainsKey($seedAlias))
                {
                    $seedNativeAliases[$seedAlias]
                }
                else
                {
                    Get-SeedNativeItemKey $seedItem.LocalName $seedItem.Include $seedOwningDirectory
                }
                if (-not $seedFilterItems.Add($seedItemKey))
                {
                    $seedErrors.Add("Duplicate native filter item: $seedRelative -> $seedItemKey")
                }

                if (-not $seedProjectItems.Contains($seedItemKey))
                {
                    $seedErrors.Add("Unknown native filter item: $seedRelative -> $seedItemKey")
                }

                if (-not $seedFilterNames.Contains([string]$seedItem.Filter))
                {
                    $seedErrors.Add("Unassigned native filter item: $seedRelative -> $seedItemKey")
                }
            }

            foreach ($seedItemKey in $seedProjectItems)
            {
                if (-not $seedFilterItems.Contains($seedItemKey))
                {
                    $seedErrors.Add("Missing native filter item: $seedRelative -> $seedItemKey")
                }
            }
        }
    }

    ++$seedChecked
}

$seedAuditDirectory = Join-Path $seedRoot 'Artifacts/audit'
New-Item -ItemType Directory -Path $seedAuditDirectory -Force | Out-Null
@{ CheckedFiles = $seedChecked
    Errors = @($seedErrors)
} | ConvertTo-Json -Depth 4 | Set-Content -LiteralPath (Join-Path $seedAuditDirectory 'source.json') -Encoding utf8
if ($seedErrors.Count)
{
    $seedErrors | Select-Object -First 5 | Write-Output
    throw "$($seedErrors.Count) source audit findings; inspect Artifacts/audit/source.json."
}

Write-Output "Seed source dependency and identifier checks passed: $seedChecked code/project files. Rights and historical content need separate audits."
