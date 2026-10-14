param(
    [ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug',
    [string] $AdapterDirectory
)

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
if (-not $AdapterDirectory)
{
    $AdapterDirectory = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Testing.TestAdapter"
}

$seedVswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$seedVstest = & $seedVswhere -latest -products '*' -find 'Common7/IDE/Extensions/TestPlatform/vstest.console.exe' | Select-Object -First 1
if (-not $seedVstest)
{
    throw 'Visual Studio VSTest is unavailable.'
}

$seedRuntime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedOutput = Join-Path $seedRoot "Artifacts/test-adapter/$Configuration/$([Guid]::NewGuid().ToString('N'))"
New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null
$seedDiscovery = & $seedVstest $seedRuntime "/TestAdapterPath:$AdapterDirectory" /ListTests /Platform:x64 2>&1
$seedDiscovery | Set-Content -LiteralPath (Join-Path $seedOutput 'discovery.log') -Encoding utf8
if ($LASTEXITCODE -ne 0 -or ($seedDiscovery -join "`n") -notmatch 'Seed::Core::Testing::Test::Configurations::SnapshotValidationTest')
{
    throw "Native adapter discovery failed: $seedOutput"
}

& $seedVstest $seedRuntime "/TestAdapterPath:$AdapterDirectory" /Platform:x64 '/TestCaseFilter:FullyQualifiedName~SnapshotValidationTest' "/ResultsDirectory:$seedOutput" '/Logger:trx;LogFileName=native.trx'
if ($LASTEXITCODE -ne 0)
{
    throw "Native adapter execution failed: $seedOutput"
}

[xml]$seedReport = Get-Content -LiteralPath (Join-Path $seedOutput 'native.trx') -Raw
if ($seedReport.TestRun.ResultSummary.Counters.total -ne '1' -or $seedReport.TestRun.ResultSummary.Counters.passed -ne '1')
{
    throw 'VSTest must report exactly one selected native test passing.'
}

$seedHost = Join-Path $AdapterDirectory 'host/Seed.Testing.Host.exe'
$seedSelection = Join-Path $seedOutput 'cancellation-selection.xml'
'<tests><test name="Seed::Core::Testing::Test::Configurations::SnapshotValidationTest" /></tests>' | Set-Content -LiteralPath $seedSelection -Encoding utf8
$seedCanceledReport = Join-Path $seedOutput 'cancellation-result.xml'
$seedCancelProcess = [Diagnostics.Process]::new()
$seedCancelNative = $null
$seedCancelStarted = $false

try
{
    $seedCancelProcess.StartInfo = [Diagnostics.ProcessStartInfo]::new($seedHost)
    $seedCancelProcess.StartInfo.UseShellExecute = $false
    $seedCancelProcess.StartInfo.CreateNoWindow = $true
    $seedCancelProcess.StartInfo.RedirectStandardInput = $true
    $seedCancelProcess.StartInfo.RedirectStandardOutput = $true
    $seedCancelProcess.StartInfo.RedirectStandardError = $true

    foreach ($seedArgument in @('run', $seedRuntime, $seedSelection, $seedCanceledReport))
    {
        $seedCancelProcess.StartInfo.ArgumentList.Add($seedArgument)
    }

    [void]$seedCancelProcess.Start()
    $seedCancelStarted = $true
    $seedCancelErrors = $seedCancelProcess.StandardError.ReadToEndAsync()
    $seedMarker = $seedCancelProcess.StandardOutput.ReadLineAsync().WaitAsync([TimeSpan]::FromSeconds(30)).GetAwaiter().GetResult()
    if ($seedMarker -notmatch '^SEED_PROCESS (\d+)$')
    {
        throw 'Cancellation host did not report its owned native process.'
    }

    $seedCancelNative = [Diagnostics.Process]::GetProcessById([int]$Matches[1])

    # Cancel before acknowledging execution: no selected native case may run.
    $seedCancelProcess.StandardInput.WriteLine('cancel')
    if (-not $seedCancelProcess.WaitForExit(30000))
    {
        throw 'Cancellation host exceeded its cleanup deadline.'
    }

    if ($seedCancelProcess.ExitCode -eq 0 -or (Test-Path -LiteralPath $seedCanceledReport))
    {
        throw 'Canceled execution unexpectedly produced a completed result.'
    }

    if (-not $seedCancelNative.WaitForExit(5000))
    {
        throw 'Cancellation left its native runtime running.'
    }

    $seedCancelErrors.GetAwaiter().GetResult() | Set-Content -LiteralPath (Join-Path $seedOutput 'cancellation.log') -Encoding utf8
}
finally
{
    if ($seedCancelNative)
    {
        if (-not $seedCancelNative.HasExited)
        {
            $seedCancelNative.Kill()
            $seedCancelNative.WaitForExit()
        }

        $seedCancelNative.Dispose()
    }

    if ($seedCancelStarted -and -not $seedCancelProcess.HasExited)
    {
        $seedCancelProcess.Kill()
    }

    $seedCancelProcess.Dispose()
}

Write-Output "Seed test adapter verification passed: $seedOutput"
