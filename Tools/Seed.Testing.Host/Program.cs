using Seed.E2E.Infrastructure;
using Seed.CTTI;
using Seed.RTTI;
using System.Xml.Linq;

if (args.Length != 4 || args[0] is not ("discover" or "run"))
{
    throw new ArgumentException("Usage: Seed.Testing.Host <discover|run> <runtime> <selection.xml|-> <output.xml>");
}

string output = Path.GetFullPath(args[3]);

Directory.CreateDirectory(Path.GetDirectoryName(output)!);

using var canceled = new CancellationTokenSource(TimeSpan.FromMinutes(15));

// The adapter cancels cooperatively through stdin, allowing owned-process cleanup.
var ready = new TaskCompletionSource(TaskCreationOptions.RunContinuationsAsynchronously);

_ = Task.Run(async () =>
{
    for (; ; )
    {
        string? line = await Console.In.ReadLineAsync();

        if (line == "run")
        {
            ready.TrySetResult();
        }
        else if (line == "cancel" || line is null)
        {
            canceled.Cancel();

            return;
        }
    }
});

await using var runtime = await RuntimeSession.StartAsync(args[1], Path.Combine(Path.GetDirectoryName(output)!, "sessions"), canceled.Token);

Console.WriteLine($"SEED_PROCESS {runtime.ProcessId}");
Console.Out.Flush();

// Acknowledgement allows Test Explorer to attach to the native process before Execute.
if (args[0] == "run")
{
    await ready.Task.WaitAsync(canceled.Token);
}

string[] available = (await runtime.CommandAsync("tests.list")).Split('\n', StringSplitOptions.RemoveEmptyEntries).Select(name => name.Trim()).ToArray();

if (available.Length == 0 || available.Distinct().Count() != available.Length)
{
    throw new InvalidDataException("Native test discovery was empty or contained duplicate registrations.");
}

if (args[0] == "discover")
{
    // Preserve the original adapter's RTTI-based discovery contract, using metadata
    // fixed up by the running native image instead of unfixed executable pointers.
    string metadata = Path.Combine(runtime.Directory, "rtti.dat");

    await runtime.CommandAsync($"di.rtti.save {metadata}");

    using var section = new RTTISection(await File.ReadAllBytesAsync(metadata, canceled.Token));
    var reflected = section.AllRegisteredTypes.Where(type => type.IsValid && type.TypeClass == ETypes.Object && type.Interfaces.Any(contract => contract.TypeName == "Seed::Core::Testing::ITest")).Select(type => type.TypeName).Order().ToArray();

    if (!reflected.SequenceEqual(available.Order()))
    {
        throw new InvalidDataException($"KMS discovery ({available.Length}) and live ITest RTTI ({reflected.Length}) disagree. Missing RTTI: {string.Join(", ", available.Except(reflected).Take(6))}; extra RTTI: {string.Join(", ", reflected.Except(available).Take(6))}");
    }

    using (var file = File.Create(output))
        await new XDocument(new XElement("tests", available.Select(name => new XElement("test", new XAttribute("name", name))))).SaveAsync(file, SaveOptions.None, canceled.Token);

    return;
}

var requested = XDocument.Load(args[2]).Descendants("test").Select(test => (string?)test.Attribute("name") ?? "").Distinct().ToArray();

if (requested.Length == 0 || requested.Except(available).Any())
{
    throw new InvalidDataException("Requested tests are empty or unavailable in the matching native image.");
}

var results = new XElement("test-run");

for (int offset = 0; offset < requested.Length;)
{
    var batch      = new List<string>();
    int characters = 10;

    while (offset < requested.Length && batch.Count < 128 && characters + requested[offset].Length + 3 < 30000)
    {
        characters += requested[offset].Length + 3;
        batch.Add(requested[offset++]);
    }

    if (batch.Count == 0)
    {
        throw new InvalidDataException("A native test name exceeds the KMS selection limit.");
    }

    string command = "tests.run " + string.Join(' ', batch.Select(name => '"' + name + '"'));

    try
    {
        await runtime.CommandAsync(command, timeout: TimeSpan.FromMinutes(10));
    }
    catch (InvalidOperationException)
    { /* tests.result.store preserves failed native cases too. */
    }

    string report = Path.Combine(runtime.Directory, $"native-{offset}.xml");

    await runtime.CommandAsync($"tests.result.store {report}");

    var cases = XDocument.Load(report).Descendants("test-case").ToArray();
    
    if (cases.Length != batch.Count || !cases.Select(test => (string?)test.Attribute("fullname")).Order().SequenceEqual(batch.Order()))
    {
        throw new InvalidDataException("Native report does not match the complete requested batch.");
    }

    results.Add(cases);
}

using (var file = File.Create(output))
    await new XDocument(results).SaveAsync(file, SaveOptions.None, canceled.Token);
