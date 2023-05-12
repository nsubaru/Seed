using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? GetBoxType(RTTI.Record.RttiTypeRecord record)
    {
        var innerType = GetTypeWithOffset(0, record);

        if (innerType == null)
            return null;

        return typeof(Lazy<>).MakeGenericType(innerType);
    }

    private void AddBoxSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"SerializeImpl(data.Value, stream);");
    }

    private void AddBoxDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())} value = data.Value;");
        code.AppendLine($"value = DeserializeImpl(value, stream);");
        code.AppendLine($"data = new {GetFriendlyTypeName(typeof(T))}(() => value);");
    }
}
