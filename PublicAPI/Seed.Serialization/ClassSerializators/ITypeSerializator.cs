using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Seed.Serialization.ClassSerializators
{
    interface ITypeSerializator
    {
        void Serialize(Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators);
        void Deserialize(ref Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators);

        Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators);
    }
}
