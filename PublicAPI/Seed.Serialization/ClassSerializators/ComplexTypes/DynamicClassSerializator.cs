using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.Dynamic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class DynamicClassSerializator : DynamicSizeTypes
    {
        public object DeserializeDynamic<T>(
           Stream stream,
           TypeMetaDataRecord thisTypeSerializationTypeRecord,
           List<SerializationTypeRecord> serializationTypeRecords,
           Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            T value = DeserializeItem<T>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord, typeSerelizators);
            return value;
        }

        public void SerializeDynamic<T>(
            T data,
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            SerializeItem(data, stream, serializationTypeRecords, thisTypeSerializationTypeRecord, typeSerelizators);
        }

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            object obj = "";
            typeSerelizators[ETypes.String].Deserialize(
                ref obj,
                stream,
                serializationTypeRecords.Where(record => record.SeedKernelTypeName == "Seed::String").First().SeedKernelTypeShema,
                serializationTypeRecords,
                typeSerelizators);

            string typeName = obj as string;

            var typeRecord = serializationTypeRecords.Where(record => record.SeedKernelTypeName == typeName).First();

            data = new DynamicObject(this
               .GetType()
               .GetMethod("DeserializeDynamic")
               .MakeGenericMethod(typeRecord.ClrType)
               .Invoke(this, new object[] { stream, typeRecord.SeedKernelTypeShema, serializationTypeRecords, typeSerelizators }));

        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var obj = (data as DynamicObject).Value;

            var typeRecord = serializationTypeRecords
                .Where(record => record.ClrType == obj.GetType())
                .First();

            typeSerelizators[ETypes.String].Serialize(
                typeRecord.SeedKernelTypeName,
                stream,
                serializationTypeRecords.Where(record => record.SeedKernelTypeName == "Seed::String").First().SeedKernelTypeShema,
                serializationTypeRecords,
                typeSerelizators);

            this
                .GetType()
                .GetMethod("SerializeDynamic")
                .MakeGenericMethod(typeRecord.ClrType)
                .Invoke(this, new object[] { obj, stream, typeRecord.SeedKernelTypeShema, serializationTypeRecords, typeSerelizators });
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            if (thisTypeSerializationTypeRecord.TypeName == "Seed::Containers::Dynamic" ||
                thisTypeSerializationTypeRecord.TypeName == "Seed::Reflection::DynamicObject")
            {
                return typeof(DynamicObject);
            }
            else
            {
                return null;
            }
        }
    }
}
