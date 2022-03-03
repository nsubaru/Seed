using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class BoxClassSerializator : DynamicSizeTypes
    {
        public object DeserializeLazyImpl<T>(
            Stream stream, 
            TypeMetaDataRecord thisTypeSerializationTypeRecord, 
            List<SerializationTypeRecord> serializationTypeRecords, 
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            T value = DeserializeItem<T>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
            Lazy<T>data = new Lazy<T>(() => value); 
            return data;
        }

        public void SerializeLazyImpl<T>(
            Lazy<T> data,
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            SerializeItem(data.Value, stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
        }

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {

            data = this
               .GetType()
               .GetMethod("DeserializeLazyImpl")
               .MakeGenericMethod(serializationTypeRecords
                       .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                       .ClrType)
               .Invoke(this, new object[] { stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            this
                .GetType()
                .GetMethod("SerializeLazyImpl")
                .MakeGenericMethod(serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType)
                .Invoke(this, new object[] { data, stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var subType = TryResolveSubTypeByTypeMetaDataRecord(thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);

            if (subType == null)
                return null;

            return typeof(Lazy<>).MakeGenericType(subType);
        }
    }
}
