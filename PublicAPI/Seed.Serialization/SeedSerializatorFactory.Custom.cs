using Seed.CTTI;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? GetCustomSerializatorType(RTTI.Record.RttiTypeRecord record)
    {
        return AppDomain.CurrentDomain
            .GetAssemblies()
            .SelectMany(x => x.GetTypes()
                              .Where(x => Attribute.GetCustomAttribute(x, typeof(SerializableStructAttribute)) != null
                        ))
            .FirstOrDefault(x => x.FullName == record.TypeName);
    }

    private void AddCustomTypeSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.Append($"var schema = customTypeMappingSchema.First(x => x.Key.TypeName == \"{typeRecord.TypeName}\");");
        code.Append("schema.Value.Serialize(data, stream, this);");
    }

    private void AddCustomTypeDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.Append($"var schema = customTypeMappingSchema.First(x => x.Key.TypeName == \"{typeRecord.TypeName}\");");
        code.Append($"return ({GetFriendlyTypeName(typeof(T))}) schema.Value.Deserialize(stream, this);");
    }
}
