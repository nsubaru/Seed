using System.Buffers.Binary;
using System.Net;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;
using System.Security.Principal;
using System.Text;

if (args.Length != 2 || !int.TryParse(args[0], out int port) || port is < 1 or > 65535)
{
    throw new ArgumentException("Use <Seed-HTTPS-port> <trusted-root.der>.");
}

if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
{
    throw new InvalidOperationException("Run server interoperability under a non-elevated account.");
}

using var root = X509CertificateLoader.LoadCertificateFromFile(args[1]);
using var wrongKey = RSA.Create(2048);
var wrongRequest = new CertificateRequest($"CN=Unrelated verifier root {Guid.NewGuid():N}", wrongKey, HashAlgorithmName.SHA256, RSASignaturePadding.Pkcs1);
wrongRequest.CertificateExtensions.Add(new X509BasicConstraintsExtension(true, false, 0, true));
using var wrongRoot = wrongRequest.CreateSelfSigned(DateTimeOffset.UtcNow.AddHours(-1), DateTimeOffset.UtcNow.AddHours(1));
SslClientAuthenticationOptions Options(SslProtocols version, string name, X509Certificate2 anchor, bool wrongAlpn = false) => new()
{
    TargetHost = name,
    EnabledSslProtocols = version,
    AllowRenegotiation = false,
    ApplicationProtocols = [wrongAlpn ? SslApplicationProtocol.Http2 : SslApplicationProtocol.Http11],
    CertificateChainPolicy = new X509ChainPolicy
    {
        TrustMode = X509ChainTrustMode.CustomRootTrust,
        CustomTrustStore =
        {
            anchor
        },
        RevocationMode = X509RevocationMode.NoCheck,
        DisableCertificateDownloads = true
    }
};
async Task<byte[]> ExchangeAsync(SslProtocols version, string path, byte[] body, string name = "localhost", X509Certificate2? anchor = null, bool wrongAlpn = false, int? proxyPort = null)
{
    using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(15));
    using var socket = new TcpClient();
    await socket.ConnectAsync(IPAddress.Loopback, proxyPort ?? port, deadline.Token);
    using var stream = new SslStream(socket.GetStream());
    await stream.AuthenticateAsClientAsync(Options(version, name, anchor ?? root, wrongAlpn), deadline.Token);
    if (stream.SslProtocol != version || stream.NegotiatedApplicationProtocol != SslApplicationProtocol.Http11 || !stream.IsAuthenticated || !stream.IsEncrypted || stream.IsMutuallyAuthenticated)
    {
        throw new InvalidDataException("Unexpected TLS version, ALPN or authentication state.");
    }

    Console.WriteLine($"Independent client negotiated {stream.SslProtocol}, {stream.NegotiatedCipherSuite}.");
    string method = body.Length == 0 ? "GET" : "POST";
    byte[] header = Encoding.ASCII.GetBytes($"{method} {path} HTTP/1.1\r\nHost: localhost\r\nContent-Length: {body.Length}\r\nConnection: close\r\n\r\n");
    await stream.WriteAsync(header, deadline.Token);
    for (int i = 0; i < body.Length; i += 997)
    {
        await stream.WriteAsync(body.AsMemory(i, Math.Min(997, body.Length - i)), deadline.Token);
    }

    using var response = new MemoryStream();
    var buffer = new byte[1311];
    for (; ; )
    {
        int read = await stream.ReadAsync(buffer, deadline.Token);
        if (read == 0)
        {
            break;
        }

        response.Write(buffer, 0, read);
        if (response.Length > 2 * 1024 * 1024)
        {
            throw new InvalidDataException("Oversized verifier response.");
        }
    }

    byte[] bytes = response.ToArray();
    int separator = bytes.AsSpan().IndexOf("\r\n\r\n"u8);
    if (separator < 0)
    {
        throw new InvalidDataException("Incomplete HTTP response.");
    }

    string headers = Encoding.ASCII.GetString(bytes, 0, separator);
    if (!headers.StartsWith("HTTP/1.1 200 ", StringComparison.Ordinal))
    {
        throw new InvalidDataException(headers);
    }

    string length = headers.Split("\r\n").Single(line => line.StartsWith("Content-Length: ", StringComparison.OrdinalIgnoreCase))[16..];
    byte[] payload = bytes[(separator + 4)..];
    if (int.Parse(length) != payload.Length)
    {
        throw new InvalidDataException("Truncated or excess response payload.");
    }

    return payload;
}

async Task ExpectFailureAsync(string label, Func<Task> action)
{
    try
    {
        await action();
    }
    catch (Exception exception) when (exception is AuthenticationException or IOException)
    {
        Console.WriteLine($"Rejected {label}: {exception.GetType().Name}.");

        return;
    }

    throw new InvalidDataException($"Unexpected success for {label}.");
}

