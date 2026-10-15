using Seed.KMS.Transport;
using System.Net.Sockets;
using System.Security.Principal;
using System.Text;

if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
{
    throw new InvalidOperationException("Run console integration under a non-elevated account.");
}

async Task VerifyReconnectAsync()
{
    await using var client = new KmsClient();

    await client.ConnectAsync(TimeSpan.FromSeconds(5));

    var result = await client.ExecuteAsync("version");

    if (result.Count == 0 || result.Any(packet => packet.IsError) || !result[0].Text.StartsWith("Seed ", StringComparison.Ordinal))
    {
        throw new InvalidDataException("A fresh session did not complete its version command.");
    }
}

async Task WriteControlledLogAsync(CancellationToken token)
{
    using var socket = new TcpClient();

    await socket.ConnectAsync("127.0.0.1", 8080, token);

    var stream  = socket.GetStream();
    var request = Encoding.ASCII.GetBytes("POST /echo HTTP/1.1\r\nHost: localhost\r\nContent-Length: 4\r\nContent-Length: 5\r\n\r\ntest");

    await stream.WriteAsync(request, token);

    var response = new byte[4096];
    int read     = await stream.ReadAsync(response, token);

    if (!Encoding.ASCII.GetString(response, 0, read).StartsWith("HTTP/1.1 400 ", StringComparison.Ordinal))
    {
        throw new InvalidDataException("The controlled malformed HTTP request was not rejected.");
    }
}

await using (var client = new KmsClient())
{
    await client.ConnectAsync(TimeSpan.FromSeconds(5));

    var rejected = await client.ExecuteAsync("logging.follow unexpected");

    if (rejected.Count != 1 || !rejected[0].IsError)
    {
        throw new InvalidDataException("logging.follow accepted unexpected arguments.");
    }

    var version = new StringBuilder();

    await foreach (var packet in client.ExecuteStreamingAsync("version"))
    {
        if (packet.IsError)
        {
            throw new InvalidDataException(packet.Text);
        }

        version.Append(packet.Text);
    }

    if (!version.ToString().StartsWith("Seed ", StringComparison.Ordinal))
    {
        throw new InvalidDataException("Finite streaming commands did not complete.");
    }

    using var cancellation = new CancellationTokenSource(TimeSpan.FromSeconds(10));
    await using var follow = client.ExecuteStreamingAsync("logging.follow", cancellationToken: cancellation.Token).GetAsyncEnumerator();

    if (!await follow.MoveNextAsync() || follow.Current.IsError || !follow.Current.Text.StartsWith("Following new Seed log output", StringComparison.Ordinal))
    {
        throw new InvalidDataException("The live subscription did not become ready.");
    }

    await WriteControlledLogAsync(cancellation.Token);

    var output = new StringBuilder();

    while (!output.ToString().Contains("Malformed HTTP request:", StringComparison.Ordinal))
    {
        if (!await follow.MoveNextAsync() || follow.Current.IsError)
        {
            throw new InvalidDataException("Live log output stopped unexpectedly.");
        }

        output.Append(follow.Current.Text);

        if (output.Length > 65536)
        {
            throw new InvalidDataException("Expected log message was missing from live output.");
        }
    }

    cancellation.Cancel();

    try
    {
        await follow.MoveNextAsync();
        throw new InvalidDataException("Canceling a live subscription did not cancel its pending read.");
    }
    catch (OperationCanceledException) when (cancellation.IsCancellationRequested)
    {
    }
}

await VerifyReconnectAsync();
await using (var client = new KmsClient())
{
    await client.ConnectAsync(TimeSpan.FromSeconds(5));

    await using var follow = client.ExecuteStreamingAsync("logging.follow").GetAsyncEnumerator();

    if (!await follow.MoveNextAsync())
    {
        throw new InvalidDataException("The subscription ended before it became ready.");
    }
    // Leaving after the first packet must close the session and unsubscribe too.
}

await VerifyReconnectAsync();

for (int iteration = 0; iteration < 8; ++iteration)
{
    using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(10));

    await using var client = new KmsClient();

    await client.ConnectAsync(TimeSpan.FromSeconds(5), deadline.Token);
    var follow = client.ExecuteStreamingAsync("logging.follow", cancellationToken: deadline.Token).GetAsyncEnumerator();

    await using (follow)
    {
        if (!await follow.MoveNextAsync())
        {
            throw new InvalidDataException("The subscription did not become ready for concurrent logging.");
        }

        var writes = Enumerable.Range(0, 8).Select(_ => WriteControlledLogAsync(deadline.Token)).ToArray();
        
        await follow.DisposeAsync();
        await Task.WhenAll(writes);
    }

    await VerifyReconnectAsync();
}

Console.WriteLine("KMS streaming, live Core log events, cancellation, concurrent disposal and reconnect passed.");
