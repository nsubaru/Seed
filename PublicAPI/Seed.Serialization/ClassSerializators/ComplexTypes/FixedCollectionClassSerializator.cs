using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

namespace Seed.Serialization.ClassSerializators.ComplexTypes
{
    class FixedCollectionClassSerializator : DynamicSizeTypes
    {
        #region Serialization implementation
        public void SerializeArrayList<T>(
            List<T> arrayList,
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            ulong ItemsCount = ulong.Parse(thisTypeSerializationTypeRecord.AdditionalInfo);
            WriteItemsCount(ItemsCount, stream, serializationTypeRecords, typeSerelizators);

            int writeCount = arrayList.Count > (int)ItemsCount ? (int)ItemsCount : arrayList.Count;

            for (int i = 0; i < writeCount; i++)
            {
                SerializeItem(arrayList[i], stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
            }

            if((ulong)arrayList.Count < ItemsCount)
            {
                var defaultValueItemCount = ItemsCount - (ulong)arrayList.Count;

                for (ulong i = 0; i < defaultValueItemCount; i++)
                {
                    SerializeItem(default(T), stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);
                }
            }
        }
        #endregion

        #region Deserialization implementation
        public object DeserializeArrayList<T>(
            Stream stream,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var count = GetItemsCount(stream, serializationTypeRecords, typeSerelizators);

            List<T> items = new List<T>();

            for (UInt64 i = 0; i < count; i++)
            {
                items.Add(DeserializeItem<T>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators));
            }

            return items;
        }
        #endregion

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            data = this
                .GetType()
                .GetMethod("DeserializeArrayList")
                .MakeGenericMethod(serializationTypeRecords
                        .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                        .ClrType)
                .Invoke(this, new object[] { stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators });
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            this
                .GetType()
                .GetMethod("SerializeArrayList")
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

            return typeof(List<>).MakeGenericType(subType);
        }

    }
}