foreach (var version in new[]
{
    SslProtocols.Tls12,
    SslProtocols.Tls13
}

)
{
    var health = await ExchangeAsync(version, "/health", []);
    if (!health.AsSpan().SequenceEqual("Seed is running\n"u8))
    {
        throw new InvalidDataException("Incorrect health payload.");
    }

    var body = Enumerable.Range(0, 192 * 1024 + 17).Select(i => (byte)(i * 23 + 9)).ToArray();
    var echo = await ExchangeAsync(version, "/echo", body);
    if (!echo.AsSpan().SequenceEqual(body))
    {
        throw new InvalidDataException("Fragmented TLS echo differs from its input.");
    }

    byte[] json = "{\"Sequence\":123,\"Message\":\"independent TLS client\"}"u8.ToArray();
    using var document = System.Text.Json.JsonDocument.Parse(await ExchangeAsync(version, "/json", json));
    if (document.RootElement.GetProperty("Sequence").GetInt64() != 123 || document.RootElement.GetProperty("Message").GetString() != "independent TLS client")
    {
        throw new InvalidDataException("Reflected HTTPS JSON round trip failed.");
    }

    await ExpectFailureAsync($"{version} wrong hostname", async () =>
    {
        await ExchangeAsync(version, "/health", [], name: "wrong.example");
    });
    await ExpectFailureAsync($"{version} untrusted root", async () =>
    {
        await ExchangeAsync(version, "/health", [], anchor: wrongRoot);
    });
    await ExpectFailureAsync($"{version} unsupported ALPN", async () =>
    {
        await ExchangeAsync(version, "/health", [], wrongAlpn: true);
    });
    await VerifyRecordProxyAsync(version, tamper: false);
    await VerifyRecordProxyAsync(version, tamper: true);
}

Console.WriteLine("Independent HTTPS server interoperability passed.");
async Task VerifyRecordProxyAsync(SslProtocols version, bool tamper)
{
    using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(15));
    using var listener = new TcpListener(IPAddress.Loopback, 0);
    listener.Start();
    int proxyPort = ((IPEndPoint)listener.LocalEndpoint).Port;
    bool modified = false;
    Task proxy = ForwardAsync();
    try
    {
        if (tamper)
        {
            await ExpectFailureAsync($"{version} modified protected client record", async () =>
            {
                await ExchangeAsync(version, "/health", [], proxyPort: proxyPort);
            });
        }
        else
        {
            byte[] body = await ExchangeAsync(version, "/health", [], proxyPort: proxyPort);
            if (!body.AsSpan().SequenceEqual("Seed is running\n"u8))
            {
                throw new InvalidDataException("Fragmented handshake proxy returned a different payload.");
            }
        }

        if (tamper && !modified)
        {
            throw new InvalidDataException("Record-tampering scenario never modified a protected record.");
        }
    }
    finally
    {
        await deadline.CancelAsync();
        try
        {
            await proxy;
        }
        catch (Exception exception) when (exception is IOException or OperationCanceledException or SocketException)
        {
        }

        listener.Stop();
    }

    async Task ForwardAsync()
    {
        using var client = await listener.AcceptTcpClientAsync(deadline.Token);
        using var server = new TcpClient();
        await server.ConnectAsync(IPAddress.Loopback, port, deadline.Token);
        await Task.WhenAll(PumpAsync(client.GetStream(), server.GetStream(), fromClient: true), PumpAsync(server.GetStream(), client.GetStream(), fromClient: false));
    }

    async Task PumpAsync(NetworkStream input, NetworkStream output, bool fromClient)
    {
        bool cipherChanged = false;
        var header = new byte[5];
        for (; ; )
        {
            int initial = await input.ReadAsync(header.AsMemory(0, 1), deadline.Token);
            if (initial == 0)
            {
                return;
            }

            await input.ReadExactlyAsync(header.AsMemory(1), deadline.Token);
            int size = BinaryPrimitives.ReadUInt16BigEndian(header.AsSpan(3));
            if (size > 18432)
            {
                throw new InvalidDataException("Proxy record bound exceeded.");
            }

            byte[] body = new byte[size];
            await input.ReadExactlyAsync(body, deadline.Token);
            if (header[0] == 20)
            {
                cipherChanged = true;
            }

            bool encrypted = version == SslProtocols.Tls13 ? header[0] == 23 : cipherChanged && header[0] is 22 or 23;
            if (tamper && fromClient && encrypted && !modified && size != 0)
            {
                body[^1] ^= 1;
                modified = true;
            }

            if (header[0] == 22 && !cipherChanged && body.Length != 0)
            {
                // Reframe plaintext handshakes without changing their transcript bytes.
                // Each message now crosses TLS records as well as TCP writes.
                for (int offset = 0; offset < body.Length; offset += 37)
                {
                    int count = Math.Min(37, body.Length - offset);
                    byte[] fragmentHeader = (byte[])header.Clone();
                    BinaryPrimitives.WriteUInt16BigEndian(fragmentHeader.AsSpan(3), (ushort)count);
                    await ForwardFragmentAsync(fragmentHeader, body.AsMemory(offset, count));
                }
            }
            else
            {
                await ForwardFragmentAsync(header, body);
            }
        }

        async Task ForwardFragmentAsync(byte[] recordHeader, ReadOnlyMemory<byte> recordBody)
        {
            for (int i = 0; i < recordHeader.Length; ++i)
            {
                await output.WriteAsync(recordHeader.AsMemory(i, 1), deadline.Token);
            }

            for (int i = 0; i < recordBody.Length; i += 113)
            {
                await output.WriteAsync(recordBody.Slice(i, Math.Min(113, recordBody.Length - i)), deadline.Token);
            }
        }
    }
}
