using Seed.KMS.Transport;
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Security.Principal;

namespace Seed.E2E.Infrastructure;

/// <summary>
/// Owns one ordinary-user native runtime, isolated files and bounded local KMS exchanges.
/// A per-account file lease serializes tests that use fixed native loopback ports.
/// </summary>
public sealed class RuntimeSession : IAsyncDisposable
{
    private readonly FileStream lease;
    private readonly Process process;
    private readonly CancellationTokenSource lifetime;
    private readonly SemaphoreSlim commandGate = new(1);
    private KmsClient? client;

    private RuntimeSession(FileStream lease, Process process, string directory, CancellationToken token)
    {
        this.lease = lease;
        this.process = process;
        Directory = directory;
        lifetime = CancellationTokenSource.CreateLinkedTokenSource(token);
        lifetime.CancelAfter(TimeSpan.FromMinutes(15));
    }

    /// <summary>
    /// Absolute artifact directory containing this session's configuration and command transcript.
    /// Files remain available after disposal for failure diagnosis.
    /// </summary>
    public string Directory
    {
        get;
    }

    /// <summary>
    /// Identifier of the native process owned by this session.
    /// </summary>
    public int ProcessId => process.Id;

    /// <summary>
    /// Current exit state of the owned native process.
    /// </summary>
    public bool HasExited => process.HasExited;

