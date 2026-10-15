param([Parameter(Mandatory)][string] $Directory)

$ErrorActionPreference = 'Stop'

$seedDirectory = [IO.Path]::GetFullPath($Directory)

New-Item -ItemType Directory -Path $seedDirectory -Force | Out-Null

$seedFiles = @('chain.pem', 'key.pem', 'root.der', 'root.pem') | ForEach-Object `
{
    Join-Path $seedDirectory $_
}

foreach ($seedFile in $seedFiles)
{
    if (Test-Path -LiteralPath $seedFile)
    {
        throw "Identity output already exists: $seedFile"
    }
}

$seedRootKey = [Security.Cryptography.RSA]::Create(2048)
$seedKey     = [Security.Cryptography.RSA]::Create(2048)
$seedRoot    = $null
$seedLeaf    = $null
$seedPrivate = $null
$seedPem     = $null

try
{
    $seedNow         = [DateTimeOffset]::UtcNow
    $seedRootRequest = [Security.Cryptography.X509Certificates.CertificateRequest]::new("CN=Seed local development CA $([Guid]::NewGuid().ToString('N'))", $seedRootKey, [Security.Cryptography.HashAlgorithmName]::SHA256, [Security.Cryptography.RSASignaturePadding]::Pkcs1)

    $seedRootRequest.CertificateExtensions.Add([Security.Cryptography.X509Certificates.X509BasicConstraintsExtension]::new($true, $false, 0, $true))
    $seedRootRequest.CertificateExtensions.Add([Security.Cryptography.X509Certificates.X509KeyUsageExtension]::new([Security.Cryptography.X509Certificates.X509KeyUsageFlags]::KeyCertSign, $true))

    $seedRoot    = $seedRootRequest.CreateSelfSigned($seedNow.AddMinutes(-5), $seedNow.AddDays(7))
    $seedRequest = [Security.Cryptography.X509Certificates.CertificateRequest]::new('CN=localhost', $seedKey, [Security.Cryptography.HashAlgorithmName]::SHA256, [Security.Cryptography.RSASignaturePadding]::Pkcs1)

    $seedRequest.CertificateExtensions.Add([Security.Cryptography.X509Certificates.X509BasicConstraintsExtension]::new($false, $false, 0, $true))
    $seedRequest.CertificateExtensions.Add([Security.Cryptography.X509Certificates.X509KeyUsageExtension]::new([Security.Cryptography.X509Certificates.X509KeyUsageFlags]::DigitalSignature, $true))

    $seedPurpose = [Security.Cryptography.OidCollection]::new()

    [void]$seedPurpose.Add([Security.Cryptography.Oid]::new('1.3.6.1.5.5.7.3.1'))

    $seedRequest.CertificateExtensions.Add([Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension]::new($seedPurpose, $true))

    $seedNames = [Security.Cryptography.X509Certificates.SubjectAlternativeNameBuilder]::new()

    $seedNames.AddDnsName('localhost')
    $seedNames.AddIpAddress([Net.IPAddress]::Loopback)
    $seedRequest.CertificateExtensions.Add($seedNames.Build())
    $seedLeaf = $seedRequest.Create($seedRoot, $seedNow.AddMinutes(-5), $seedNow.AddDays(1), [Security.Cryptography.RandomNumberGenerator]::GetBytes(16))

    [IO.File]::WriteAllText($seedFiles[0], $seedLeaf.ExportCertificatePem() + "`n" + $seedRoot.ExportCertificatePem() + "`n", [Text.Encoding]::ASCII)

    $seedPrivate = $seedKey.ExportPkcs8PrivateKey()
    $seedPem     = [Security.Cryptography.PemEncoding]::WriteUtf8([Text.Encoding]::ASCII.GetBytes('PRIVATE KEY'), $seedPrivate)

    [IO.File]::WriteAllBytes($seedFiles[1], $seedPem)
    [IO.File]::WriteAllBytes($seedFiles[2], $seedRoot.RawData)
    [IO.File]::WriteAllText($seedFiles[3], $seedRoot.ExportCertificatePem(), [Text.Encoding]::ASCII)
}
finally
{
    if ($seedPrivate)
    {
        [Security.Cryptography.CryptographicOperations]::ZeroMemory($seedPrivate)
    }

    if ($seedPem)
    {
        [Security.Cryptography.CryptographicOperations]::ZeroMemory($seedPem)
    }

    if ($seedLeaf)
    {
        $seedLeaf.Dispose()
    }

    if ($seedRoot)
    {
        $seedRoot.Dispose()
    }

    $seedKey.Dispose()
    $seedRootKey.Dispose()
}

Write-Host "Created fresh localhost development identity in $seedDirectory; leaf expires in one day. Trust root was not installed."
