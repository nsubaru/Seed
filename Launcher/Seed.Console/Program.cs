using Seed.KMS.Transport;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;

using var cancellation = new CancellationTokenSource();

Console.CancelKeyPress += (_, eventArgs) =>
{
    eventArgs.Cancel = true;
    cancellation.Cancel();
};

try
{
    if (args.Length > 0 && args[0] == "--boot-demo")
    {
        if (args.Length != 3)
        {
            throw new ArgumentException("Use --boot-demo <Seed.Runtime.exe> <new-output-directory>.");
        }

        if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
        {
            throw new InvalidOperationException("Start ordinary Seed demonstrations under a non-elevated account.");
        }

        string runtime = Path.GetFullPath(args[1]);
        string directory = Path.GetFullPath(args[2]);

        if (!File.Exists(runtime))
        {
            throw new FileNotFoundException("The Seed runtime was not found.", runtime);
        }
        if (Directory.Exists(directory) || File.Exists(directory))
        {
            throw new IOException("Choose a new output directory for the native demonstration.");
        }

        Directory.CreateDirectory(directory);
        Environment.SetEnvironmentVariable("SEED_CONFIG_DIRECTORY", Path.Combine(directory, "Data"));

        int status = Starter.SeedStartRuntime(runtime, $"Seed.Runtime.exe --boot-demo \"{directory}\"", out ulong processId);

        if (status < 0)
        {
            throw new InvalidOperationException($"Native startup failed: NTSTATUS 0x{status:X8}.");
        }

        using var process = Process.GetProcessById(checked((int)processId));
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(cancellation.Token);

        deadline.CancelAfter(TimeSpan.FromMinutes(1));

        try
        {
            await process.WaitForExitAsync(deadline.Token);
            string report = Path.Combine(directory, "boot-demo.txt");

            if (process.ExitCode != 0 || !File.Exists(report))
            {
                throw new InvalidOperationException($"Native demonstration failed with exit code {process.ExitCode}; output: {directory}");
            }

            Console.Write(await File.ReadAllTextAsync(report, cancellation.Token));

            return 0;
        }
        catch (OperationCanceledException) when (!cancellation.IsCancellationRequested)
        {
            throw new TimeoutException("The native demonstration exceeded its one-minute deadline.");
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

    string pipeName = KmsClient.DefaultPipeName;

    if (args.Length > 0 && args[0] == "--pipe")
    {
        if (args.Length < 2)
        {
            throw new ArgumentException("Use --pipe <local-Seed-pipe-name> before --start or a command.");
        }

        pipeName = args[1];
        args = args[2..];
    }

    await using var client = new KmsClient(pipeName);

    if (args.Length > 0 && args[0] == "--start")
    {
        if (args.Length < 2)
        {
            throw new ArgumentException("Use --start <Seed.Runtime.exe> followed by an optional command.");
        }

        if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
        {
            throw new InvalidOperationException("Start ordinary Seed demonstrations under a non-elevated account.");
        }

        string runtime = Path.GetFullPath(args[1]);
        if (!File.Exists(runtime))
        {
            throw new FileNotFoundException("The Seed runtime was not found.", runtime);
        }

        foreach (string name in new[] { "SEED_RUN_TESTS", "SEED_TEST_REPORT", "SEED_RTTI_EXPORT", "SEED_HTTP_URL", "SEED_HTTP_OUTPUT", "SEED_HTTP_TRUST_ROOT", "SEED_HTTP_PROTOCOL", "SEED_HTTP_CONNECT_ADDRESS" })
        {
            Environment.SetEnvironmentVariable(name, null);
        }

        int status = Starter.SeedStartRuntime(runtime, "Seed.Runtime.exe", out ulong processId);

        if (status < 0)
        {
            throw new InvalidOperationException($"Native startup failed: NTSTATUS 0x{status:X8}.");
        }

        Console.WriteLine($"Started Seed runtime process {processId}.");

        args = args[2..];
    }

    await client.ConnectAsync(TimeSpan.FromSeconds(10), cancellation.Token);

    async Task<bool> Execute(string command)
    {
        bool reload = command == "reload" || command.StartsWith("reload ", StringComparison.Ordinal);

        if (reload)
        {
            Console.WriteLine("Waiting for Seed runtime reinitialization...");
        }

        bool succeeded = true;
        TextWriter? lastWriter = null;
        bool endsWithNewline = true;

        await foreach (var datagram in client.ExecuteStreamingAsync(command, cancellationToken: cancellation.Token))
        {
            var writer = datagram.IsError ? Console.Error : Console.Out;
            writer.Write(datagram.Text);

            if (datagram.Text.Length != 0)
            {
                lastWriter = writer;
                endsWithNewline = datagram.Text.EndsWith('\n');
            }

            succeeded &= !datagram.IsError;
        }

        if (!endsWithNewline)
        {
            lastWriter?.WriteLine();
        }

        if (reload && succeeded)
        {
            Console.WriteLine("Seed runtime reinitialized; KMS reconnected.");
        }

        return succeeded;
    }

    if (args.Length != 0)
    {
        bool quotedArguments = args[0].StartsWith("fs.", StringComparison.Ordinal) ||
            args[0].StartsWith("registry.", StringComparison.Ordinal);

        string command = quotedArguments
            ? args[0] + " " + string.Join(' ', args.Skip(1).Select(value => "\"" + value.Replace("\"", "\"\"") + "\""))
            : string.Join(' ', args);

        return await Execute(command) ? 0 : 1;
    }

    Console.WriteLine("Seed console; help lists commands. Ctrl+C cancels the session.");

    for (; ; )
    {
        Console.Write("seed> ");
        string? command = Console.ReadLine();

        if (command is null)
        {
            break;
        }

        if (string.IsNullOrWhiteSpace(command))
        {
            continue;
        }

        await Execute(command);

        if (command == "exit")
        {
            break;
        }
    }

    return 0;
}
catch (OperationCanceledException) when (cancellation.IsCancellationRequested)
{
    return 130;
}
catch (OperationCanceledException)
{
    Console.Error.WriteLine("The local Seed endpoint did not become ready before the connection deadline. Check its startup log and configured pipe name.");

    return 1;
}
catch (Exception exception)
{
    Console.Error.WriteLine(exception.Message);

    return 1;
}

internal static class Starter
{
    [DllImport("Seed.Starter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int SeedStartRuntime(string path, string commandLine, out ulong processId);
}
