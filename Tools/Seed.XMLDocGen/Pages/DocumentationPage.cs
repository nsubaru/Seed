namespace Seed.XMLDocGen;

internal sealed class DocumentationPage : ISourcePage
{
    public string FileName => "documentation.md";

    public string Title => "Documentation generation";

    public string Render(PageContext context) => """
        # Documentation generation

        Run from the repository root:

        ```powershell
        dotnet run --project Tools/Seed.XMLDocGen -- . Documentation
        ```

        The generator reads seed-components.json, lists only the selected source
        components and loads Seed compiler XML under Artifacts. It writes Markdown into
        the ignored Documentation directory. Generated output is not source-controlled.

        Keep one page implementation per independent topic under Pages. Update the
        component manifest when the shipped source surface changes. Build native and
        managed components before generating the complete API reference.

        Contributor workflows include [KMS E2E authoring](e2e.md),
        [native test adapter/package updates](test-adapter.md),
        [ntdll import updates](ntdll.md), [WinDbg boot debugging](boot-debugging.md),
        [Windows Docker builds](docker.md) and [code conventions and agent guidance](code-style.md).
        [Runtime patterns](runtime-patterns.md) use the same canonical examples as
        the agent guide for refcounting, exceptions and IoC service registration.
        """;
}
