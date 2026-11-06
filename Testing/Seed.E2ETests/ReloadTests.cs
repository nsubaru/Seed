using Seed.E2E.Infrastructure;
using Seed.KMS.Transport;
using System.Diagnostics;

namespace Seed.E2ETests;

public sealed class ReloadTests : E2ETest<ReloadTests>
{
    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task ReconnectFollowsSavedPipeChangesAndIgnoresUnsavedPipeChanges(bool singleProcessor)
    {
        // Arrange
        if (singleProcessor)
        {
            // Exercise shutdown callbacks that remain queued while the main thread tears down services.
            using var process = Process.GetProcessById(Runtime.ProcessId);
            long affinity = process.ProcessorAffinity.ToInt64();
            process.ProcessorAffinity = new IntPtr(affinity & -affinity);
        }

        // Arrange
        string custom = "Seed.KMS.ReloadTest." + Guid.NewGuid().ToString("N");

        // Act
        await Runtime.DisconnectAsync();

        // Arrange
        await using var client = new KmsClient();
        var token = TestContext.Current.CancellationToken;

        // Act
        await client.ConnectAsync(TimeSpan.FromSeconds(5), token);
        await client.ExecuteAsync($"configurations.set host.kms.pipeName {custom}", cancellationToken: token);
        await client.ExecuteAsync("reload", cancellationToken: token);

        // Assert
        Assert.Contains("Seed 0.1.0", Text(await client.ExecuteAsync("version", cancellationToken: token)));
        Assert.Contains($"Next KMS endpoint: {KmsClient.DefaultPipeName}", Text(await client.ExecuteAsync("configurations.get host.kms.pipeName", cancellationToken: token)));

        // Act
        await client.ExecuteAsync($"configurations.set host.kms.pipeName {custom}", cancellationToken: token);
        await client.ExecuteAsync("configurations.save", cancellationToken: token);

        // Assert
        Assert.Contains($"Next KMS endpoint: {custom}", Text(await client.ExecuteAsync("configurations.get host.kms.pipeName", cancellationToken: token)));

        // Act
        await client.ExecuteAsync("reload", cancellationToken: token);

        // Assert
        Assert.Contains("Seed 0.1.0", Text(await client.ExecuteAsync("version", cancellationToken: token)));

        // Act
        await client.ExecuteAsync("configurations.reset host.kms.pipeName", cancellationToken: token);
        await client.ExecuteAsync("configurations.save", cancellationToken: token);
        await client.ExecuteAsync("reload", cancellationToken: token);

        // Assert
        Assert.Contains("Seed 0.1.0", Text(await client.ExecuteAsync("version", cancellationToken: token)));

        // Act
        await client.DisposeAsync();
        await Runtime.ReconnectAsync();
    }

    [Theory]
    [InlineData(false)]
    [InlineData(true)]
    public async Task ConsoleReloadWaitsForReinitializationAndRemainsConnected(bool interactive)
    {
        // Act
        string before = RuntimeSession.Composition(await Runtime.CommandAsync("status.kernel"));

        // Arrange
        int processId = Runtime.ProcessId;

        // Act
        await Runtime.DisconnectAsync();

        // Arrange
        string runtime = Path.GetFullPath(Environment.GetEnvironmentVariable("SEED_E2E_RUNTIME")!);
        string console = Path.Combine(Path.GetDirectoryName(Path.GetDirectoryName(runtime)!)!, "Seed.Console", "Seed.Console.exe");

        // Assert
        Assert.True(File.Exists(console), "Build the matching console demonstration before running its E2E checks.");

        // Arrange
        var start = new ProcessStartInfo(console)
        {
            UseShellExecute = false,
            RedirectStandardInput = interactive,
            RedirectStandardOutput = true,
            RedirectStandardError = true,
            WorkingDirectory = Runtime.Directory
        };

        // Act
        if (!interactive)
        {
            start.ArgumentList.Add("reload");
        }

        // Arrange
        using var process = Process.Start(start)!;
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);

        // Act
        deadline.CancelAfter(TimeSpan.FromSeconds(30));

        // Assert
        try
        {
#pragma warning disable xUnit1051 // The owned-process deadline is linked to the current test token.

            Task<string> output = process.StandardOutput.ReadToEndAsync(deadline.Token);
            Task<string> errors = process.StandardError.ReadToEndAsync(deadline.Token);

            if (interactive)
            {
                await process.StandardInput.WriteLineAsync("reload".AsMemory(), deadline.Token);
                await process.StandardInput.WriteLineAsync("version".AsMemory(), deadline.Token);
                await process.StandardInput.WriteLineAsync("exit".AsMemory(), deadline.Token);

                process.StandardInput.Close();
            }

            await process.WaitForExitAsync(deadline.Token);

#pragma warning restore xUnit1051
            Assert.Equal(0, process.ExitCode);
            Assert.Empty(await errors);

            string text = await output;

            Assert.Contains("Runtime transition requested", text);
            Assert.Contains("Waiting for Seed runtime reinitialization...", text);
            Assert.Contains("Seed runtime reinitialized; KMS reconnected.", text);

            if (interactive)
            {
                Assert.Contains("Seed 0.1.0", text);
            }

            await Runtime.ReconnectAsync();

            string after = RuntimeSession.Composition(await Runtime.CommandAsync("status.kernel"));

            Assert.NotEqual(before, after);
            Assert.Equal(processId, Runtime.ProcessId);
            Assert.False(Runtime.HasExited);
        }
        finally
        {
            if (!process.HasExited)
            {
                process.Kill();

                await process.WaitForExitAsync(CancellationToken.None);
            }
        }
    }

    private static string Text(IReadOnlyList<KmsDatagram> output)
    {
        Assert.DoesNotContain(output, datagram => datagram.IsError);

        return string.Concat(output.Select(datagram => datagram.Text));
    }
}
