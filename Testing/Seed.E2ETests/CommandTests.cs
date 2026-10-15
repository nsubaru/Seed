using Seed.E2E.Infrastructure;
using Seed.KMS.Transport;
using System.Xml.Linq;

namespace Seed.E2ETests;

public sealed class CommandTests : E2ETest<CommandTests>
{
    internal static readonly string[] Commands = [
        "help", 
        "commands", 
        "version", 
        "exit", 
        "status.kernel", 
        "shutdown", 
        "reload", 
        "configurations.list", 
        "configurations.get", 
        "configurations.set", 
        "configurations.save",
        "configurations.reset", 
        "logging.view", 
        "logging.get", 
        "logging.set", 
        "logging.follow", 
        "tests.list", 
        "tests.run", 
        "tests.result.store", 
        "di.rtti.list", 
        "di.rtti.save", 
        "network.interfaces", 
        "network.dns.resolve", 
        "network.dhcp.start", 
        "network.dhcp.status", 
        "network.dhcp.stop"
    ];

    [Fact]
    public async Task DiscoveryAndHelpMatchTheExactCommandSet()
    {
        foreach (string command in new[]  {  "commands", "help" })
        {
            var actual = (await Runtime.CommandAsync(command)).Split('\n', StringSplitOptions.RemoveEmptyEntries).Select(line => line.Trim()).Order().ToArray();
            Assert.Equal(Commands.Order(), actual);
        }

        Assert.StartsWith("Seed 0.1.0", await Runtime.CommandAsync("version"));
        await Runtime.CommandAsync("unavailable.command", expectError: true);
    }

