using Seed.Serialization.MetaData;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace Seed.Serialization.ClassSerializators
{
    class NumericClassSerializator : SimpeTypeSerializator
    {
        public override void Deserialize(ref object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            switch (thisTypeSerializationTypeRecord.TypeName)
            {
                case "Seed::UInt8":
                    {
                        byte[] buffer = new byte[sizeof(byte)];
                        stream.Read(buffer, 0, sizeof(byte));
                        data = buffer[0];
                    }
                    break;
                case "Seed::UInt16":
                    {
                        byte[] buffer = new byte[sizeof(UInt16)];
                        stream.Read(buffer, 0, sizeof(UInt16));
                        data = BitConverter.ToUInt16(buffer, 0);
                    }
                    break;
                case "Seed::UInt32":
                    {
                        byte[] buffer = new byte[sizeof(UInt32)];
                        stream.Read(buffer, 0, sizeof(UInt32));
                        data = BitConverter.ToUInt32(buffer, 0);
                    }
                    break;
                case "Seed::SizeT":
                case "Seed::UInt64":
                    {
                        byte[] buffer = new byte[sizeof(UInt64)];
                        stream.Read(buffer, 0, sizeof(UInt64));
                        data = BitConverter.ToUInt64(buffer, 0);
                    }
                    break;

                case "Seed::Int8":
                    {
                        byte[] buffer = new byte[sizeof(char)];
                        stream.Read(buffer, 0, sizeof(sbyte));
                        data = (sbyte)BitConverter.ToChar(buffer, 0);
                    }
                    break;
                case "Seed::Int16":
                    {
                        byte[] buffer = new byte[sizeof(Int16)];
                        stream.Read(buffer, 0, sizeof(Int16));
                        data = BitConverter.ToInt16(buffer, 0);
                    }
                    break;
                case "Seed::Int32":
                    {
                        byte[] buffer = new byte[sizeof(Int32)];
                        stream.Read(buffer, 0, sizeof(Int32));
                        data = BitConverter.ToInt32(buffer, 0);
                    }
                    break;
                case "Seed::Int64":
                    {
                        byte[] buffer = new byte[sizeof(Int64)];
                        stream.Read(buffer, 0, sizeof(Int64));
                        data = BitConverter.ToInt64(buffer, 0);
                    }
                    break;

                case "Seed::RefCountT":
                    {
                        byte[] buffer = new byte[sizeof(UInt64)];
                        stream.Read(buffer, 0, sizeof(UInt64));
                        data = BitConverter.ToUInt64(buffer, 0);
                    }
                    break;
                default:
                    throw new NotImplementedException($"Deserialization of type  '{thisTypeSerializationTypeRecord.TypeName}' and type class '{thisTypeSerializationTypeRecord.Type}' not implemented");
            }
        }

        public override void Serialize(object data, Stream stream, TypeMetaDataRecord thisTypeSerializationTypeRecord, List<SerializationTypeRecord> serializationTypeRecords, Dictionary<ETypes, ITypeSerializator> typeSerelizators)
        {
            switch (thisTypeSerializationTypeRecord.TypeName)
            {
                case "Seed::UInt8":
                    {
                        var val = (byte)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));
                    }
                    break;
                case "Seed::UInt16":
                    {
                        var val = (UInt16)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));
                    }
                    break;
                case "Seed::UInt32":
                    {
                        var val = (UInt32)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt32));
                    }
                    break;
                case "Seed::SizeT":
                case "Seed::UInt64":
                    {
                        var val = (UInt64)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));
                    }
                    break;

                case "Seed::Int8":
                    {
                        var val = (sbyte)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(sbyte));
                    }
                    break;
                case "Seed::Int16":
                    {
                        var val = (Int16)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int16));
                    }
                    break;
                case "Seed::Int32":
                    {
                        var val = (Int32)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int32));
                    }
                    break;
                case "Seed::Int64":
                    {
                        var val = (Int64)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));
                    }
                    break;

                case "Seed::RefCountT":
                    {
                        var val = (UInt64)data;
                        stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));
                    }
                    break;
                default:
                    throw new NotImplementedException($"Serialization of type  ' { thisTypeSerializationTypeRecord.TypeName} ' and type class ' { thisTypeSerializationTypeRecord.Type} ' not implemented");
            }
        }

        public override List<SerializationTypeRecord> GetSupportedTypes() => new List<SerializationTypeRecord>
        {
            #region Unsigned integer
            new SerializationTypeRecord
            {
                ClrType = typeof(byte),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::UInt8",
                    SizeOfSize = sizeof(byte),
                    StaticSize = sizeof(byte),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(UInt16),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::UInt16",
                    SizeOfSize = sizeof(UInt16),
                    StaticSize = sizeof(UInt16),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(UInt32),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::UInt32",
                    SizeOfSize = sizeof(UInt32),
                    StaticSize = sizeof(UInt32),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(UInt64),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::UInt64",
                    SizeOfSize = sizeof(UInt64),
                    StaticSize = sizeof(UInt64),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },
            #endregion

            #region Signed integer
            new SerializationTypeRecord
            {
                ClrType = typeof(sbyte),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Int8",
                    SizeOfSize = sizeof(sbyte),
                    StaticSize = sizeof(sbyte),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(Int16),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Int16",
                    SizeOfSize = sizeof(Int16),
                    StaticSize = sizeof(Int16),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(Int32),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Int32",
                    SizeOfSize = sizeof(Int32),
                    StaticSize = sizeof(Int32),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(Int64),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Int64",
                    SizeOfSize = sizeof(Int64),
                    StaticSize = sizeof(Int64),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },
            #endregion

            new SerializationTypeRecord
            {
                ClrType = typeof(UInt64),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::RefCountT",
                    SizeOfSize = sizeof(UInt64),
                    StaticSize = sizeof(UInt64),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric,
                    NestedTypes = new List<TypeMetaDataRecord>
                    {
                        new TypeMetaDataRecord
                        {
                            TypeName = "Seed::UInt64",
                            SizeOfSize = sizeof(UInt64),
                            StaticSize = sizeof(UInt64),
                            AdditionalInfo = "",
                            Type = ETypes.Numeric
                        }
                    }
                }
            },

            new SerializationTypeRecord
            {
                ClrType = typeof(UInt64),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::SizeT",
                    SizeOfSize = sizeof(UInt64),
                    StaticSize = sizeof(UInt64),
                    AdditionalInfo = "",
                    Type = ETypes.Numeric
                }
            },
        };
    }
}
