using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddFixedCollectionSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        AddCollectionSer<T>(ref code, typeRecord);
    }

    private void AddFixedCollectionDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        AddCollectionDer<T>(ref code, typeRecord);
    }

    private void AddFixedArraySer<T>(ref StringBuilder code, TypeRecord _)
    {
        code.AppendLine($"foreach (var item in data){{");
        code.AppendLine($"SerializeImpl(item, stream);}}");
    }

    private void AddFixedArrayDer<T>(ref StringBuilder code, TypeRecord _)
    {
        code.AppendLine($"for (int i = 0; i < data.Length; i++){{");
        code.AppendLine($"data[i] = DeserializeImpl(data[i], stream);}}");
    }
}
