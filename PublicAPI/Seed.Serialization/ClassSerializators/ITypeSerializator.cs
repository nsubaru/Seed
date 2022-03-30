using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Seed.Serialization.ClassSerializators
{
    interface ITypeSerializator
    {
        void Serialize(Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);
        void Deserialize(ref Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);

        Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerializators);
    }
}