    [Fact]
    public async Task StatusHasStableLabelsAndFastRebootKeepsTheProcess()
    {
        int processId = Runtime.ProcessId;
        string before = await Runtime.CommandAsync("status.kernel");

        Assert.Contains("Uptime: ", before);
        Assert.Contains("Allocated bytes: ", before);
        Assert.Contains("Services: ", before);

        for (int index = 0; index < 3; ++index)
        {
            string after = await Runtime.FastRebootAsync();
            Assert.NotEqual(RuntimeSession.Composition(before), RuntimeSession.Composition(after));
            Assert.Equal(processId, Runtime.ProcessId);
            Assert.False(Runtime.HasExited);
            Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));
            before = after;
        }
    }

    [Fact]
    public async Task ExitClosesOnlyTheSessionAndShutdownStopsTheOwnedRuntime()
    {
        await Runtime.CommandAsync("exit");
        Assert.False(Runtime.HasExited);

        await Runtime.ReconnectAsync();
        Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));

        await Runtime.CommandAsync("shutdown");
        await Runtime.WaitForExitAsync();
    }

    [Fact]
    public async Task ConfigurationValidatesAndOnlyExplicitSaveSurvivesFastReboot()
    {
        const string name = "core.memory.stringReserveCharacters";
        Assert.Contains(name, await Runtime.CommandAsync("configurations.list"));

        string defaults = await Runtime.CommandAsync($"configurations.get {name}");

        await Runtime.CommandAsync($"configurations.set {name} 73");
        await Runtime.FastRebootAsync();

        Assert.Equal(defaults, await Runtime.CommandAsync($"configurations.get {name}"));

        await Runtime.CommandAsync($"configurations.set {name} 73");

        foreach (string invalid in new[] { "-1", "18446744073709551616",  "1048577" })
        {
            await Runtime.CommandAsync($"configurations.set {name} {invalid}", expectError: true);
        }

        Assert.Contains("= 73 ", await Runtime.CommandAsync($"configurations.get {name}"));
        await Runtime.CommandAsync("configurations.save");
        await Runtime.FastRebootAsync();

        Assert.Contains("= 73 ", await Runtime.CommandAsync($"configurations.get {name}"));
        await Runtime.CommandAsync($"configurations.reset {name}");

        Assert.StartsWith(defaults, await Runtime.CommandAsync($"configurations.get {name}"));
        await Runtime.CommandAsync("configurations.save unexpected", expectError: true);
    }

    [Fact]
    public async Task LogsSupportMasksBufferedReadingAndCancelableFollowing()
    {
        Assert.Contains("HTTP client", await Runtime.CommandAsync("logging.get"));
        await Runtime.CommandAsync("logging.set 28 HTTP client");

        Assert.Contains("= 28", await Runtime.CommandAsync("logging.get HTTP client"));
        await Runtime.CommandAsync("logging.set 32 HTTP client", expectError: true);

        Assert.Contains("= 28", await Runtime.CommandAsync("logging.get HTTP client"));

        await Runtime.CommandAsync("logging.view");
        await Runtime.DisconnectAsync();

        await using (var follower = new KmsClient())
        {
            using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);
            deadline.CancelAfter(TimeSpan.FromSeconds(5));

            await follower.ConnectAsync(TimeSpan.FromSeconds(5), TestContext.Current.CancellationToken);
#pragma warning disable xUnit1051 // The shorter follow deadline is linked to TestContext.Current.CancellationToken above.

            await using var messages = follower.ExecuteStreamingAsync("logging.follow", cancellationToken: deadline.Token).GetAsyncEnumerator();
#pragma warning restore xUnit1051

            Assert.True(await messages.MoveNextAsync());
            deadline.Cancel();

            await Assert.ThrowsAnyAsync<OperationCanceledException>(async () =>
            {
                while (await messages.MoveNextAsync()) {}
            });
        }

        await Runtime.ReconnectAsync();

        Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));

        await Runtime.CommandAsync("configurations.save");
        await Runtime.FastRebootAsync();

        Assert.Contains("= 28", await Runtime.CommandAsync("logging.get HTTP client"));
    }

    [Fact]
    public async Task NativeTestSelectionAndReportHaveNoDuplicateCases()
    {
        const string test = "Seed::Core::Testing::Test::Configurations::SnapshotValidationTest";

        Assert.Contains(test, await Runtime.CommandAsync("tests.list"));

        await Runtime.CommandAsync($"tests.run {test} *SnapshotValidationTest", timeout: TimeSpan.FromMinutes(2));

        string report = Path.Combine(Runtime.Directory, "native.xml");

        await Runtime.CommandAsync($"tests.result.store {report}");

        var cases = XDocument.Load(report).Descendants("test-case").ToArray();

        Assert.Single(cases);
        Assert.Equal("Passed", (string?)cases[0].Attribute("result"));

        await Runtime.CommandAsync("tests.run Seed::Unavailable", expectError: true);
        await Runtime.CommandAsync("tests.run \"unterminated", expectError: true);
    }

    [Fact]
    public async Task ReflectionExportsOnlyTheLiveSeedSurface()
    {
        string types = await Runtime.CommandAsync("di.rtti.list");

        Assert.Contains("Seed::Core::Testing::ITest", types);

        string path = Path.Combine(Runtime.Directory, "rtti.dat");

        Assert.Contains("saved", await Runtime.CommandAsync($"di.rtti.save {path}"));
        Assert.True(new FileInfo(path).Length > 1024);

        await Runtime.CommandAsync("di.rtti.save", expectError: true);
    }

    [Fact]
    public async Task NetworkingQueriesAndInvalidDhcpSelectionsLeaveMaintenanceIdle()
    {
        Assert.NotEmpty(await Runtime.CommandAsync("network.interfaces"));

        // Full resolver lifecycle is tested by the native DNS cases with controlled
        // servers. This KMS test checks argument rejection without external DNS IO.
        await Runtime.CommandAsync("network.dns.resolve", expectError: true);

        Assert.Contains("DHCP maintenance is idle", await Runtime.CommandAsync("network.dhcp.status"));

        foreach (string index in new[] { "0", "4294967296", "-1" })
        {
            await Runtime.CommandAsync($"network.dhcp.start {index}", expectError: true);
        }

        Assert.Contains("No DHCP maintenance", await Runtime.CommandAsync("network.dhcp.stop 0"));

        await Runtime.CommandAsync("network.dhcp.status unexpected", expectError: true);
        
        Assert.Contains("DHCP maintenance is idle", await Runtime.CommandAsync("network.dhcp.status"));
    }
}
