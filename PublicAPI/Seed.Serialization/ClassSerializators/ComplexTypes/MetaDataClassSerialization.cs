using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class MetaDataClassSerialization : DynamicSizeTypes
    {
        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            TypeMetaDataRecord serializationTextType = null;
            if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord")
                serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::String").SeedKernelTypeShema;
            else if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
                serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::String").SeedKernelTypeShema;
            else
                throw new NotImplementedException();

            var testMetaData = DeserializeItem<String>(stream, serializationTypeRecords, serializationTextType, typeSerelizators);

            data = new TypeMetaDataDecoder().DecodeTypeMetaData(testMetaData);
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            TypeMetaDataRecord serializationTextType = null;
            if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord")
                serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::String").SeedKernelTypeShema;
            else if (thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
                serializationTextType = serializationTypeRecords.First(type => type.SeedKernelTypeName == "Seed::SensitiveString").SeedKernelTypeShema;
            else
                throw new NotImplementedException();

            string serializationText = ((TypeMetaDataRecord)data).ToString("S");

            SerializeItem(serializationText, stream, serializationTypeRecords, serializationTextType, typeSerelizators);
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            if(thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord" ||
               thisTypeSerializationTypeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
            {
                return typeof(TypeMetaDataRecord);
            }

            return null;
        }
    }
}
