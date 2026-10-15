using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.Xml.Linq;

var options = new Dictionary<string, string>(StringComparer.Ordinal);

for (var index = 0; index < args.Length; ++index)
{
    if (!args[index].StartsWith("--") || index + 1 == args.Length)
    {
        throw new ArgumentException("Options require a name and value: --runtime, --wildcard, --test, --report, --rtti, --configuration-directory, --timeout-seconds.");
    }

    options.Add(args[index], args[++index]);
}

string Option(string key, string fallback) => options.GetValueOrDefault(key, fallback);

var root          = Directory.GetCurrentDirectory();
var runtime       = Path.GetFullPath(Option("--runtime", Path.Combine(root, "Artifacts/x64/Debug/Runtime/Seed.Runtime.exe")));
var report        = Path.GetFullPath(Option("--report", Path.Combine(root, "Artifacts/tests/native.xml")));
var selection     = Option("--test", Option("--wildcard", "*"));
var configuration = Path.GetFullPath(Option("--configuration-directory", Path.Combine(root, "Artifacts/tests/configuration")));
var metadata      = options.TryGetValue("--rtti", out var rtti) ? Path.GetFullPath(rtti) : "";
var elevated      = new WindowsPrincipal(WindowsIdentity.GetCurrent()).IsInRole(WindowsBuiltInRole.Administrator);

Console.WriteLine($"Elevated: {elevated}");

if (elevated)
{
    throw new InvalidOperationException("Run standard Seed tests under a non-elevated account.");
}

if (!File.Exists(runtime))
{
    throw new FileNotFoundException("Build the matching Seed runtime first.", runtime);
}

// Network self-tests currently use fixed loopback ports. Keep runner invocations
// under this Windows account sequential, including different checkout/configuration paths.
using var testLease = await AcquireTestLeaseAsync();

Directory.CreateDirectory(configuration);
Directory.CreateDirectory(Path.GetDirectoryName(report)!);

if (metadata.Length != 0)
{
    Directory.CreateDirectory(Path.GetDirectoryName(metadata)!);
}

if (File.Exists(report))
{
    File.Delete(report);
}

Environment.SetEnvironmentVariable("SEED_CONFIG_DIRECTORY", configuration);
Environment.SetEnvironmentVariable("SEED_RUN_TESTS", selection);
Environment.SetEnvironmentVariable("SEED_TEST_REPORT", report);
Environment.SetEnvironmentVariable("SEED_RTTI_EXPORT", metadata);

var status = Starter.SeedStartRuntime(runtime, "Seed.Runtime.exe", out var processId);

if (status < 0)
{
    throw new InvalidOperationException($"Native startup failed: NTSTATUS 0x{status:X8}.");
}

Console.WriteLine($"Started Seed process {processId}; selection {selection}.");

using var process = Process.GetProcessById(checked((int)processId));
using var deadline = new CancellationTokenSource(TimeSpan.FromSeconds(double.Parse(Option("--timeout-seconds", "600"), System.Globalization.CultureInfo.InvariantCulture)));

try
{
    await process.WaitForExitAsync(deadline.Token);
}
catch (OperationCanceledException)
{
    process.Kill();
    throw new TimeoutException("The launched Seed test process exceeded its deadline.");
}

if (process.ExitCode != 0)
{
    throw new InvalidOperationException($"Seed exited with 0x{process.ExitCode:X8}.");
}

if (!File.Exists(report))
{
    throw new InvalidOperationException("Seed exited without creating the requested test report; inspect its startup log.");
}

var run = XDocument.Load(report).Root ?? throw new InvalidDataException("Empty test report.");
var tests = run.Descendants("test-case").ToArray();

bool IsIntentionalSkip(XElement test) => (string?)test.Attribute("result") == "Skipped" && test.Element("reason")?.Element("message")?.Value == "Returned false";

var skipped = tests.Count(IsIntentionalSkip);
var failed = tests.Count(test => (string?)test.Attribute("result") != "Passed" && !IsIntentionalSkip(test));

Console.WriteLine($"Native tests: {tests.Length}; passed: {tests.Length - failed - skipped}; skipped: {skipped}; failed: {failed}; report: {report}.");

foreach (var test in tests.Where(test => (string?)test.Attribute("result") != "Passed" && !IsIntentionalSkip(test)))
{
    Console.WriteLine($"FAIL {(string?)test.Attribute("fullname")}: {test.Element("failure")?.Element("message")?.Value}");
}

if (tests.Length == 0 || failed != 0)
{
    return 1;
}

return 0;

async Task<FileStream> AcquireTestLeaseAsync()
{
    var path = Path.Combine(Path.GetTempPath(), "Seed.Native.TestRunner.v1.lock");
    var deadline = Stopwatch.StartNew();
    bool reportedWait = false;
    for (; ; )
    {
        try
        {
            return new FileStream(path, FileMode.OpenOrCreate, FileAccess.ReadWrite, FileShare.None);
        }
        catch (IOException) when (deadline.Elapsed < TimeSpan.FromMinutes(20))
        {
            if (!reportedWait)
            {
                Console.WriteLine("Waiting for another Seed native test run to finish.");
                reportedWait = true;
            }

            await Task.Delay(TimeSpan.FromMilliseconds(100));
        }
    }
}

internal static class Starter
{
    [DllImport("Seed.Starter.dll", CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
    internal static extern int SeedStartRuntime(string path, string commandLine, out ulong processId);
}
