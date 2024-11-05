using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.RTTI.AttributeValueReaderProviders;

public class UniversalTypeIdentifierAttributeValueReaderProvider : IAttributeValueProvider<UniversalTypeIdentifier>
{
    public UniversalTypeIdentifier GetAttributeValue(IntPtr rawData) => new UniversalTypeIdentifier()
    {
        Id = Marshal.PtrToStructure<UUID>(rawData)
    };
}
