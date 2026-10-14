namespace Seed.XMLDocGen;

internal sealed class RuntimePatternsPage : ISourcePage
{
    public string FileName => "runtime-patterns.md";

    public string Title => "Refcount, exception and service patterns";

    public string Render(PageContext context) => File.ReadAllText(Path.Combine(context.Root, ".github", "copilot-runtime-patterns.md"));
}
