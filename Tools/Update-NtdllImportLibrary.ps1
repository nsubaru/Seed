param(
    [string[]] $AddExport = @(),
    [switch] $RefreshDefFromSystem,
    [switch] $SkipLib,
    [string] $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path,
    [string] $LibExe,
    [string] $DumpbinExe
)

$ErrorActionPreference = "Stop"

function Find-VsTool
{
    param([string] $ToolName)

    $vsRoot = Join-Path ${env:ProgramFiles} "Microsoft Visual Studio"
    if (-not (Test-Path $vsRoot))
    {
        throw "Visual Studio root not found: $vsRoot"
    }

    $tool = Get-ChildItem -Path $vsRoot -Recurse -Filter $ToolName |
        Where-Object `
        {
            $_.FullName -like "*\bin\Hostx64\x64\*"
        } |
        Sort-Object FullName -Descending |
        Select-Object -First 1

    if ($null -eq $tool)
    {
        throw "Unable to locate $ToolName under $vsRoot"
    }

    return $tool.FullName
}

function Read-DefExports
{
    param([string] $DefPath)

    $lines = Get-Content -Path $DefPath
    $exports = @()

    foreach ($line in $lines)
    {
        $trimmed = $line.Trim()
        if ($trimmed.Length -eq 0 -or $trimmed -eq "EXPORTS")
        {
            continue
        }

        $exports += $trimmed
    }

    return $exports
}

function Write-DefExports
{
    param(
        [string] $DefPath,
        [string[]] $Exports
    )

    $seen =
    @{}
    $orderedExports = @()

    foreach ($export in $Exports)
    {
        if (-not $seen.ContainsKey($export))
        {
            $seen[$export] = $true
            $orderedExports += $export
        }
    }

    $content = @("EXPORTS") + $orderedExports
    Set-Content -Path $DefPath -Value $content -Encoding ASCII
}

function Get-SystemNtdllExports
{
    param([string] $DumpbinPath)

    $ntdllPath = Join-Path $env:SystemRoot "System32\ntdll.dll"
    $output = & $DumpbinPath /exports $ntdllPath
    if ($LASTEXITCODE -ne 0)
    {
        throw "dumpbin failed with exit code $LASTEXITCODE"
    }

    $exports = @()

    foreach ($line in $output)
    {
        if ($line -match "^\s*\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+([^\s=]+)")
        {
            $name = $Matches[1]
            if ($name -ne "[NONAME]")
            {
                $exports += $name
            }
        }
    }

    return $exports
}

$defPath = Join-Path $RepoRoot "Lib\NtApi\ntdll.def"
$libPath = Join-Path $RepoRoot "Lib\NtApi\ntdll.lib"
$expPath = [IO.Path]::ChangeExtension($libPath, ".exp")

if (-not (Test-Path $defPath))
{
    throw "ntdll.def not found: $defPath"
}

if ($RefreshDefFromSystem)
{
    if ([string]::IsNullOrWhiteSpace($DumpbinExe))
    {
        $DumpbinExe = Find-VsTool "dumpbin.exe"
    }

    Write-DefExports -DefPath $defPath -Exports (Get-SystemNtdllExports -DumpbinPath $DumpbinExe)
}

if ($AddExport.Count -gt 0)
{
    $exports = @(Read-DefExports -DefPath $defPath)

    foreach ($export in $AddExport)
    {
        if (-not ($exports -contains $export))
        {
            $exports += $export
        }
    }

    Write-DefExports -DefPath $defPath -Exports $exports
}

if (-not $SkipLib)
{
    if ([string]::IsNullOrWhiteSpace($LibExe))
    {
        $LibExe = Find-VsTool "lib.exe"
    }

    & $LibExe /def:$defPath /machine:amd64 /out:$libPath
    if ($LASTEXITCODE -ne 0)
    {
        throw "lib.exe failed with exit code $LASTEXITCODE"
    }
}

Write-Host "Updated $defPath"
if (-not $SkipLib)
{
    Write-Host "Updated $libPath"
    Write-Host "Updated $expPath"
}
