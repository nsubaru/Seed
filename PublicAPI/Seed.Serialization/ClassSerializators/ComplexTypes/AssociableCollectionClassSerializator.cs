using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class AssociableCollectionClassSerializator : DynamicSizeTypes
    {
        #region Serialization implementation
        public void SerializeDictionaryImpl<T, U>(
            Dictionary<T, U> arrayList,
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            WriteItemsCount((ulong)arrayList.Count, stream, serializationTypeRecords, typeSerelizators);

            foreach (var item in arrayList)
            {
                SerializeItem(item, stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
            }
        }
        #endregion

        #region Deserialization implementation
        public object DeserializeDictionaryImpl<T, U>(
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var count = GetItemsCount(stream, serializationTypeRecords, typeSerelizators);

            var items = new Dictionary<T, U>();

            for (UInt64 i = 0; i < count; i++)
            {
                var item = DeserializeItem<KeyValuePair<T, U>>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
                items.Add(item.Key, item.Value);
            }

            return items;
        }
        #endregion

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var pairShema = thisTypeSerializationTypeRecord.NestedTypes[0];

            var keyShema = pairShema.NestedTypes[0];
            var valueShema = pairShema.NestedTypes[1];

            var keyType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == keyShema.TypeName)
                        .ClrType;

            var valueType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == valueShema.TypeName)
                        .ClrType;

            data = this
                .GetType()
                .GetMethod("DeserializeDictionaryImpl")
                .MakeGenericMethod(new Type[] { keyType, valueType })
                .Invoke(this, new object[] { stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var pairShema = thisTypeSerializationTypeRecord.NestedTypes[0];

            var keyShema = pairShema.NestedTypes[0];
            var valueShema = pairShema.NestedTypes[1];

            var keyType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == keyShema.TypeName)
                        .ClrType;

            var valueType = serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == valueShema.TypeName)
                        .ClrType;

            this
                .GetType()
                .GetMethod("SerializeDictionaryImpl")
                .MakeGenericMethod(new Type[] { keyType, valueType })
                .Invoke(this, new object[] { data, stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var subType = TryResolveSubTypeByTypeMetaDataRecord(thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);

            var pairTypes = subType.GetGenericArguments();

            return typeof(Dictionary<,>).MakeGenericType(pairTypes);
        }
    }
}
