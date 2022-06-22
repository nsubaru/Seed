using Seed.CTTI;
using Seed.CTTI.MetaData;

namespace Seed.CTTI.ClassSerializators.ComplexTypes;

class MetaDataClassSerialization : DynamicSizeTypes
{
    public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        TypeMetaDataRecord serializationTextType = null;
        if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord")
            serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::Core::String").SeedKernelTypeShema;
        else if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
            serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::Core::String").SeedKernelTypeShema;
        else
            throw new NotImplementedException();

        var testMetaData = DeserializeItem<String>(stream, serializationTypeRecords, serializationTextType, typeSerializators);

        data = new TypeMetaDataDecoder().DecodeTypeMetaData(testMetaData);
    }

    public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        TypeMetaDataRecord serializationTextType = null;
        if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord")
            serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::Core::String").SeedKernelTypeShema;
        else if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
            serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::Core::SensitiveString").SeedKernelTypeShema;
        else
            throw new NotImplementedException();

        string serializationText = ((TypeMetaDataRecord)data).ToString("S");

        SerializeItem(serializationText, stream, serializationTypeRecords, serializationTextType, typeSerializators);
    }

    public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        if(thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord" ||
           thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
        {
            return typeof(TypeMetaDataRecord);
        }

        return null;
    }
}
