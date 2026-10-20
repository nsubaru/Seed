param(
    [Parameter(Mandatory)][ValidateSet('Tests', 'Rtti', 'Documentation', 'Coverage')][string] $Action,
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [string] $Wildcard = '*',
    [string] $Output,
    [switch] $NoBuild
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
Push-Location -LiteralPath $seedRoot

try
{
    if ($Action -eq 'Documentation')
    {
        if ($NoBuild)
        {
            $seedGenerator = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.XMLDocGen/Seed.XMLDocGen.exe"

            if (-not (Test-Path -LiteralPath $seedGenerator))
            {
                throw "Documentation generator is unavailable: $seedGenerator"
            }

            & $seedGenerator $seedRoot (Join-Path $seedRoot 'Documentation')
        }
        else
        {
            dotnet run --project Tools/Seed.XMLDocGen --configuration $Configuration -- $seedRoot (Join-Path $seedRoot 'Documentation')
        }

        if ($LASTEXITCODE -ne 0)
        {
            throw 'Seed documentation generation failed.'
        }

        return
    }

    if ($Action -eq 'Coverage')
    {
        $Configuration = 'Coverage'
    }

    if (-not $NoBuild)
    {
        & (Join-Path $PSScriptRoot 'Update-NtdllImportLibrary.ps1')

        $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
        $msbuild = & $vswhere -latest -products '*' -requires Microsoft.Component.MSBuild -find 'MSBuild/**/Bin/MSBuild.exe' | Select-Object -First 1

        if (-not $msbuild)
        {
            throw 'Visual Studio MSBuild is unavailable.'
        }

        & $msbuild Seed.slnx /m /restore /nologo "/p:Configuration=$Configuration" /p:Platform=x64

        if ($LASTEXITCODE -ne 0)
        {
            throw 'Seed solution build failed.'
        }
    }

    $runtime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
    $runner  = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Managed.TestRunner/Seed.Managed.TestRunner.exe"

    if (-not (Test-Path -LiteralPath $runner))
    {
        throw "Test runner is unavailable: $runner"
    }

    if (-not $Output)
    {
        $Output = Join-Path $seedRoot "Artifacts/tests/$Configuration/native.xml"
    }

    $runnerArguments = @('--runtime', $runtime, '--wildcard', $Wildcard, '--report', $Output)

    if ($Action -eq 'Rtti')
    {
        $runnerArguments += @('--rtti', (Join-Path $seedRoot "Artifacts/tests/$Configuration/seed-rtti.dat"))
    }

    if ($Action -eq 'Coverage')
    {
        $collector = 'C:/Program Files/OpenCppCoverage/OpenCppCoverage.exe'

        if (-not (Test-Path -LiteralPath $collector))
        {
            throw 'OpenCppCoverage is unavailable.'
        }

        $coverageDirectory = Join-Path $seedRoot ('Artifacts/coverage/' + (Get-Date -Format 'yyyyMMdd-HHmmss'))

        New-Item -ItemType Directory -Path $coverageDirectory | Out-Null
        Copy-Item -LiteralPath $runtime, ([IO.Path]::ChangeExtension($runtime, '.pdb')), ([IO.Path]::ChangeExtension($runtime, '.map')) -Destination $coverageDirectory

        & $collector --cover_children --continue_after_cpp_exception --modules '*Seed.Runtime.exe' --sources $seedRoot --excluded_sources '*Lib\NtApi*' --export_type "cobertura:$(Join-Path $coverageDirectory 'coverage.xml')" --export_type "html:$(Join-Path $coverageDirectory 'html')" -- $runner @runnerArguments

        if ($LASTEXITCODE -ne 0)
        {
            throw 'Coverage collection or the covered native tests failed.'
        }

        [xml]$coverage = Get-Content -LiteralPath (Join-Path $coverageDirectory 'coverage.xml') -Raw
        
        if ([int]$coverage.coverage.'lines-valid' -le 0)
        {
            throw 'Collector did not attribute native source lines; the child-process debugger path needs review.'
        }

        Write-Output $coverageDirectory
    }
    else
    {
        & $runner @runnerArguments
        if ($LASTEXITCODE -ne 0)
        {
            throw 'Seed native verification failed; inspect the NUnit report and runtime log.'
        }
    }
}
finally
{
    Pop-Location
}
