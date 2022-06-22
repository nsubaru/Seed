using Seed.CTTI;
using Seed.CTTI.MetaData;
using System.Text;

namespace Seed.CTTI.ClassSerializators;

class StringClassSerialization : DynamicSizeTypes
{
    public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        switch (thisTypeSerializationTypeRecord.TypeName)
        {
            case "Seed::Core::String":
            case "Seed::Core::SencitiveString":
                {
                    var strSize = GetItemsCount(stream, serializationTypeRecords, typeSerializators) - 1;

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

    public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        switch (thisTypeSerializationTypeRecord.TypeName)
        {
            case "Seed::Core::String":
            case "Seed::Core::SencitiveString":
                {
                    var str = (string)data;
                    var bytes = Encoding.Unicode.GetBytes(str);
                    WriteItemsCount((ulong)str.Length + 1, stream, serializationTypeRecords, typeSerializators);
                    stream.Write(bytes, 0, bytes.Length);
                    stream.Write(Encoding.Unicode.GetBytes(new char[]{'\0' }), 0, 1 * sizeof(char));
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type '{thisTypeSerializationTypeRecord.TypeName}' and type class '{thisTypeSerializationTypeRecord.Type}' not implemented");
        }
    }

    public override Type TryResolveTypeByTypeMetaDataRecord(TypeMetaDataRecord thisTypeSerializationTypeRecord, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        return typeof(string);
    }
}
