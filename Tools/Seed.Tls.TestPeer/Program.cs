using System.Net;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Text;

if (args is ["probe", string output])
{
    await ClientHelloProbe.Run(Path.Combine(AppContext.BaseDirectory, "ClientHello.tls12.hex"), output);

    return;
}

if (args.Length is < 2 or > 3 || args[1] is not ("1.2" or "1.3") || (args.Length == 3 && args[2] is not ("expired" or "ipv6")))
{
    throw new ArgumentException("Use <output-directory> <1.2|1.3> [expired|ipv6].");
}

bool expired = args.Length == 3 && args[2] == "expired";
bool ipv6 = args.Length == 3 && args[2] == "ipv6";

string directory = Path.GetFullPath(args[0]);

Directory.CreateDirectory(directory);

using var rootKey = RSA.Create(2048);
var rootRequest = new CertificateRequest($"CN=Seed development test CA {Guid.NewGuid():N}", rootKey, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);

rootRequest.CertificateExtensions.Add(new X509BasicConstraintsExtension(true, false, 0, true));
rootRequest.CertificateExtensions.Add(new X509KeyUsageExtension(X509KeyUsageFlags.KeyCertSign, true));
rootRequest.CertificateExtensions.Add(new X509SubjectKeyIdentifierExtension(rootRequest.PublicKey, false));

var now = DateTimeOffset.UtcNow;
using var root = rootRequest.CreateSelfSigned(now.AddDays(-2), now.AddHours(2));
using var serverKey = RSA.Create(2048);
var serverRequest = new CertificateRequest("CN=localhost", serverKey, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);

serverRequest.CertificateExtensions.Add(new X509BasicConstraintsExtension(false, false, 0, true));
serverRequest.CertificateExtensions.Add(new X509KeyUsageExtension(X509KeyUsageFlags.DigitalSignature | X509KeyUsageFlags.KeyEncipherment, true));
serverRequest.CertificateExtensions.Add(new X509EnhancedKeyUsageExtension(new OidCollection{ new Oid("1.3.6.1.5.5.7.3.1")}, true));

var names = new SubjectAlternativeNameBuilder();
names.AddDnsName("localhost");
names.AddIpAddress(IPAddress.Loopback);
names.AddIpAddress(IPAddress.IPv6Loopback);

serverRequest.CertificateExtensions.Add(names.Build());

using var publicCertificate = serverRequest.Create(root, now.AddHours(-1), expired ? now.AddMinutes(-30) : now.AddHours(1), RandomNumberGenerator.GetBytes(16));
using var ephemeralCertificate = publicCertificate.CopyWithPrivateKey(serverKey);

// Schannel requires a provider-backed server key; the import owns a temporary user key container.
byte[] pfx = ephemeralCertificate.Export(X509ContentType.Pfx);
using var certificate = X509CertificateLoader.LoadPkcs12(pfx, password: null);

CryptographicOperations.ZeroMemory(pfx);

var roots = new X509Certificate2Collection(root);
var context = SslStreamCertificateContext.Create(certificate, roots, offline: true, trust: SslCertificateTrust.CreateForX509Collection(roots, sendTrustInHandshake: false));
var listener = new TcpListener(ipv6 ? IPAddress.IPv6Loopback : IPAddress.Loopback, 0);

listener.Start();

int port = ((IPEndPoint)listener.LocalEndpoint).Port;

await File.WriteAllBytesAsync(Path.Combine(directory, "root.der"), root.Export(X509ContentType.Cert));
await File.WriteAllTextAsync(Path.Combine(directory, "url.txt"), $"https://{(ipv6 ? "[::1]" : "localhost")}:{port}/health");

Console.WriteLine($"Independent .NET TLS {args[1]} test peer on loopback port {port}.");
Console.WriteLine($"TLS peer environment: .NET {Environment.Version}; {Environment.OSVersion.VersionString}.");
Console.WriteLine($"Schannel version: {System.Diagnostics.FileVersionInfo.GetVersionInfo(Path.Combine(Environment.SystemDirectory, "schannel.dll")).FileVersion}; FIPS: {CryptoConfig.AllowOnlyFipsAlgorithms}.");

using (var privateKey = certificate.GetRSAPrivateKey())
{
    Console.WriteLine($"TLS peer key: {privateKey?.GetType().Name}; {privateKey?.KeySize} bits; certificate signature {certificate.SignatureAlgorithm.Value}.");

    if (privateKey is RSACng cng)
    {
        Console.WriteLine($"TLS peer key provider: {cng.Key.Provider?.Provider}; usage {cng.Key.KeyUsage}; machine key {cng.Key.IsMachineKey}.");
    }
}

using var cancellation = new CancellationTokenSource();

Console.CancelKeyPress += (_, eventArgs) =>
{
    eventArgs.Cancel = true;
    cancellation.Cancel();
};

