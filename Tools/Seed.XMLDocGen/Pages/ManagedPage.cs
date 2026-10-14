namespace Seed.XMLDocGen;

internal sealed class ManagedPage : ISourcePage
{
    public string FileName => "managed.md";

    public string Title => "Managed reflection and serialization";

    public string Render(PageContext context) => """
        # Managed reflection and serialization

        Seed.Shared and Seed.CTTI provide foundational metadata contracts. Seed.Objects
        contains selected Core records and the DynamicObject envelope. Seed.RTTI reads
        native reflection metadata. Seed.Serialization builds binary serializers from
        metadata and mapped managed types.

        Foundational assemblies target netstandard2.0. Serialization and developer tools
        use .NET 10. Package dependencies are defined in the corresponding projects;
        license and provenance validation is part of release preparation.

        A runtime RTTI export must come from the live fixed-up Seed metadata section.
        Native-to-managed round-trip validation and regenerated Seed fixtures are pending;
        successful managed compilation alone does not validate the metadata ABI.

        See [API reference](api/index.md) for compiler-generated managed documentation.
        """;
}
