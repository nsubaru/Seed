param()

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedDocumentation = Join-Path $seedRoot 'Documentation'
if (-not (Test-Path -LiteralPath (Join-Path $seedDocumentation 'index.md')))
{
    throw 'Generate Seed documentation first.'
}

$seedErrors = [Collections.Generic.List[string]]::new()
$seedLinks = 0

foreach ($seedPage in Get-ChildItem -LiteralPath $seedDocumentation -Filter '*.md' -File -Recurse)
{
    $seedText = [IO.File]::ReadAllText($seedPage.FullName)

    foreach ($seedMatch in [regex]::Matches($seedText, '\[[^\]\r\n]+\]\(([^)]+)\)'))
    {
        $seedTargetText = $seedMatch.Groups[1].Value.Trim('<', '>')

        if ($seedTargetText -match '^[A-Za-z][A-Za-z0-9+.-]*:|^#')
        {
            continue
        }

        $seedTargetText = [Uri]::UnescapeDataString(($seedTargetText -split '#', 2)[0])
        $seedTarget     = [IO.Path]::GetFullPath((Join-Path $seedPage.DirectoryName $seedTargetText))

        if (-not $seedTarget.StartsWith($seedRoot + [IO.Path]::DirectorySeparatorChar, [StringComparison]::OrdinalIgnoreCase) -or -not (Test-Path -LiteralPath $seedTarget))
        {
            $seedErrors.Add("Broken or external local link in $($seedPage.Name): $seedTargetText")
        }

        ++$seedLinks
    }
}

if ($seedErrors.Count)
{
    throw ($seedErrors -join "`n")
}

$seedApiIndex = [IO.File]::ReadAllText((Join-Path $seedDocumentation 'api/index.md'))

if ($seedApiIndex -notmatch 'documented members')
{
    throw 'API documentation has no compiler XML input; build the shipped projects first.'
}

Write-Output "Seed documentation links passed: $seedLinks local references."
