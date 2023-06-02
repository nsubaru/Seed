using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddAssociableCollectionSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"SerializeImpl((ulong)data.Count, stream);");

        code.AppendLine($"foreach (var item in data){{");
        code.AppendLine($"SerializeImpl(item, stream);}}");
    }

    private void AddAssociableCollectionDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"UInt64 count = 0;");
        code.AppendLine($"count = DeserializeImpl(count, stream);");

        code.AppendLine($"data = new {GetFriendlyTypeName(typeof(T))}();");

        code.AppendLine($"for (UInt64 i = 0; i < count; i++){{");
        code.AppendLine($"var item = new KeyValuePair<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}, {GetFriendlyTypeName(typeof(T).GetGenericArguments().Skip(1).First())} >();");
        code.AppendLine($"item = DeserializeImpl(item, stream);");
        code.AppendLine($"data.Add(item.Key, item.Value);}}");
    }
}
