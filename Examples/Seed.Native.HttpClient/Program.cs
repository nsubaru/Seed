using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;

var options = new Dictionary<string, string>(StringComparer.Ordinal);
for (int index = 0; index < args.Length; ++index)
{
    if (!args[index].StartsWith("--", StringComparison.Ordinal) || index + 1 == args.Length)
    {
        throw new ArgumentException("Use --url <url> --output <file>, optionally --runtime <exe>, --trust-root <DER>, --tls <1.2|1.3>, --connect-address <IP>.");
    }

    options.Add(args[index], args[++index]);
}

if (!options.TryGetValue("--url", out string? url) || !Uri.TryCreate(url, UriKind.Absolute, out var uri) || uri.Scheme is not ("http" or "https"))
{
    throw new ArgumentException("An absolute HTTP or HTTPS URL is required.");
}

if (!options.TryGetValue("--output", out string? target))
{
    throw new ArgumentException("An output file is required.");
}

var root = Directory.GetCurrentDirectory();
while (!File.Exists(Path.Combine(root, "Seed.slnx")))
{
    root = Directory.GetParent(root)?.FullName ?? throw new DirectoryNotFoundException("Run this example from a Seed checkout.");
}

var runtime = Path.GetFullPath(options.GetValueOrDefault("--runtime", Path.Combine(root, "Artifacts/x64/Debug/Runtime/Seed.Runtime.exe")));
var output = Path.GetFullPath(target);
if (new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator))
{
    throw new InvalidOperationException("Run the client demonstration under a non-elevated account.");
}

Directory.CreateDirectory(Path.GetDirectoryName(output)!);
if (File.Exists(output))
{
    File.Delete(output);
}

if (File.Exists(output + ".headers.txt"))
{
    File.Delete(output + ".headers.txt");
}

if (File.Exists(output + ".error.txt"))
{
    File.Delete(output + ".error.txt");
}

Environment.SetEnvironmentVariable("SEED_RUN_TESTS", null);
Environment.SetEnvironmentVariable("SEED_TEST_REPORT", null);
Environment.SetEnvironmentVariable("SEED_RTTI_EXPORT", null);
Environment.SetEnvironmentVariable("SEED_HTTP_URL", url);
Environment.SetEnvironmentVariable("SEED_HTTP_OUTPUT", output);
Environment.SetEnvironmentVariable("SEED_HTTP_TRUST_ROOT", options.TryGetValue("--trust-root", out var trustRoot) ? Path.GetFullPath(trustRoot) : null);
Environment.SetEnvironmentVariable("SEED_HTTP_PROTOCOL", options.GetValueOrDefault("--tls"));
Environment.SetEnvironmentVariable("SEED_HTTP_CONNECT_ADDRESS", options.GetValueOrDefault("--connect-address"));
var status = Starter.SeedStartRuntime(runtime, "Seed.Runtime.exe", out ulong processId);
if (status < 0)
{
    throw new InvalidOperationException($"Native startup failed: NTSTATUS 0x{status:X8}.");
}

using var process = Process.GetProcessById(checked((int)processId));
using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(30));
try
{
    await process.WaitForExitAsync(deadline.Token);
}
catch (OperationCanceledException)
{
    process.Kill();
    throw new TimeoutException("The native client demonstration exceeded its deadline.");
}

if (process.ExitCode != 0 || !File.Exists(output + ".headers.txt"))
{
    if (File.Exists(output + ".error.txt"))
    {
        Console.Error.Write(await File.ReadAllTextAsync(output + ".error.txt"));
    }

    throw new InvalidOperationException($"The native client did not produce a response (exit 0x{process.ExitCode:X8}); diagnostics: {output}.error.txt.");
}

Console.Write(await File.ReadAllTextAsync(output + ".headers.txt"));
Console.WriteLine($"Native response body: {output} ({new FileInfo(output).Length} bytes).");
return 0;
internal static class Starter
{
    [DllImport("Seed.Starter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int SeedStartRuntime(string path, string commandLine, out ulong processId);
}
