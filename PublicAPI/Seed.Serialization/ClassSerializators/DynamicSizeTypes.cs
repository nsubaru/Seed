using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Seed.Serialization.ClassSerializators
{
    abstract class DynamicSizeTypes : ITypeSerializator
    {
        public abstract void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serelizationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators);
        public abstract void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serelizationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators);
        public abstract Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators);

        #region Serialization implementation
        public void WriteItemsCount(
            UInt64 count,
            Stream stream, 
            List<SerializationTypeRecord> serelizationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            typeSerelizators[ETypes.Numeric].Serialize(
                count,
                stream, serelizationTypeRecords.First(item => item.SeedKernelTypeName == "Seed::UInt64").SeedKernelTypeShema,
                serelizationTypeRecords,
                typeSerelizators);
        }

        public void SerializeItem<T>(
            T item,
            Stream stream, 
            List<SerializationTypeRecord> serelizationTypeRecords,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {

            typeSerelizators[thisTypeSerializationTypeRecord.Type]
                .Serialize(item, stream, thisTypeSerializationTypeRecord, serelizationTypeRecords, typeSerelizators);
        }

        #endregion

        #region Deserilization implementation
        public UInt64 GetItemsCount(
            Stream stream, 
            List<SerializationTypeRecord> serelizationTypeRecords,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            object data = new object();
            data = (UInt64)0;

            typeSerelizators[ETypes.Numeric].Deserialize(
                ref data,
                stream, serelizationTypeRecords.First(item => item.SeedKernelTypeName == "Seed::UInt64").SeedKernelTypeShema,
                serelizationTypeRecords,
                typeSerelizators);

            return (UInt64)data;
        }

        public T DeserializeItem<T>(
            Stream stream, 
            List<SerializationTypeRecord> serelizationTypeRecords,
            TypeMetaDataRecord thisTypeSerializationTypeRecord,
            Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            T var = default;
            object data = var;

            typeSerelizators[thisTypeSerializationTypeRecord.Type]
                .Deserialize(ref data, stream, thisTypeSerializationTypeRecord, serelizationTypeRecords, typeSerelizators);

            return (T)data;
        }
        #endregion

        #region TypeResolver

        public Type TryResolveSubTypeByTypeMetaDataRecord(TypeMetaDataRecord subTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            return typeSerelizators[subTypeSerializationTypeRecord.Type]
                .TryResolveTypeByTypeMetaDataRecord(subTypeSerializationTypeRecord, typeSerelizators);
        }

        #endregion
    }
}
