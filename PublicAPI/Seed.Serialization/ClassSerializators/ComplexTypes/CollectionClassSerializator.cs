using Seed.Serialization.MetaData;
using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators
{
    class CollectionClassSerializator : DynamicSizeTypes
    {
        #region Serialization implementation
        public void SerializeArrayList<T>(
            List<T> arrayList,
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

        public void SerializeLinkedList<T>(
            LinkedList<T> arrayList,
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

        public object DeserializeLinkedList<T>(
            Stream stream, 
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            List<SerializationTypeRecord> serializationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            var count = GetItemsCount(stream, serializationTypeRecords, typeSerelizators);
            
            LinkedList<T> items = new LinkedList<T>();

            for (UInt64 i = 0; i < count; i++)
            {
                items.AddLast(DeserializeItem<T>(stream, serializationTypeRecords, thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators));
            }

            return items;
        }
        #endregion

        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            if (thisTypeSerializationTypeRecord.TypeName.StartsWith("Seed::Containers::List"))
            {
                data = this
                    .GetType()
                    .GetMethod("DeserializeLinkedList")
                    .MakeGenericMethod(serializationTypeRecords
                            .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                            .ClrType)
                    .Invoke(this, new object[]{stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators});
            }
            else
            {
                data = this
                    .GetType()
                    .GetMethod("DeserializeArrayList")
                    .MakeGenericMethod(serializationTypeRecords
                            .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                            .ClrType)
                    .Invoke(this, new object[]{stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators});
            }
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
           if (thisTypeSerializationTypeRecord.TypeName.StartsWith("Seed::Containers::List"))
           {
                this
                    .GetType()
                    .GetMethod("SerializeLinkedList")
                    .MakeGenericMethod(serializationTypeRecords
                            .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                            .ClrType)
                    .Invoke(this, new object[]{data, stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators});
            }
            else
            {
               this
                    .GetType()
                    .GetMethod("SerializeArrayList")
                    .MakeGenericMethod(serializationTypeRecords
                            .First(item => item.SeedKernelTypeName == thisTypeSerializationTypeRecord.NestedTypes[0].TypeName)
                            .ClrType)
                    .Invoke(this, new object[]{data, stream, thisTypeSerializationTypeRecord, serializationTypeRecords, typeSerelizators});
            }
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            if (thisTypeSerializationTypeRecord.TypeName.StartsWith("Seed::Containers::List"))
            {
                var subType = TryResolveSubTypeByTypeMetaDataRecord(thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);

                if(subType == null)
                    return null;

                return typeof(LinkedList<>).MakeGenericType(subType);
            }
            else
            {
                var subType = TryResolveSubTypeByTypeMetaDataRecord(thisTypeSerializationTypeRecord.NestedTypes[0], typeSerelizators);

                if(subType == null)
                    return null;

                return typeof(List<>).MakeGenericType(subType);
            }
        }
    }
}
