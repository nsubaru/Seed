using Seed.KMS.Transport;
using System.Diagnostics;
using System.IO.Pipes;
using System.Text;
using System.Xml.Linq;

namespace Seed.E2ETests;

public sealed class CancellationTests : E2ETest<CancellationTests>
{
    [Fact]
    public async Task PrematureCommandInputClosesTheRunningSession()
    {
        await Runtime.DisconnectAsync();

        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);

        deadline.CancelAfter(TimeSpan.FromSeconds(15));

        await using (var pipe = new NamedPipeClientStream(".", KmsClient.DefaultPipeName, PipeDirection.InOut, PipeOptions.Asynchronous))
        {
#pragma warning disable xUnit1051 // The pipe deadline is linked to the current test token.

            await pipe.ConnectAsync(deadline.Token);

            pipe.ReadMode = PipeTransmissionMode.Message;

            var bytes = new byte[KmsClient.MaximumMessageBytes];
            int count = await pipe.ReadAsync(bytes, deadline.Token);

            Assert.True(pipe.IsMessageComplete);
            Assert.Equal("SEED_KMS/1", Encoding.Unicode.GetString(bytes, 5, count - 5));

            await pipe.WriteAsync(CommandPacket("tests.run *"), deadline.Token);
            await pipe.WriteAsync(CommandPacket("version"), deadline.Token);

            try
            {
                Assert.Equal(0, await pipe.ReadAsync(bytes, deadline.Token));
            }
            catch (IOException)
            {
                // Either EOF or a broken-pipe status represents the rejected session.
            }
#pragma warning restore xUnit1051
        }

        await Runtime.ReconnectAsync();

        Assert.False(Runtime.HasExited);
        Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));
    }

    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task DisconnectOrCancellationStopsTheRunAndAllowsACompletedNewRun(bool cancelToken)
    {
        const string test = "Seed::Core::Testing::Test::Configurations::SnapshotValidationTest";
        string previous = await Runtime.CommandAsync("status.kernel");
        int processId = Runtime.ProcessId;

        await Runtime.CommandAsync($"tests.run {test}");
        await Runtime.DisconnectAsync();

        await using var connection = new KmsClient();
        using var cancellation = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);

        cancellation.CancelAfter(TimeSpan.FromSeconds(15));

        await connection.ConnectAsync(TimeSpan.FromSeconds(5), TestContext.Current.CancellationToken);
#pragma warning disable xUnit1051 // The shorter cancellation deadline is linked to the current test token.

        Task<IReadOnlyList<KmsDatagram>> command = connection.ExecuteAsync("tests.run *", cancellationToken: cancellation.Token);
#pragma warning restore xUnit1051
        await Task.Delay(200, TestContext.Current.CancellationToken);

        Assert.False(command.IsCompleted, "The full run must still be executing when its session is canceled.");
        var elapsed = Stopwatch.StartNew();

        if (cancelToken)
        {
            cancellation.Cancel();
        }
        else
        {
            await connection.DisposeAsync();
        }

        Exception? failure = await Record.ExceptionAsync(async () => await command.WaitAsync(TimeSpan.FromSeconds(5), TestContext.Current.CancellationToken));

        Assert.True(failure is IOException or ObjectDisposedException or OperationCanceledException, $"Closing a running command must fail its exchange, received {failure}.");

        await Runtime.ReconnectAsync();

        Assert.True(elapsed.Elapsed < TimeSpan.FromSeconds(10), "The canceled suite must release its KMS session after current-case cleanup.");
        Assert.Equal(processId, Runtime.ProcessId);
        Assert.False(Runtime.HasExited);
        Assert.Equal(Seed.E2E.Infrastructure.RuntimeSession.Composition(previous), Seed.E2E.Infrastructure.RuntimeSession.Composition(await Runtime.CommandAsync("status.kernel")));
        Assert.Contains("idle", await Runtime.CommandAsync("network.dhcp.status"));

        string report = Path.Combine(Runtime.Directory, "completed-after-cancellation.xml");
        await Runtime.CommandAsync($"tests.result.store {report}", expectError: true);

        Assert.False(File.Exists(report));

        await Runtime.CommandAsync($"tests.run {test}");
        await Runtime.CommandAsync($"tests.result.store {report}");

        var cases = XDocument.Load(report).Descendants("test-case").ToArray();

        Assert.Single(cases);
        Assert.Equal("Passed", (string?)cases[0].Attribute("result"));
    }

    private static byte[] CommandPacket(string text)
    {
        var bytes = new byte[5 + Encoding.Unicode.GetByteCount(text)];
        bytes[0] = 1;
        bytes[1] = 6;
        
        Encoding.Unicode.GetBytes(text, bytes.AsSpan(5));

        return bytes;
    }
}
