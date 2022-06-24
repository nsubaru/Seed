using Seed.CTTI;
using Seed.CTTI.MetaData;

namespace Seed.CTTI.ClassSerializators;

class CharacterClassSerializator : SimpeTypeSerializator
{
    public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        switch (thisTypeSerializationTypeRecord.TypeName)
        {
            case "Seed::AChar":
                {
                    byte[] buffer = new byte[sizeof(byte)];
                    stream.Read(buffer, 0, sizeof(byte));
                    data = buffer[0];
                }
                break;
            case "Seed::UChar":
                {
                    byte[] buffer = new byte[sizeof(char)];
                    stream.Read(buffer, 0, sizeof(char));
                    data = BitConverter.ToChar(buffer, 0);
                }
                break;
            case "Seed::U8Char":
                {
                    byte[] buffer = new byte[sizeof(byte)];
                    stream.Read(buffer, 0, sizeof(byte));
                    data = buffer[0];
                }
                break;
            case "Seed::U16Char":
                {
                    byte[] buffer = new byte[sizeof(char)];
                    stream.Read(buffer, 0, sizeof(char));
                    data = BitConverter.ToChar(buffer, 0);
                }
                break;
            case "Seed::U32Char":
                {
                    byte[] buffer = new byte[sizeof(Int32)];
                    stream.Read(buffer, 0, sizeof(Int32));
                    data = BitConverter.ToUInt32(buffer, 0);
                }
                break;
            default:
                throw new NotImplementedException($"Deserialization of type  '{thisTypeSerializationTypeRecord.TypeName}' and type class '{thisTypeSerializationTypeRecord.Type}' not implemented");
        }
    }

    public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerializators)
    {
        switch (thisTypeSerializationTypeRecord.TypeName)
        {
            case "Seed::AChar":
                {
                    var val = (byte)data;
                    stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));
                }
                break;
            case "Seed::UChar":
                {
                    var val = (char)data;
                    stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));
                }
                break;
            case "Seed::U8Char":
                {
                    var val = (byte)data;
                    stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));
                }
                break;
            case "Seed::U16Char":
                {
                    var val = (char)data;
                    stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));
                }
                break;
            case "Seed::U32Char":
                {
                    var val = (UInt32)data;
                    stream.Write(BitConverter.GetBytes(val), 0, sizeof(int));
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  '{thisTypeSerializationTypeRecord.TypeName}' and type class ' { thisTypeSerializationTypeRecord.Type} ' not implemented");
        }
    }

    public override List<SerializationTypeRecord> GetSupportedTypes() => new List<SerializationTypeRecord>
    {
        new SerializationTypeRecord
        {
            ClrType = typeof(byte),
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                TypeName = "Seed::AChar",
                SizeOfSize = sizeof(byte),
                StaticSize = sizeof(byte),
                AdditionalInfo = "ASCII",
                Type = ETypes.Character
            }
        },

        new SerializationTypeRecord
        {
            ClrType = typeof(char),
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                TypeName = "Seed::UChar",
                SizeOfSize = sizeof(char),
                StaticSize = sizeof(char),
                AdditionalInfo = "UTF-16",
                Type = ETypes.Character
            }
        },

        new SerializationTypeRecord
        {
            ClrType = typeof(byte),
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                TypeName = "Seed::U8Char",
                SizeOfSize = sizeof(byte),
                StaticSize = sizeof(byte),
                AdditionalInfo = "UTF-8",
                Type = ETypes.Character
            }
        },

        new SerializationTypeRecord
        {
            ClrType = typeof(char),
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                TypeName = "Seed::U16Char",
                SizeOfSize = sizeof(char),
                StaticSize = sizeof(char),
                AdditionalInfo = "UTF-16",
                Type = ETypes.Character
            }
        },

        new SerializationTypeRecord
        {
            ClrType = typeof(Int32),
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                TypeName = "Seed::U32Char",
                SizeOfSize = sizeof(Int32),
                StaticSize = sizeof(Int32),
                AdditionalInfo = "UTF-32",
                Type = ETypes.Character
            }
        }
    };
}
