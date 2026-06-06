using Microsoft.Extensions.Logging;
using Seed.CTTI;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetCustomSerializerType(RTTI.Record.RttiTypeRecord record)
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
        code.Append($"/*Type: {typeRecord.TypeName}*/");
        code.Append($"ulong typeId = {typeRecord.RttiType.FullTypeId.TypeId};");
        code.Append($"var schema = customTypeMappingSchema[typeId];");
        code.Append("schema.Serialize(data, stream, this);");
    }

    private void AddCustomTypeDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.Append($"/*Type: {typeRecord.TypeName}*/");
        code.Append($"ulong typeId = {typeRecord.RttiType.FullTypeId.TypeId};");
        code.Append($"var schema = customTypeMappingSchema[typeId];");
        code.Append($"data = ({GetFriendlyTypeName(typeof(T))}) schema.Deserialize(stream, this);");
    }
}
