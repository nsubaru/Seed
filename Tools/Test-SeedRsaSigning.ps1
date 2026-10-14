param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'
$seedRoot = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedOutput = Join-Path $seedRoot ("Artifacts/rsa-signing/$Configuration/" + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null
$seedPrevious = [Environment]::GetEnvironmentVariable('SEED_RSA_VERIFY_DIRECTORY')

try
{
    [Environment]::SetEnvironmentVariable('SEED_RSA_VERIFY_DIRECTORY', $seedOutput)

    & (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Tests -Configuration $Configuration -Wildcard '*RsaSigning*' -Output (Join-Path $seedOutput 'native.xml') -NoBuild
    [xml]$seedReport = Get-Content -LiteralPath (Join-Path $seedOutput 'native.xml') -Raw
    if ($seedReport.'test-run'.passed -ne '3' -or $seedReport.'test-run'.failed -ne '0')
    {
        throw 'The three RSA signing tests did not pass.'
    }

    # These are deliberately public, locally generated test identities. Only
    # their public integers and native signatures are loaded into this verifier.
    $seedVectors = [IO.File]::ReadAllText((Join-Path $seedRoot 'Native/Seed.Core.Testing.Tests.Crypt.RsaSigner.Vectors.h'))
    $seedMessage = [Text.Encoding]::UTF8.GetBytes('Seed RSA signing independent test fixture')
    $seedVerified = 0

    foreach ($seedBits in @(2048, 3072, 4096))
    {
        $seedMatch = [regex]::Match($seedVectors, "\bPublic$seedBits\s*\[\s*\]\s*=\s*L`"([0-9a-f]+)`"\s*;")
        if (-not $seedMatch.Success)
        {
            throw "Missing public $seedBits-bit test key."
        }

        $seedRsa = [Security.Cryptography.RSA]::Create()

        try
        {
            [int]$seedRead = 0
            $seedPublic = [Convert]::FromHexString($seedMatch.Groups[1].Value)
            $seedRsa.ImportRSAPublicKey($seedPublic, [ref]$seedRead)
            if ($seedRead -ne $seedPublic.Length)
            {
                throw 'Public RSA fixture has trailing data.'
            }

            foreach ($seedHash in @('SHA256', 'SHA384', 'SHA512'))
            {
                foreach ($seedPadding in @('pkcs1', 'pss'))
                {
                    $seedSignature = [IO.File]::ReadAllBytes((Join-Path $seedOutput "$seedBits-$seedHash-$seedPadding.sig"))
                    $seedMode = if ($seedPadding -eq 'pss')
                    {
                        [Security.Cryptography.RSASignaturePadding]::Pss
                    }
                    else
                    {
                        [Security.Cryptography.RSASignaturePadding]::Pkcs1
                    }

                    if (-not $seedRsa.VerifyData($seedMessage, $seedSignature, [Security.Cryptography.HashAlgorithmName]::new($seedHash), $seedMode))
                    {
                        throw "Independent .NET RSA verification failed: $seedBits/$seedHash/$seedPadding."
                    }

                    $seedSignature[$seedSignature.Length - 1] = $seedSignature[$seedSignature.Length - 1] -bxor 1
                    if ($seedRsa.VerifyData($seedMessage, $seedSignature, [Security.Cryptography.HashAlgorithmName]::new($seedHash), $seedMode))
                    {
                        throw 'Independent RSA verification accepted a modified signature.'
                    }

                    ++$seedVerified
                }
            }
        }
        finally
        {
            $seedRsa.Dispose()
        }
    }

    Write-Output "Independent .NET RSA signing checks passed: $seedVerified native signatures and tamper rejections. Reports: $seedOutput"
}
finally
{
    [Environment]::SetEnvironmentVariable('SEED_RSA_VERIFY_DIRECTORY', $seedPrevious)
}
