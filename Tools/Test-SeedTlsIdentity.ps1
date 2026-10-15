param([ValidateSet('Debug', 'Release', 'Coverage')][string] $Configuration = 'Debug')

$ErrorActionPreference = 'Stop'

$seedRoot   = (Resolve-Path -LiteralPath (Join-Path $PSScriptRoot '..')).Path
$seedOutput = Join-Path $seedRoot ("Artifacts/tls-identity/$Configuration/" + [Guid]::NewGuid().ToString('N'))

New-Item -ItemType Directory -Path $seedOutput -Force | Out-Null

$seedPrevious = [Environment]::GetEnvironmentVariable('SEED_TLS_IDENTITY_DIRECTORY')

try
{
    [Environment]::SetEnvironmentVariable('SEED_TLS_IDENTITY_DIRECTORY', $seedOutput)

    & (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Tests -Configuration $Configuration -Wildcard '*Tls::IdentityTests::*' -Output (Join-Path $seedOutput 'native.xml') -NoBuild

    [xml]$seedReport = Get-Content -LiteralPath (Join-Path $seedOutput 'native.xml') -Raw

    if ($seedReport.'test-run'.passed -ne '4' -or $seedReport.'test-run'.failed -ne '0')
    {
        throw 'The four TLS identity tests did not pass.'
    }

    # All keys in this test output are deliberately public fixture keys, never runtime identities.
    $seedLeaf  = [Security.Cryptography.X509Certificates.X509CertificateLoader]::LoadCertificate([IO.File]::ReadAllBytes((Join-Path $seedOutput 'leaf.der')))
    $seedChain = [Security.Cryptography.X509Certificates.X509Certificate2Collection]::new()

    $seedChain.ImportFromPem([IO.File]::ReadAllText((Join-Path $seedOutput 'chain.pem')))

    $seedValidator = [Security.Cryptography.X509Certificates.X509Chain]::new()
    $seedKey       = [Security.Cryptography.RSA]::Create()

    try
    {
        $seedValidator.ChainPolicy.TrustMode = [Security.Cryptography.X509Certificates.X509ChainTrustMode]::CustomRootTrust

        [void]$seedValidator.ChainPolicy.CustomTrustStore.Add($seedChain[1])

        $seedValidator.ChainPolicy.RevocationMode = [Security.Cryptography.X509Certificates.X509RevocationMode]::NoCheck
        $seedValidator.ChainPolicy.DisableCertificateDownloads = $true

        [void]$seedValidator.ChainPolicy.ApplicationPolicy.Add([Security.Cryptography.Oid]::new('1.3.6.1.5.5.7.3.1'))

        if (-not $seedValidator.Build($seedLeaf))
        {
            throw 'Independent .NET fixture chain validation failed.'
        }

        [int]$seedRead = 0

        $seedKeyBytes = [IO.File]::ReadAllBytes((Join-Path $seedOutput 'key.der'))

        try
        {
            $seedKey.ImportPkcs8PrivateKey($seedKeyBytes, [ref]$seedRead)
        }
        finally
        {
            [Security.Cryptography.CryptographicOperations]::ZeroMemory($seedKeyBytes)
        }

        $seedCertificateKey = [Security.Cryptography.X509Certificates.RSACertificateExtensions]::GetRSAPublicKey($seedLeaf)

        try
        {
            if ([Convert]::ToHexString($seedCertificateKey.ExportRSAPublicKey()) -cne [Convert]::ToHexString($seedKey.ExportRSAPublicKey()))
            {
                throw 'Independent .NET certificate/private-key matching failed.'
            }
        }
        finally
        {
            $seedCertificateKey.Dispose()
        }
    }
    finally
    {
        $seedKey.Dispose()
        $seedValidator.Dispose()
        $seedLeaf.Dispose()

        foreach ($seedCertificate in $seedChain)
        {
            $seedCertificate.Dispose()
        }
    }

    & (Join-Path $PSScriptRoot 'Invoke-SeedWorkflow.ps1') -Action Tests -Configuration $Configuration -Wildcard '*IO::ByteStream::*' -Output (Join-Path $seedOutput 'transport.xml') -NoBuild

    [xml]$seedTransportReport = Get-Content -LiteralPath (Join-Path $seedOutput 'transport.xml') -Raw
    
    if ($seedTransportReport.'test-run'.passed -ne '3' -or $seedTransportReport.'test-run'.failed -ne '0')
    {
        throw 'The byte transport round-trip, cancellation and timeout tests did not all pass.'
    }

    $global:LASTEXITCODE = 0

    Write-Output "TLS identity and TCP transport tests passed with independent .NET key/chain checks. Reports: $seedOutput"
}
finally
{
    [Environment]::SetEnvironmentVariable('SEED_TLS_IDENTITY_DIRECTORY', $seedPrevious)
}
