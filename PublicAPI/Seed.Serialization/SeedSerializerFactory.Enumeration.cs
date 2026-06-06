using Seed.CTTI;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetEnumUnderlyingType(RTTI.Record.RttiTypeRecord record, RTTI.RTTISection section)
    {
        var serializableTypes = AppDomain.CurrentDomain
            .GetAssemblies()
            .SelectMany(x => x.GetTypes()
                              .Where(x => Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) != null
                        ))
            .Select(x => new { Type = x, Attribute = (Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) as SeedSerializableAttribute) })
            .ToList();

        var type = serializableTypes
            .FirstOrDefault(x => x.Attribute!.SeedKernelTypeName == record.TypeName)!
            ?.Type;

        if (type == null)
        {
            return null;
        }
        return type;
    }

    private void AddEnumSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("this.SerializeImpl(value, stream);");
    }

    private void AddEnumDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("value = this.DeserializeImpl(value, stream);");
        code.AppendLine($"data = ({GetFriendlyTypeName(typeof(T))}) value;");
    }
}
