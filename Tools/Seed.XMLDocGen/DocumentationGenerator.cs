using System.Text;
using System.Text.Json;
using System.Text.RegularExpressions;
using System.Xml.Linq;

namespace Seed.XMLDocGen;

internal static class DocumentationGenerator
{
    internal static async Task GenerateAsync(string root, string output)
    {
        if (!File.Exists(Path.Combine(root, "seed-components.json")))
        {
            throw new ArgumentException("Source root must contain seed-components.json.");
        }

        if (!Path.GetFullPath(output).StartsWith(root + Path.DirectorySeparatorChar, StringComparison.OrdinalIgnoreCase))
        {
            throw new ArgumentException("Generated documentation must be inside the Seed checkout.");
        }

        Directory.CreateDirectory(output);

        var components = JsonSerializer.Deserialize<Component[]>(await File.ReadAllTextAsync(Path.Combine(root, "seed-components.json")))!;
        var context    = new PageContext(root, output, components);
        var pages      = typeof(DocumentationGenerator).Assembly.GetTypes().Where(type => typeof(ISourcePage).IsAssignableFrom(type) && !type.IsAbstract && !type.IsInterface).Select(type => (ISourcePage)Activator.CreateInstance(type, nonPublic: true)!).OrderBy(page => page.FileName).ToArray();
        var index      = new StringBuilder("# Seed documentation\n\nDocumentation for the source components shipped in this checkout.\n\n");

        foreach (var page in pages)
        {
            await File.WriteAllTextAsync(Path.Combine(output, page.FileName), page.Render(context));
            index.AppendLine($"- [{page.Title}]({page.FileName})");
        }

        index.AppendLine("- [API reference](api/index.md)");

        await File.WriteAllTextAsync(Path.Combine(output, "index.md"), index.ToString());
        await GenerateApiAsync(context);
        
        Console.WriteLine($"Generated {pages.Length} source pages and API reference in {output}.");
    }

    private static async Task GenerateApiAsync(PageContext context)
    {
        var directory = Path.Combine(context.Output, "api");
        
        Directory.CreateDirectory(directory);

        var index    = new StringBuilder("# Seed API reference\n\nGenerated from compiler XML documentation for the shipped projects.\n\n");
        var root     = Path.Combine(context.Root, "Artifacts");
        var xmlFiles = Directory.Exists(root) ? Directory.EnumerateFiles(root, "Seed*.xml", SearchOption.AllDirectories).Where(path => !path.Contains("Intermediate", StringComparison.OrdinalIgnoreCase)) : [];
        var members  = new Dictionary<string, XElement>();

        foreach (var path in xmlFiles.Order())
        {
            var document = XDocument.Load(path);

            foreach (var member in document.Descendants("member"))
            {
                var name = (string?)member.Attribute("name");

                if (name is null || !name.Contains("Seed") || name.Contains("Seed.Core.Testing.Test.") || name.Contains("Seed::Core::Testing::Test::"))
                {
                    continue;
                }

                members[name] = member;
            }
        }

        foreach (var group in members.OrderBy(pair => pair.Key).GroupBy(pair => Module(pair.Key)))
        {
            var file = group.Key + ".md";

            index.AppendLine($"- [{group.Key}]({file}) ({group.Count()} documented members)");

            var text = new StringBuilder($"# {group.Key} API\n\n");

            foreach (var (name, member) in group)
            {
                string display;

                try
                {
                    display = XdcParser.ParseName(name);
                }
                catch (Exception)
                {
                    display = name;
                }

                text.AppendLine($"## `{display.Replace("`", "'")}`\n");
                text.AppendLine(Clean(member.Element("summary")?.Value) + "\n");

                foreach (var parameter in member.Elements("param"))
                {
                    text.AppendLine($"- `{(string?)parameter.Attribute("name")}`: {Clean(parameter.Value)}");
                }

                if (member.Element("returns") is {} result)
                {
                    text.AppendLine($"\nReturns: {Clean(result.Value)}\n");
                }

                if (member.Element("remarks") is {} remarks)
                {
                    text.AppendLine($"\n{Clean(remarks.Value)}\n");
                }

                text.AppendLine();
            }

            await File.WriteAllTextAsync(Path.Combine(directory, file), text.ToString());
        }

        if (members.Count == 0)
        {
            index.AppendLine("Build the native and managed projects to supply compiler XML documentation.");
        }

        await File.WriteAllTextAsync(Path.Combine(directory, "index.md"), index.ToString());
        
        Console.WriteLine($"API reference: {members.Count} documented members.");
    }

    private static string Clean(string? text) => Regex.Replace(text ?? "", @"\s+", " ").Trim();

    private static string Module(string name)
    {
        var identifier = name[2..].Replace("::", ".");

        return string.Join('.', identifier.Split('.').Take(identifier.StartsWith("Seed.Core.") ? 3 : 2));
    }
}

internal sealed record Component(string Name, string Summary, string Directory, string Pattern);
internal sealed record PageContext(string Root, string Output, Component[] Components)
{
    internal IEnumerable<string> Sources(Component component)
        => System.IO.Directory.EnumerateFiles(Path.Combine(Root, component.Directory), component.Pattern, SearchOption.AllDirectories)
            .Where(path => !path.Contains(Path.DirectorySeparatorChar + "obj" + Path.DirectorySeparatorChar) && !path.Contains(Path.DirectorySeparatorChar + "bin" + Path.DirectorySeparatorChar))
            .Select(path => Path.GetRelativePath(Root, path)
            .Replace('\\', '/'))
            .Order();
}

internal interface ISourcePage
{
    string FileName { get; }

    string Title { get; }

    string Render(PageContext context);
}
