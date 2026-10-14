using System.Text;

namespace Seed.XMLDocGen;

internal sealed class ComponentsPage : ISourcePage
{
    public string FileName => "components.md";

    public string Title => "Components and source map";

    public string Render(PageContext context)
    {
        var text = new StringBuilder("# Components and source map\n\n");
        foreach (var component in context.Components)
        {
            text.AppendLine($"## {component.Name}\n\n{component.Summary}\n");
            foreach (var source in context.Sources(component))
            {
                text.AppendLine($"- [{source}](../{Uri.EscapeDataString(source).Replace("%2F", "/")})");
            }

            text.AppendLine();
        }

        return text.ToString();
    }
}
