namespace Seed.XMLDocGen;

internal sealed class CodeStylePage : ISourcePage
{
    public string FileName => "code-style.md";

    public string Title => "Code style and agent guidance";

    public string Render(PageContext context) => File.ReadAllText(Path.Combine(context.Root, ".github", "code-convention.md")) + "\n\n" + """
        ## Companion guidance

        The conventions above are generated from the same source that AGENTS.md
        directs agents to read. Edit that source to keep public and agent guidance aligned.

        - [Root contributor and agent instructions](../AGENTS.md)
        - [Copilot instructions](../.github/copilot-instructions.md)
        - [Source context](../.github/copilot-context.md)
        - [Canonical code conventions](../.github/code-convention.md)
        - [Runtime patterns](../.github/copilot-runtime-patterns.md)
        - [Collection semantics](../.github/copilot-collections.md)
        - [Async and interruption](../.github/copilot-async-model.md)
        - [Anti-patterns](../.github/copilot-anti-patterns.md)
        """;
}
