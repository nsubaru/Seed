param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'

$seedRoot    = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedFixture = Join-Path $seedRoot 'Artifacts/integration/configuration/core.v1.bin'
$seedRuntime = Join-Path $seedRoot "Artifacts/x64/$Configuration/Runtime/Seed.Runtime.exe"
$seedRunner  = Join-Path $seedRoot "Artifacts/x64/$Configuration/Seed.Managed.TestRunner/Seed.Managed.TestRunner.exe"
$seedOutput  = Join-Path $seedRoot "Artifacts/configuration-validation/$Configuration"

if (-not (Test-Path -LiteralPath $seedFixture))
{
    throw 'Run Seed integration first to create a saved configuration fixture.'
}

$seedBytes = [IO.File]::ReadAllBytes($seedFixture)

if ($seedBytes.Length -le 16)
{
    throw 'The saved configuration fixture has no payload.'
}

foreach ($seedCase in @('valid', 'invalid-values', 'bad-magic', 'bad-version', 'truncated', 'extended'))
{
    $seedDirectory = Join-Path $seedOutput $seedCase

    New-Item -ItemType Directory -Path $seedDirectory -Force | Out-Null

    [byte[]]$seedCaseBytes = $seedBytes.Clone()

    switch ($seedCase)
    {
        'invalid-values'
        {
            for ($seedIndex = 16; $seedIndex -lt $seedCaseBytes.Length; ++$seedIndex)
            {
                $seedCaseBytes[$seedIndex] = 255
            }
        }

        'bad-magic'
        {
            $seedCaseBytes[0] = $seedCaseBytes[0] -bxor 255
        }

        'bad-version'
        {
            [BitConverter]::GetBytes([uint32]2).CopyTo($seedCaseBytes, 8)
        }

        'truncated'
        {
            $seedCaseBytes = $seedCaseBytes[0..($seedCaseBytes.Length - 2)]
        }

        'extended'
        {
            $seedCaseBytes = $seedCaseBytes + [byte]0
        }
    }

    $seedConfigurationFile = Join-Path $seedDirectory 'core.v1.bin'

    [IO.File]::WriteAllBytes($seedConfigurationFile, $seedCaseBytes)

    $seedBefore = (Get-FileHash -LiteralPath $seedConfigurationFile -Algorithm SHA256).Hash
    $seedReport = Join-Path $seedDirectory 'native.xml'

    & $seedRunner --runtime $seedRuntime --wildcard '*SnapshotValidationTest' --report $seedReport --configuration-directory $seedDirectory --timeout-seconds 30 *> (Join-Path $seedDirectory 'console.log')

    $seedExit = $LASTEXITCODE

    if ($seedCase -eq 'valid')
    {
        if ($seedExit -ne 0)
        {
            throw 'Startup rejected the valid saved Seed configuration.'
        }

        [xml]$seedResult = Get-Content -LiteralPath $seedReport -Raw
        
        if ($seedResult.'test-run'.passed -ne '1' -or $seedResult.'test-run'.failed -ne '0')
        {
            throw 'The saved configuration validation test did not pass.'
        }
    }

    elseif ($seedExit -eq 0 -or (Test-Path -LiteralPath $seedReport))
    {
        throw "Startup accepted a malformed configuration: $seedCase"
    }

    if ((Get-FileHash -LiteralPath $seedConfigurationFile -Algorithm SHA256).Hash -cne $seedBefore)
    {
        throw "Startup modified its input configuration: $seedCase"
    }

    Write-Host "Configuration startup scenario passed: $seedCase"
}
