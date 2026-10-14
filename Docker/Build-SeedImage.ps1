param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Release',
    [ValidateSet('Native', 'Console', 'Windows')][string] $Subsystem = 'Native'
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$seedVsRoot = & $seedVswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if ($LASTEXITCODE -ne 0 -or -not $seedVsRoot)
{
    throw 'MSVC installation is unavailable in the build image.'
}

$seedToolDirectory = Get-ChildItem -LiteralPath (Join-Path $seedVsRoot 'VC/Tools/MSVC') -Directory |
    Sort-Object Name -Descending | Select-Object -First 1 -ExpandProperty FullName
$seedLib = Join-Path $seedToolDirectory 'bin/Hostx64/x64/lib.exe'
$seedDumpbin = Join-Path $seedToolDirectory 'bin/Hostx64/x64/dumpbin.exe'
$seedMsbuild = $env:MSBUILD_EXE_PATH
if (-not $seedMsbuild -or -not (Test-Path -LiteralPath $seedMsbuild))
{
    $seedMsbuild = & $seedVswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/**/Bin/MSBuild.exe' | Select-Object -First 1
}

if (-not $seedMsbuild)
{
    throw 'Visual Studio MSBuild is unavailable in the build image.'
}

& (Join-Path $seedRoot 'Tools/Update-NtdllImportLibrary.ps1') -LibExe $seedLib -DumpbinExe $seedDumpbin

& $seedMsbuild (Join-Path $seedRoot 'Seed.slnx') /m /restore /nologo /p:Platform=x64 "/p:Configuration=$Configuration" "/p:SeedSubsystem=$Subsystem"
if ($LASTEXITCODE -ne 0)
{
    throw 'Seed container source build failed.'
}

$seedArtifacts = Join-Path $seedRoot "Artifacts/x64/$Configuration"
$seedPayload = 'C:/payload'

# The external starter requires the Microsoft C++ runtime. The native Seed image does not.
$seedRedistDirectory = Get-ChildItem -LiteralPath (Join-Path $seedVsRoot 'VC/Redist/MSVC') -Directory |
    Where-Object `
    {
        Test-Path -LiteralPath (Join-Path $_.FullName 'x64/Microsoft.VC145.CRT/vcruntime140.dll')
    } |
    Sort-Object Name -Descending | Select-Object -First 1 -ExpandProperty FullName
$seedCrtDirectory = Join-Path $seedRedistDirectory 'x64/Microsoft.VC145.CRT'
if (-not (Test-Path -LiteralPath (Join-Path $seedCrtDirectory 'vcruntime140.dll')))
{
    throw 'MSVC x64 runtime DLLs are unavailable.'
}

Get-ChildItem -LiteralPath $seedArtifacts -Filter Seed.Starter.dll -Recurse -File | ForEach-Object `
{
    Copy-Item -Path (Join-Path $seedCrtDirectory '*.dll') -Destination $_.DirectoryName
}

New-Item -ItemType Directory -Path "$seedPayload/Runtime", "$seedPayload/Console" -Force | Out-Null
Copy-Item -LiteralPath (Join-Path $seedArtifacts 'Runtime/Seed.Runtime.exe') -Destination "$seedPayload/Runtime/Seed.Runtime.exe"
Copy-Item -Path (Join-Path $seedArtifacts 'Seed.Console/*') -Destination "$seedPayload/Console"

& (Join-Path $seedArtifacts 'Seed.XMLDocGen/Seed.XMLDocGen.exe') $seedRoot (Join-Path $seedRoot 'Documentation')
if ($LASTEXITCODE -ne 0)
{
    throw 'Container documentation generation failed.'
}

# Record import evidence without loading Win32 dependencies into the native image.
$seedImports = & $seedDumpbin /imports (Join-Path $seedArtifacts 'Runtime/Seed.Runtime.exe')
if ($LASTEXITCODE -ne 0)
{
    throw 'Cannot inspect native image imports.'
}

$seedImports | Set-Content -LiteralPath (Join-Path $seedRoot 'Artifacts/native-imports.txt') -Encoding ascii
$seedLibraries = @($seedImports | ForEach-Object `
    {
        if ($_ -match '^\s+([A-Za-z0-9_.-]+\.dll)\s*$')
        {
            $Matches[1].ToLowerInvariant()
        }
    })
if ($seedLibraries.Count -ne 1 -or $seedLibraries[0] -cne 'ntdll.dll')
{
    throw 'Unexpected native runtime DLL imports.'
}

@{
    Configuration = $Configuration
    Subsystem = $Subsystem
    WindowsVersion = [Environment]::OSVersion.Version.ToString()
    DotnetSdk = (& dotnet --version)
    CompilerVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo((Join-Path $seedToolDirectory 'bin/Hostx64/x64/cl.exe')).ProductVersion
    MsbuildVersion = [Diagnostics.FileVersionInfo]::GetVersionInfo($seedMsbuild).ProductVersion
} | ConvertTo-Json | Set-Content -LiteralPath (Join-Path $seedRoot 'Artifacts/container-build.json') -Encoding utf8
