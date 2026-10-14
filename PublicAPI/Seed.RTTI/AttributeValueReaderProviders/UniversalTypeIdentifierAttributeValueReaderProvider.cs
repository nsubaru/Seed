using Seed.Core.RTTI;

namespace Seed.RTTI.AttributeValueReaderProviders;

public class UniversalTypeIdentifierAttributeValueReaderProvider : IAttributeValueProvider<UniversalTypeIdentifier>
{
    public UniversalTypeIdentifier GetAttributeValue(IntPtr rawData) => new UniversalTypeIdentifier()
    {
        Id = Marshal.PtrToStructure<UUID>(rawData)
    };
}
