using Seed.CTTI;
using Seed.CTTI.MetaData;

namespace Seed.CTTI.ClassSerializators;

abstract class SimpeTypeSerializator : ITypeSerializator
{
    public Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerializators)
        => GetSupportedTypes().FirstOrDefault(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.TypeName).ClrType;

    public abstract List<SerializationTypeRecord> GetSupportedTypes();
    public abstract void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);
    public abstract void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);
}
