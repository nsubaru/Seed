using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddPairSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var key = data.Key;");
        code.AppendLine($"var value = data.Value;");
        code.AppendLine($"SerializeImpl(key, stream);");
        code.AppendLine($"SerializeImpl(value, stream);");
    }

    private void AddPairDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var key = data.Key;");
        code.AppendLine($"key = DeserializeImpl(key, stream);");
        code.AppendLine($"var value = data.Value;");
        code.AppendLine($"value = DeserializeImpl(value, stream);");
        code.AppendLine($"data = new {GetFriendlyTypeName(typeof(T))}(key, value);");
    }
}
