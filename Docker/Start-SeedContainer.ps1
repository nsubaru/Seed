$ErrorActionPreference = 'Stop'

$seedRoot    = Split-Path -Parent $PSCommandPath
$seedConsole = Join-Path $seedRoot 'Console/Seed.Console.exe'
$seedRuntime = Join-Path $seedRoot 'Runtime/Seed.Runtime.exe'
$seedProcess = $null

try
{
    # The external console starts the chosen image subsystem via Seed.Starter.
    # Keeping this supervising process alive keeps the container alive.
    $seedStartup = & $seedConsole --start $seedRuntime version

    $seedStartup | Write-Output

    if ($LASTEXITCODE -ne 0)
    {
        throw 'Seed container startup failed.'
    }

    $seedMarker = @($seedStartup | Where-Object `
    {
        $_ -match '^Started Seed runtime process (\d+)\.$'
    })

    if ($seedMarker.Count -ne 1 -or $seedMarker[0] -notmatch '^Started Seed runtime process (\d+)\.$')
    {
        throw 'Missing owned runtime identifier.'
    }

    $seedProcess = [Diagnostics.Process]::GetProcessById([int]$Matches[1])
    
    while (-not $seedProcess.WaitForExit(1000))
    {
    }

    exit $seedProcess.ExitCode
}
finally
{
    if ($seedProcess)
    {
        if (-not $seedProcess.HasExited)
        {
            $seedProcess.Kill()
        }

        $seedProcess.Dispose()
    }
}
