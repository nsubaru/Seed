using Microsoft.Win32;
using System.Diagnostics;
using System.Text;
using System.Text.RegularExpressions;

namespace Seed.E2ETests;

public sealed class SystemCommandTests : E2ETest<SystemCommandTests>
{
    private static string Quote(string value) => "\"" + value.Replace("\"", "\"\"") + "\"";

    [Fact]
    public async Task FilesSupportUnicodeQuotingAndNonDestructiveMutations()
    {
        string directory = Path.Combine(Runtime.Directory, "files with spaces 🌱");
        string source = Path.Combine(directory, "first.txt");
        string copy = Path.Combine(directory, "copy.txt");
        string moved = Path.Combine(directory, "renamed.txt");
        const string text = "Seed says \"hello\" 🌱";

        await Runtime.CommandAsync($"fs.directory.create {Quote(directory)}");
        await Runtime.CommandAsync($"fs.write {Quote(source)} {Quote(text)}");

        Assert.Equal(text, await File.ReadAllTextAsync(source, TestContext.Current.CancellationToken));
        Assert.Equal(text, await Runtime.CommandAsync($"fs.read {Quote(source)}"));
        Assert.Contains($"Size: {Encoding.UTF8.GetByteCount(text)} bytes", await Runtime.CommandAsync($"fs.info {Quote(source)}"));
        Assert.Contains("first.txt", await Runtime.CommandAsync($"fs.list {Quote(directory)}"));

        await Runtime.CommandAsync($"fs.write {Quote(source)} replacement", expectError: true);
        await Runtime.CommandAsync($"fs.copy {Quote(source)} {Quote(copy)}");
        await Runtime.CommandAsync($"fs.copy {Quote(source)} {Quote(copy)}", expectError: true);
        await Runtime.CommandAsync($"fs.move {Quote(source)} {Quote(copy)}", expectError: true);

        Assert.True(File.Exists(source));
        Assert.Equal(text, await File.ReadAllTextAsync(copy, TestContext.Current.CancellationToken));

        await Runtime.CommandAsync($"fs.move {Quote(copy)} {Quote(moved)}");

        Assert.False(File.Exists(copy));
        Assert.True(File.Exists(moved));

        await Runtime.CommandAsync($"fs.delete {Quote(directory)}", expectError: true);

        await Runtime.CommandAsync($"fs.delete {Quote(source)}");
        await Runtime.CommandAsync($"fs.delete {Quote(moved)}");
        await Runtime.CommandAsync($"fs.delete {Quote(directory)}");

        Assert.False(Directory.Exists(directory));
    }

    [Fact]
    public async Task FileTablesAlignNumericSizesAndKeepUnicodeNamesIntact()
    {
        string directory = Path.Combine(Runtime.Directory, "table");

        Directory.CreateDirectory(directory);
        Directory.CreateDirectory(Path.Combine(directory, "subdirectory"));

        await File.WriteAllBytesAsync(Path.Combine(directory, "a.txt"), new byte[1], TestContext.Current.CancellationToken);
        const string longName = "long file name with spaces 🌱.txt";
        await File.WriteAllBytesAsync(Path.Combine(directory, longName), new byte[12345], TestContext.Current.CancellationToken);

        string output = await Runtime.CommandAsync($"fs.list {Quote(directory)}");
        Assert.DoesNotContain('\t', output);

        string[] lines = output.Split('\n', StringSplitOptions.RemoveEmptyEntries);
        Assert.Equal(5, lines.Length);

        int nameColumn = lines[0].IndexOf("Name", StringComparison.Ordinal);
        int sizeEnd = lines[0].IndexOf("Size (bytes)", StringComparison.Ordinal) + "Size (bytes)".Length;
        var rows = lines.Skip(2).Select(line => Regex.Match(line, @"^(File|Directory)  +(\d+|-)  +(.+)$")).ToArray();

        Assert.All(rows, row =>
        {
            Assert.True(row.Success);
            Assert.Equal(sizeEnd, row.Groups[2].Index + row.Groups[2].Length);
            Assert.Equal(nameColumn, row.Groups[3].Index);
            Assert.Equal(row.Value.TrimEnd(), row.Value);
        });

        Assert.Equal("12345", Assert.Single(rows, row => row.Groups[3].Value == longName).Groups[2].Value);
        Assert.Equal("1", Assert.Single(rows, row => row.Groups[3].Value == "a.txt").Groups[2].Value);
        Assert.Equal("-", Assert.Single(rows, row => row.Groups[3].Value == "subdirectory").Groups[2].Value);

        string empty = Path.Combine(directory, "empty");
        Directory.CreateDirectory(empty);

        string emptyTable = await Runtime.CommandAsync($"fs.list {Quote(empty)}");
        Assert.Equal(2, emptyTable.Split('\n', StringSplitOptions.RemoveEmptyEntries).Length);
    }

