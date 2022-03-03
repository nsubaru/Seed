using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace Seed.Serialization.ClassSerializators
{
    class StringClassSerialization : DynamicSizeTypes
    {
        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            switch (thisTypeSerializationTypeRecord.TypeName)
            {
                case "Seed::String":
                case "Seed::SencitiveString":
                    {
                        var strSize = GetItemsCount(stream, serializationTypeRecords, typeSerelizators) - 1;

                        byte[] buffer = new byte[sizeof(char) * strSize];
                        stream.Read(buffer, 0, sizeof(char) * (int)strSize);
                        data = Encoding.Unicode.GetString(buffer);
                        stream.Seek(2, SeekOrigin.Current);
                    }
                    break;
                default:
                    throw new NotImplementedException($"Deserialization of type '{thisTypeSerializationTypeRecord.TypeName}' and type class '{thisTypeSerializationTypeRecord.Type}' not implemented");
            }
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            switch (thisTypeSerializationTypeRecord.TypeName)
            {
                case "Seed::String":
                case "Seed::SencitiveString":
                    {
                        var str = (string)data;
                        WriteItemsCount((ulong)str.Length + 1, stream, serializationTypeRecords, typeSerelizators);
                        stream.Write(Encoding.Unicode.GetBytes(str), 0, str.Length * sizeof(char));
                        stream.Write(Encoding.Unicode.GetBytes(new char[]{'\0' }), 0, 1 * sizeof(char));
                    }
                    break;
                default:
                    throw new NotImplementedException($"Serialization of type '{thisTypeSerializationTypeRecord.TypeName}' and type class '{thisTypeSerializationTypeRecord.Type}' not implemented");
            }
        }

        public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            return typeof(string);
        }
    }
}
