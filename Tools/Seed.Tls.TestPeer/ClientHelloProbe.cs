using System.Diagnostics;
using System.Net.Security;
using System.Net.Sockets;
using System.Security.Authentication;
using System.Security.Cryptography.X509Certificates;

internal static class ClientHelloProbe
{
    // Replay the public ClientHello fixture only through the server's first flight.
    // No private scalar or session secret is needed for this negotiation check.
    internal static async Task Run(string template, string output)
    {
        string directory = Path.GetFullPath(output);
        Directory.CreateDirectory(directory);
        string urlPath = Path.Combine(directory, "url.txt");
        File.Delete(urlPath);
        var start = new ProcessStartInfo(Path.Combine(AppContext.BaseDirectory, "Seed.Tls.TestPeer.exe"))
        {
            UseShellExecute = false,
            CreateNoWindow = true,
            RedirectStandardOutput = true,
            RedirectStandardError = true
        };
        start.ArgumentList.Add(directory);
        start.ArgumentList.Add("1.2");
        using var peer = Process.Start(start)!;
        var stdout = peer.StandardOutput.ReadToEndAsync();
        var stderr = peer.StandardError.ReadToEndAsync();

        try
        {
            var deadline = DateTime.UtcNow.AddSeconds(10);
            while (!File.Exists(urlPath))
            {
                if (peer.HasExited || DateTime.UtcNow > deadline)
                {
                    throw new IOException("Peer did not become ready.");
                }
                await Task.Delay(25);
            }
            bool nativeAccepted = false;
            var uri = new Uri(await File.ReadAllTextAsync(urlPath));
            byte[] original = Convert.FromHexString(await File.ReadAllTextAsync(template));

            foreach (var (name, omitted, group, signatures) in new (string, int, byte[]?, byte[]?)[]
            {
                ("native", -1, null, null),
                ("without-supported-versions", 43, null, null),
                ("without-certificate-signatures", 50, null, null),
                ("without-alpn", 16, null, null),
                ("x25519-and-p256", -1, new byte[] {0, 4, 0, 29, 0, 23}, null),
                ("p256-only", -1, new byte[] {0, 2, 0, 23}, null),
                ("rsa-pkcs1-signatures", -1, null, new byte[] {0, 6, 4, 1, 5, 1, 6, 1}),
                ("rsa-pss-signatures", -1, null, new byte[] {0, 6, 8, 4, 8, 5, 8, 6}),
                ("minimal-x25519", -2, null, null),
                ("minimal-p256", -2, new byte[] {0, 2, 0, 23}, null)
            })
            {
                byte[] frame = Modify(original, omitted, group, signatures);
                using var client = new TcpClient();
                using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(3));
                await client.ConnectAsync("127.0.0.1", uri.Port, timeout.Token);
                var stream = client.GetStream();
                await stream.WriteAsync(frame, timeout.Token);
                byte[] header = new byte[5];
                await stream.ReadExactlyAsync(header, timeout.Token);
                byte[] body = new byte[Read16(header, 3)];
                await stream.ReadExactlyAsync(body, timeout.Token);
                Console.WriteLine($"{name}: type {header[0]}, body {Convert.ToHexString(body.AsSpan(0, Math.Min(8, body.Length)))}");
                nativeAccepted |= name == "native" && header[0] == 22 && body.Length >= 4 && body[0] == 2;
            }

            using var managed = new TcpClient();
            using var managedDeadline = new CancellationTokenSource(TimeSpan.FromSeconds(10));
            await managed.ConnectAsync("127.0.0.1", uri.Port, managedDeadline.Token);
            using var ssl = new SslStream(managed.GetStream());
            var policy = new X509ChainPolicy
            {
                TrustMode = X509ChainTrustMode.CustomRootTrust,
                RevocationMode = X509RevocationMode.NoCheck
            };
            using var root = X509CertificateLoader.LoadCertificateFromFile(Path.Combine(directory, "root.der"));
            policy.CustomTrustStore.Add(root);
            await ssl.AuthenticateAsClientAsync(new SslClientAuthenticationOptions
            {
                TargetHost = "localhost",
                EnabledSslProtocols = SslProtocols.Tls12,
                CertificateChainPolicy = policy,
                ApplicationProtocols = [SslApplicationProtocol.Http11]
            }, managedDeadline.Token);
            Console.WriteLine($"managed-control: {ssl.SslProtocol}, {ssl.NegotiatedCipherSuite}");
            if (!nativeAccepted)
            {
                throw new AuthenticationException("Schannel rejected the Seed TLS 1.2 ClientHello offer.");
            }
        }
        finally
        {
            if (!peer.HasExited)
            {
                peer.Kill();
                await peer.WaitForExitAsync();
            }
            await File.WriteAllTextAsync(Path.Combine(directory, "peer.log"), await stdout);
            await File.WriteAllTextAsync(Path.Combine(directory, "peer-error.log"), await stderr);
        }
    }

    static int Read16(byte[] value, int offset) => (value[offset] << 8) + value[offset + 1];

    static byte[] Modify(byte[] value, int omitted, byte[]? group, byte[]? signatures)
    {
        int offset = 9 + 34;
        offset += 1 + value[offset];
        offset += 2 + Read16(value, offset);
        offset += 1 + value[offset];
        int lengthOffset = offset;
        offset += 2;
        var result = new List<byte>(value.AsSpan(0, offset).ToArray());
        while (offset < value.Length)
        {
            int kind = Read16(value, offset);
            int count = Read16(value, offset + 2);
            if (kind != omitted && (omitted != -2 || kind is 10 or 13 or 11 or 65281))
            {
                byte[] payload = kind == 10 && group != null ? group : kind == 13 && signatures != null ? signatures : value.AsSpan(offset + 4, count).ToArray();
                result.AddRange(new byte[] {(byte)(kind >> 8), (byte)kind, (byte)(payload.Length >> 8), (byte)payload.Length});
                result.AddRange(payload);
            }
            offset += 4 + count;
        }
        byte[] bytes = result.ToArray();
        int extensionLength = bytes.Length - lengthOffset - 2;
        bytes[lengthOffset] = (byte)(extensionLength >> 8);
        bytes[lengthOffset + 1] = (byte)extensionLength;
        int handshakeLength = bytes.Length - 9;
        bytes[6] = (byte)(handshakeLength >> 16);
        bytes[7] = (byte)(handshakeLength >> 8);
        bytes[8] = (byte)handshakeLength;
        int recordLength = bytes.Length - 5;
        bytes[3] = (byte)(recordLength >> 8);
        bytes[4] = (byte)recordLength;
        return bytes;
    }
}