    /// <summary>
    /// Starts a matching native image through the staged Seed starter and connects without authentication.
    /// </summary>
    /// <param name="runtime">Absolute path of the native image.</param>
    /// <param name="output">Parent directory for a fresh session artifact directory.</param>
    /// <param name="cancellationToken">Cancellation of lease acquisition, startup and subsequent session operations.</param>
    /// <returns>An initialized session owning only the process it launched.</returns>
    public static async Task<RuntimeSession> StartAsync(string runtime, string output, CancellationToken cancellationToken = default)
    {
        if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
        {
            throw new InvalidOperationException("Run ordinary Seed tests without elevation.");
        }

        runtime = Path.GetFullPath(runtime);
        if (!File.Exists(runtime))
        {
            throw new FileNotFoundException("Build the matching Seed runtime first.", runtime);
        }

        using var startup = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        startup.CancelAfter(TimeSpan.FromMinutes(20));
        FileStream lease;
        for (; ; )
        {
            startup.Token.ThrowIfCancellationRequested();
            try
            {
                lease = new FileStream(Path.Combine(Path.GetTempPath(), "Seed.Native.TestRunner.v1.lock"), FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.None);
                break;
            }
            catch (IOException)
            {
                await Task.Delay(100, startup.Token);
            }
        }

        RuntimeSession? session = null;
        Process? ownedProcess = null;
        try
        {
            // The shipped default pipe has one console slot. Refuse to connect to
            // an existing runtime whose lifecycle this fixture does not own.
            if (System.IO.Directory.EnumerateFiles(@"\\.\pipe\").Any(path => Path.GetFileName(path).Equals("Seed.KMS.v1", StringComparison.OrdinalIgnoreCase)))
            {
                throw new InvalidOperationException("The default Seed KMS endpoint already exists; use an isolated test account/session.");
            }

            string directory = Path.Combine(Path.GetFullPath(output), Guid.NewGuid().ToString("N"));
            System.IO.Directory.CreateDirectory(directory);
            System.IO.Directory.CreateDirectory(Path.Combine(directory, "configuration"));
            var environment = new Dictionary<string, string?>
            {
                ["SEED_CONFIG_DIRECTORY"] = Path.Combine(directory, "configuration"),
                ["SEED_RUN_TESTS"] = null,
                ["SEED_TEST_REPORT"] = null,
                ["SEED_RTTI_EXPORT"] = null,
                ["SEED_HTTP_URL"] = null,
                ["SEED_HTTP_OUTPUT"] = null,
                ["SEED_HTTP_TRUST_ROOT"] = null,
                ["SEED_HTTP_PROTOCOL"] = null,
                ["SEED_HTTP_CONNECT_ADDRESS"] = null
            };
            var previous = environment.Keys.ToDictionary(key => key, Environment.GetEnvironmentVariable);
            ulong processId;
            try
            {
                foreach (var (name, value) in environment)
                {
                    Environment.SetEnvironmentVariable(name, value);
                }

                int status = Starter.SeedStartRuntime(runtime, "Seed.Runtime.exe", out processId);
                if (status < 0)
                {
                    throw new InvalidOperationException($"Native startup failed: NTSTATUS 0x{status:X8}.");
                }

                ownedProcess = Process.GetProcessById(checked((int)processId));
            }
            finally
            {
                foreach (var (name, value) in previous)
                {
                    Environment.SetEnvironmentVariable(name, value);
                }
            }

            session = new RuntimeSession(lease, ownedProcess, directory, cancellationToken);
            await session.ReconnectAsync();
            await session.CommandAsync("status.kernel");

            return session;
        }
        catch
        {
            if (session is not null)
            {
                await session.DisposeAsync();
            }
            else
            {
                if (ownedProcess is not null)
                {
                    if (!ownedProcess.HasExited)
                    {
                        ownedProcess.Kill();
                    }

                    ownedProcess.Dispose();
                }

                lease.Dispose();
            }

            throw;
        }
    }

    /// <summary>
    /// Executes a finite command and retains the complete bounded response in the session transcript.
    /// </summary>
    /// <param name="command">Command name followed by optional argument text.</param>
    /// <param name="expectError">Whether an error datagram is required instead of forbidden.</param>
    /// <param name="timeout">Optional command deadline; the default is thirty seconds.</param>
    /// <returns>The complete textual response after the command-ready marker.</returns>
    public async Task<string> CommandAsync(string command, bool expectError = false, TimeSpan? timeout = null)
    {
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(lifetime.Token);
        deadline.CancelAfter(timeout ?? TimeSpan.FromSeconds(30));
        await commandGate.WaitAsync(deadline.Token);
        try
        {
            client ??= await ConnectAsync(deadline.Token);
            var output = await client.ExecuteAsync(command, cancellationToken: deadline.Token);
            // KMS may split one logical response inside a line at its packet limit.
            string text = string.Concat(output.Select(item => item.Text));
            await File.AppendAllTextAsync(Path.Combine(Directory, "commands.log"), $"> {command}\n{text}\n", CancellationToken.None);
            bool error = output.Any(item => item.IsError);
            if (error != expectError)
            {
                throw new InvalidOperationException($"Unexpected KMS error state for {command}:\n{text}");
            }

            return text;
        }
        finally
        {
            commandGate.Release();
        }
    }

    /// <summary>
    /// Replaces the local pipe connection without changing runtime ownership.
    /// </summary>
    /// <returns>A task completing after the versioned initialization marker is received.</returns>
    public async Task ReconnectAsync()
    {
        await commandGate.WaitAsync(lifetime.Token);
        try
        {
            if (client is not null)
            {
                await client.DisposeAsync();
            }

            client = null;
            client = await ConnectAsync(lifetime.Token);
        }
        finally
        {
            commandGate.Release();
        }
    }

    /// <summary>
    /// Releases the current command connection so an independent local console can connect.
    /// </summary>
    /// <returns>A task completing after the command pipe is closed.</returns>
    public async Task DisconnectAsync()
    {
        await commandGate.WaitAsync(lifetime.Token);
        try
        {
            if (client is not null)
            {
                await client.DisposeAsync();
            }

            client = null;
        }
        finally
        {
            commandGate.Release();
        }
    }

    private static async Task<KmsClient> ConnectAsync(CancellationToken token)
    {
        var connection = new KmsClient();
        try
        {
            await connection.ConnectAsync(TimeSpan.FromSeconds(15), token);

            return connection;
        }
        catch
        {
            await connection.DisposeAsync();
            throw;
        }
    }

    /// <summary>
    /// Performs Core fast reboot through reload and waits for a new composition in the same process.
    /// </summary>
    /// <returns>The status response for the completed new composition.</returns>
    public async Task<string> FastRebootAsync()
    {
        string previous = Composition(await CommandAsync("status.kernel"));
        await CommandAsync("reload");
        string status = await CommandAsync("status.kernel");
        if (Composition(status) == previous || process.HasExited)
        {
            throw new InvalidOperationException("Reload returned before a new runtime composition was ready.");
        }

        return status;
    }

    /// <summary>
    /// Extracts the stable composition identifier from a status.kernel response.
    /// </summary>
    /// <param name="status">Complete status text.</param>
    /// <returns>The non-empty composition value.</returns>
    public static string Composition(string status) => status.Split('\n').Single(line => line.StartsWith("Composition: ", StringComparison.Ordinal))["Composition: ".Length..].Trim();

    /// <summary>
    /// Waits for intentional shutdown of this session's process within a fixed deadline.
    /// </summary>
    /// <returns>A task completing after a successful native process exit.</returns>
    public async Task WaitForExitAsync()
    {
        using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(30));
        await process.WaitForExitAsync(deadline.Token);
        if (process.ExitCode != 0)
        {
            throw new InvalidOperationException($"Seed exited with 0x{process.ExitCode:X8}.");
        }
    }

    /// <summary>
    /// Closes KMS, requests shutdown and terminates only the owned process if bounded cleanup expires.
    /// </summary>
    /// <returns>A task completing after the process and per-account test lease are released.</returns>
    public async ValueTask DisposeAsync()
    {
        try
        {
            if (client is not null)
            {
                await client.DisposeAsync();
            }

            if (!process.HasExited)
            {
                try
                {
                    using var cleanup = new CancellationTokenSource(TimeSpan.FromSeconds(10));
                    await using var connection = await ConnectAsync(cleanup.Token);
                    try
                    {
                        await connection.ExecuteAsync("shutdown", cancellationToken: cleanup.Token);
                    }
                    catch (IOException)
                    {
                    }

                    await process.WaitForExitAsync(cleanup.Token);
                }
                catch (Exception exception) when (exception is IOException or OperationCanceledException or InvalidOperationException)
                {
                    if (!process.HasExited)
                    {
                        process.Kill();
                    }

                    await process.WaitForExitAsync();
                }
            }
        }
        finally
        {
            lifetime.Dispose();
            process.Dispose();
            lease.Dispose();
            commandGate.Dispose();
        }
    }

    private static class Starter
    {
        [DllImport("Seed.Starter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        internal static extern int SeedStartRuntime(string path, string commandLine, out ulong processId);
    }
}
