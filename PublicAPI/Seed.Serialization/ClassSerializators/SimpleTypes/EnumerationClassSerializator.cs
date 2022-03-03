using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Seed.Serialization.ClassSerializators.SimpleTypes
{
    class EnumerationClassSerializator : ITypeSerializator
    {
        public void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            Type type = data.GetType();

            if (type.IsEnum)
            {
                object underlyingData = Convert.ChangeType(data, Enum.GetUnderlyingType(type));
                typeSerelizators[ETypes.Numeric]
                    .Deserialize(ref underlyingData, stream, thisTypeSerializationTypeRecord.NestedTypes[0], serializationTypeRecords, typeSerelizators);
                data = Enum.ToObject(type, underlyingData);
            }
            else
            {
                typeSerelizators[ETypes.Numeric]
                    .Deserialize(ref data, stream, thisTypeSerializationTypeRecord.NestedTypes[0], serializationTypeRecords, typeSerelizators);
            }
        }

        public void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            Type type = data.GetType();

            if (type.IsEnum)
            {
                object underlyingData = Convert.ChangeType(data, Enum.GetUnderlyingType(type));
                typeSerelizators[ETypes.Numeric]
                    .Serialize(underlyingData, stream, thisTypeSerializationTypeRecord.NestedTypes[0], serializationTypeRecords, typeSerelizators);
            }
            else
            {
                typeSerelizators[ETypes.Numeric]
                    .Serialize(data, stream, thisTypeSerializationTypeRecord.NestedTypes[0], serializationTypeRecords, typeSerelizators);
            }
        }

        public Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators) 
            => typeSerelizators[ETypes.Numeric].TryResolveTypeByTypeMetaDataRecord(thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
    }
}
