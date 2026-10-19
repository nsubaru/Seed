using System.Diagnostics;

namespace Seed.E2ETests;

public sealed class BootDemoTests
{
    [Fact]
    public async Task BoundedNativeDemoProducesFilesAndReturnsWithoutAnEndpoint()
    {
        string runtime = Path.GetFullPath(Environment.GetEnvironmentVariable("SEED_E2E_RUNTIME")!);
        string console = Path.Combine(Path.GetDirectoryName(Path.GetDirectoryName(runtime)!)!, "Seed.Console", "Seed.Console.exe");
        string directory = Path.Combine(Environment.GetEnvironmentVariable("SEED_E2E_OUTPUT")!, "native demo " + Guid.NewGuid().ToString("N"));

        var start = new ProcessStartInfo(console)
        {
            UseShellExecute = false,
            CreateNoWindow = true,
            RedirectStandardOutput = true,
            RedirectStandardError = true
        };

        foreach (string argument in new[] { "--boot-demo", runtime, directory })
        {
            start.ArgumentList.Add(argument);
        }

        using var process = Process.Start(start)!;
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);

        deadline.CancelAfter(TimeSpan.FromSeconds(75));

        try
        {
#pragma warning disable xUnit1051 // The owned-process deadline is linked to the test token.
            Task<string> output = process.StandardOutput.ReadToEndAsync(deadline.Token);
            Task<string> errors = process.StandardError.ReadToEndAsync(deadline.Token);

            await process.WaitForExitAsync(deadline.Token);
#pragma warning restore xUnit1051

            Assert.Equal(0, process.ExitCode);
            Assert.Empty(await errors);

            string report = await output;

            Assert.Contains("Native demonstration completed; returning to Windows startup.", report);
            Assert.Contains("> process.list", report);
            Assert.Contains("> registry.get", report);
            Assert.Equal("Hello from Seed during native startup", await File.ReadAllTextAsync(Path.Combine(directory, "files", "native.txt"), TestContext.Current.CancellationToken));
            Assert.True(File.Exists(Path.Combine(directory, "files", "renamed.txt")));
            Assert.False(File.Exists(Path.Combine(directory, "files", "copy.txt")));
            Assert.True(new FileInfo(Path.Combine(directory, "boot-rtti.dat")).Length > 1024);
        }
        finally
        {
            if (!process.HasExited)
            {
                process.Kill(entireProcessTree: true);

                await process.WaitForExitAsync(CancellationToken.None);
            }
        }
    }
}
