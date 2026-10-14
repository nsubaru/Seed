using System.Net.Sockets;
using System.Text;

namespace Seed.E2ETests;

public sealed class HttpLifecycleTests : E2ETest<HttpLifecycleTests>
{
    [Fact]
    public async Task CompletedConnectionsReleaseAdmissionCapacityBeforeTheNextAccept()
    {
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);
        deadline.CancelAfter(TimeSpan.FromSeconds(30));
        var sockets = new List<TcpClient>();
        byte[] headers = Encoding.ASCII.GetBytes("POST /echo HTTP/1.1\r\nHost: localhost\r\nContent-Length: 1\r\n\r\n");
        byte[] body = [(byte)'x'];
        try
        {
            // Keep every admitted request incomplete so the listener's retained
            // work list reaches its limit before it waits for another accept.
            for (int index = 0; index < 16; ++index)
            {
                var socket = new TcpClient();
                sockets.Add(socket);
                await socket.ConnectAsync("127.0.0.1", 8080, deadline.Token);
                await socket.GetStream().WriteAsync(headers, deadline.Token);
            }

            // A seventeenth request confirms the listener has admitted all
            // sixteen incomplete requests before this test completes them.
            using (var busyProbe = new TcpClient())
            {
                await busyProbe.ConnectAsync("127.0.0.1", 8080, deadline.Token);
                await busyProbe.GetStream().WriteAsync(Encoding.ASCII.GetBytes("GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n"), deadline.Token);
                try
                {
                    Assert.Empty(await ReadResponseAsync(busyProbe, deadline.Token));
                }
                catch (IOException exception) when (exception.InnerException is SocketException socket && socket.SocketErrorCode is SocketError.ConnectionReset or SocketError.ConnectionAborted)
                {
                    // Closing an over-capacity socket with unread request bytes
                    // produces a reset instead of an orderly end of stream.
                }
            }

            foreach (var socket in sockets)
            {
                await socket.GetStream().WriteAsync(body, deadline.Token);
            }

            foreach (var socket in sockets)
            {
                string response = await ReadResponseAsync(socket, deadline.Token);
                Assert.StartsWith("HTTP/1.1 200 ", response);
                Assert.EndsWith("\r\n\r\nx", response);
            }

            // The listener was awaiting a connection while the preceding work
            // finished. Admission must reclaim that work after accept too.
            using var probe = new TcpClient();
            await probe.ConnectAsync("127.0.0.1", 8080, deadline.Token);
            await probe.GetStream().WriteAsync(Encoding.ASCII.GetBytes("GET /health HTTP/1.1\r\nHost: localhost\r\n\r\n"), deadline.Token);
            string health = await ReadResponseAsync(probe, deadline.Token);
            Assert.StartsWith("HTTP/1.1 200 ", health);
            Assert.EndsWith("\r\n\r\nSeed is running\n", health);
        }
        finally
        {
            foreach (var socket in sockets)
            {
                socket.Dispose();
            }
        }

        Assert.False(Runtime.HasExited);
        Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));
    }

    private static async Task<string> ReadResponseAsync(TcpClient socket, CancellationToken token)
    {
        var buffer = new byte[4096];
        using var response = new MemoryStream();
        int count;
        while ((count = await socket.GetStream().ReadAsync(buffer, token)) != 0)
        {
            response.Write(buffer, 0, count);
            Assert.True(response.Length <= 65536, "HTTP response exceeded the test limit.");
        }

        return Encoding.UTF8.GetString(response.ToArray());
    }
}
