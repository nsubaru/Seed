using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class PairClassSerializator : DynamicSizeTypes
    {
        public void SerilizeImpl<T, U>(KeyValuePair<T, U> pair, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            SerializeItem<T>(pair.Key, stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
            SerializeItem<U>(pair.Value, stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[1], typeSerelizators);
        }

        public object DeserilizeImpl<T, U>(Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            return new KeyValuePair<T, U>
            (
                DeserializeItem<T>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators),
                DeserializeItem<U>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[1], typeSerelizators)
            );
            
        }

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var keyType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType;

            var valueType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType;

            data = this
                .GetType()
                .GetMethod("DeserilizeImpl")
                .MakeGenericMethod(new Type[] { keyType, valueType })
                .Invoke(this, new object[] { stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var keyType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType;

            var valueType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType;

            this
                .GetType()
                .GetMethod("SerilizeImpl")
                .MakeGenericMethod(new Type[] { keyType, valueType })
                .Invoke(this, new object[] { data, stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var keyTypeShema = thisTypeSerializationTypeRecord.NestedTypes[0];
            var valueTypeShema = thisTypeSerializationTypeRecord.NestedTypes[1];

            var keyType = TryResolveSubTypeByTypeMetaDataRecord(keyTypeShema, typeSerelizators);
            var valueType = TryResolveSubTypeByTypeMetaDataRecord(valueTypeShema, typeSerelizators);

            if (keyType == null || valueType == null)
                return null;

            return typeof(KeyValuePair<,>).MakeGenericType(new Type[]{ keyType, valueType});
        }
    }
}