try
{
    while (!cancellation.IsCancellationRequested)
    {
        var socket = await listener.AcceptTcpClientAsync(cancellation.Token);
        _ = Handle(socket);
    }
}
catch (OperationCanceledException)
{
}
finally
{
    listener.Stop();
}

async Task Handle(TcpClient socket)
{
    using (socket)
    using (var stream = new SslStream(socket.GetStream(), leaveInnerStreamOpen: false))

    using (var deadline = CancellationTokenSource.CreateLinkedTokenSource(cancellation.Token))
    {
        deadline.CancelAfter(TimeSpan.FromSeconds(20));

        try
        {
            await LogClientHello(socket, deadline.Token);

            // HTTP/1.1 can use TLS without ALPN. Keep this independent peer focused on
            // record and certificate interoperability; the native round trips verify ALPN.
            await stream.AuthenticateAsServerAsync(new SslServerAuthenticationOptions
            {
                ServerCertificateContext  = context,
                ClientCertificateRequired = false,
                EnabledSslProtocols       = args[1] == "1.2" ? SslProtocols.Tls12 : SslProtocols.Tls13
            }, deadline.Token);

            Console.WriteLine($"Negotiated {stream.SslProtocol}, {stream.NegotiatedCipherSuite}.");

            var request = new MemoryStream();
            var buffer = new byte[1024];

            while (request.Length < 16384)
            {
                int read = await stream.ReadAsync(buffer, deadline.Token);

                if (read == 0)
                {
                    return;
                }

                request.Write(buffer, 0, read);

                if (Encoding.ASCII.GetString(request.GetBuffer(), 0, (int)request.Length).Contains("\r\n\r\n", StringComparison.Ordinal))
                {
                    break;
                }
            }

            string requestText = Encoding.ASCII.GetString(request.GetBuffer(), 0, (int)request.Length);
            string target = requestText.Split(' ', 3)[1];

            Console.WriteLine($"Request {target}.");

            const string success = "HTTP/1.1 200 OK\r\nContent-Length: 17\r\nConnection: close\r\nContent-Type: text/plain\r\n\r\nSeed TLS test OK\n";

            string responseText = target switch
            {
                "/redirect-relative"  => "HTTP/1.1 302 Found\r\nLocation: /health\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
                "/redirect-absolute"  => $"HTTP/1.1 307 Temporary Redirect\r\nLocation: https://127.0.0.1:{port}/health\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
                "/redirect-loop"      => "HTTP/1.1 302 Found\r\nLocation: /redirect-loop\r\nContent-Length: 0\r\nConnection: close\r\n\r\n",
                "/chunked"            => "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\n7\r\nSeed TL\r\na\r\nS test OK\n\r\n0\r\nX-Trailer: complete\r\n\r\n",
                "/informational"      => "HTTP/1.1 100 Continue\r\n\r\n" + success,
                "/duplicate-length"   => "HTTP/1.1 200 OK\r\nContent-Length: 17\r\nContent-Length: 17\r\nConnection: close\r\n\r\nSeed TLS test OK\n",
                "/conflicting-length" => "HTTP/1.1 200 OK\r\nContent-Length: 17\r\nContent-Length: 5\r\nConnection: close\r\n\r\nSeed TLS test OK\n",
                "/ambiguous-framing"  => "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Length: 17\r\nConnection: close\r\n\r\n11\r\nSeed TLS test OK\n\r\n0\r\n\r\n",
                "/truncated"          => "HTTP/1.1 200 OK\r\nContent-Length: 17\r\nConnection: close\r\n\r\nshort",
                "/invalid-chunk"      => "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nConnection: close\r\n\r\ng\r\ninvalid\r\n0\r\n\r\n",
                _                     => success
            };

            byte[] response = Encoding.ASCII.GetBytes(responseText);
            
            // Intentionally fragment application writes for the native record reader.
            for (int index = 0; index < response.Length; index += 7)
            {
                await stream.WriteAsync(response.AsMemory(index, Math.Min(7, response.Length - index)), deadline.Token);
            }

            await stream.ShutdownAsync();
        }
        catch (Exception exception) when (exception is IOException or AuthenticationException or OperationCanceledException)
        {
            Console.Error.WriteLine($"TLS test peer: {exception}");
        }
    }
}

async Task LogClientHello(TcpClient socket, CancellationToken cancellationToken)
{
    var buffer = new byte[18437];

    while (true)
    {
        int count = await socket.Client.ReceiveAsync(buffer, SocketFlags.Peek, cancellationToken);

        if (count == 0)
        {
            throw new IOException("The client closed before sending its TLS record.");
        }

        if (count >= 5)
        {
            int recordLength = 5 + (buffer[3] << 8) + buffer[4];

            if (recordLength > buffer.Length)
            {
                throw new IOException("The initial TLS record exceeds the diagnostic buffer.");
            }

            if (count >= recordLength)
            {
                Console.WriteLine($"TLS initial record: {Convert.ToHexString(buffer.AsSpan(0, recordLength))}");

                return;
            }
        }

        await Task.Delay(10, cancellationToken);
    }
}
