using Seed.CTTI;
using Seed.CTTI.MetaData;
using System;
using System.Collections.Generic;
using System.IO;

namespace Seed.CTTI.ClassSerializators;

interface ITypeSerializator
{
    void Serialize(Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);
    void Deserialize(ref Object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators);

    Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerializators);
}