    [Fact]
    public async Task BinaryCopiesSpanChunksAndTextReadsRejectInvalidInput()
    {
        string source = Path.Combine(Runtime.Directory, "binary.dat");
        string copy = Path.Combine(Runtime.Directory, "binary-copy.dat");

        byte[] bytes = new byte[256 * 1024 + 17];
        new Random(42).NextBytes(bytes);

        await File.WriteAllBytesAsync(source, bytes, TestContext.Current.CancellationToken);

        await Runtime.CommandAsync($"fs.copy {Quote(source)} {Quote(copy)}");

        Assert.Equal(bytes, await File.ReadAllBytesAsync(copy, TestContext.Current.CancellationToken));

        await Runtime.CommandAsync($"fs.read {Quote(source)}", expectError: true);

        string invalid = Path.Combine(Runtime.Directory, "invalid.txt");

        await File.WriteAllBytesAsync(invalid, new byte[] { 0xC0, 0xAF }, TestContext.Current.CancellationToken);
        await Runtime.CommandAsync($"fs.read {Quote(invalid)}", expectError: true);

        string empty = Path.Combine(Runtime.Directory, "empty.txt");

        await Runtime.CommandAsync($"fs.write {Quote(empty)} {Quote("")}");

        Assert.Equal(0, new FileInfo(empty).Length);
        Assert.Empty(await Runtime.CommandAsync($"fs.read {Quote(empty)}"));

        await Runtime.CommandAsync("fs.read relative.txt", expectError: true);
        await Runtime.CommandAsync("fs.write", expectError: true);
        await Runtime.CommandAsync("fs.read \"unterminated", expectError: true);
        await Runtime.CommandAsync("registry.unavailable", expectError: true);

        Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));
    }

    [Fact]
    public async Task RegistryValuesRoundTripThroughTheWindowsRegistry()
    {
        string relative = "Software\\Seed.DemoTests." + Guid.NewGuid().ToString("N");
        string key = "HKCU\\" + relative;
        string child = key + "\\Child key";

        using var fixture = Registry.CurrentUser.CreateSubKey(relative);

        try
        {
            await Runtime.CommandAsync($"registry.key.create {Quote(child)}");
            await Runtime.CommandAsync($"registry.set {Quote(child)} {Quote("")} string {Quote("Hello \"registry\" 🌱")}");
            await Runtime.CommandAsync($"registry.set {Quote(child)} Count dword 42");
            await Runtime.CommandAsync($"registry.set {Quote(child)} Large qword 18446744073709551615");

            Assert.Equal("Hello \"registry\" 🌱", await Runtime.CommandAsync($"registry.get {Quote(child)} {Quote("")}"));
            Assert.Equal("42", await Runtime.CommandAsync($"registry.get {Quote(child)} Count"));
            Assert.Equal("18446744073709551615", await Runtime.CommandAsync($"registry.get {Quote(child)} Large"));

            using (var actual = fixture.OpenSubKey("Child key")!)
            {
                Assert.Equal("Hello \"registry\" 🌱", actual.GetValue(""));
                Assert.Equal(42, actual.GetValue("Count"));
                Assert.Equal(-1L, actual.GetValue("Large"));
            }

            Assert.Contains("[key] Child key", await Runtime.CommandAsync($"registry.list {Quote(key)}"));
            Assert.Contains("[value] Count", await Runtime.CommandAsync($"registry.list {Quote(child)}"));

            await Runtime.CommandAsync($"registry.set {Quote(child)} Count dword 4294967296", expectError: true);
            await Runtime.CommandAsync($"registry.set {Quote(child)} Count binary 1", expectError: true);

            Assert.Equal("42", await Runtime.CommandAsync($"registry.get {Quote(child)} Count"));

            await Runtime.CommandAsync($"registry.key.delete {Quote(key)}", expectError: true);
            await Runtime.CommandAsync($"registry.value.delete {Quote(child)} Count");
            await Runtime.CommandAsync($"registry.get {Quote(child)} Count", expectError: true);
            await Runtime.CommandAsync($"registry.key.delete {Quote(child)}");

            Assert.Null(fixture.OpenSubKey("Child key"));
        }
        finally
        {
            Registry.CurrentUser.DeleteSubKeyTree(relative, throwOnMissingSubKey: false);
        }
    }

    [Fact]
    public async Task ProcessesListAndTerminateOnlyTheSelectedOwnedChild()
    {
        var start = new ProcessStartInfo(Path.Combine(Environment.SystemDirectory, "WindowsPowerShell", "v1.0", "powershell.exe"))
        {
            UseShellExecute = false,
            CreateNoWindow = true
        };

        foreach (string argument in new[] { "-NoProfile", "-NonInteractive", "-Command", "Start-Sleep -Seconds 120" })
        {
            start.ArgumentList.Add(argument);
        }

        using var child = Process.Start(start)!;

        try
        {
            string list = await Runtime.CommandAsync("process.list");
            string[] lines = list.Split('\n', StringSplitOptions.RemoveEmptyEntries);

            Assert.DoesNotContain('\t', list);
            Assert.Matches(@"^\s*PID  +Parent PID  +Image$", lines[0]);

            int imageColumn = lines[0].IndexOf("Image", StringComparison.Ordinal);
            int pidEnd = lines[0].IndexOf("PID", StringComparison.Ordinal) + "PID".Length;
            int parentEnd = lines[0].IndexOf("Parent PID", StringComparison.Ordinal) + "Parent PID".Length;
            var rows = lines.Skip(2).Select(line => Regex.Match(line, @"^\s*(\d+)  +(\d+)  +(.+)$")).ToArray();

            Assert.All(rows, row =>
            {
                Assert.True(row.Success);
                Assert.Equal(pidEnd, row.Groups[1].Index + row.Groups[1].Length);
                Assert.Equal(parentEnd, row.Groups[2].Index + row.Groups[2].Length);
                Assert.Equal(imageColumn, row.Groups[3].Index);
            });

            Assert.Single(rows, row => row.Groups[1].Value == child.Id.ToString());
            Assert.Single(rows, row => row.Groups[1].Value == Runtime.ProcessId.ToString());

            foreach (string invalid in new[] { "0", "4", "-1", "4294967296", Runtime.ProcessId.ToString() })
            {
                await Runtime.CommandAsync($"process.terminate {invalid}", expectError: true);
            }

            await Runtime.CommandAsync($"process.terminate {child.Id}");

            Assert.True(child.WaitForExit(5000));
            Assert.StartsWith("Seed ", await Runtime.CommandAsync("version"));
        }
        finally
        {
            if (!child.HasExited)
            {
                child.Kill();
                await child.WaitForExitAsync(TestContext.Current.CancellationToken);
            }
        }
    }

    [Fact]
    public async Task ConsolePreservesSpacesAndQuotes()
    {
        await Runtime.DisconnectAsync();

        string runtime = Path.GetFullPath(Environment.GetEnvironmentVariable("SEED_E2E_RUNTIME")!);
        string console = Path.Combine(Path.GetDirectoryName(Path.GetDirectoryName(runtime)!)!, "Seed.Console", "Seed.Console.exe");
        string path = Path.Combine(Runtime.Directory, "console file 🌱.txt");

        const string text = "A \"quoted\" console value 🌱";

        var start = new ProcessStartInfo(console)
        {
            UseShellExecute = false,
            CreateNoWindow = true,
            RedirectStandardOutput = true,
            RedirectStandardError = true
        };

        foreach (string argument in new[] { "fs.write", path, text })
        {
            start.ArgumentList.Add(argument);
        }

        using var process = Process.Start(start)!;
        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(TestContext.Current.CancellationToken);

        deadline.CancelAfter(TimeSpan.FromSeconds(15));

        try
        {
#pragma warning disable xUnit1051 // The owned-process deadline is linked to the test token.
            Task<string> output = process.StandardOutput.ReadToEndAsync(deadline.Token);
            Task<string> errors = process.StandardError.ReadToEndAsync(deadline.Token);

            await process.WaitForExitAsync(deadline.Token);
#pragma warning restore xUnit1051
            Assert.Equal(0, process.ExitCode);
            Assert.Empty(await errors);
            Assert.Contains("written", await output);
            Assert.Equal(text, await File.ReadAllTextAsync(path, TestContext.Current.CancellationToken));
        }
        finally
        {
            if (!process.HasExited)
            {
                process.Kill();

                await process.WaitForExitAsync(CancellationToken.None);
            }

            await Runtime.ReconnectAsync();
        }
    }
}
