using Seed.Core;
using System;
using System.Collections.Generic;
using System.Text;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddNumericSer<T>(ref StringBuilder code, string typeName)
    {
        var type = typeof(T);

        switch (type)
        {
            case Type _ when type == typeof(byte) || typeName == Constants.Types.UInt8:
            {
                code.AppendLine("stream.WriteByte((byte) data);");
                break;
            }

            case Type _ when type == typeof(UInt16) || typeName == Constants.Types.UInt16:
                {
                    code.AppendLine("var val = (UInt16)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");

                    break;
                }

            case Type _ when type == typeof(UInt32) || typeName == Constants.Types.UInt32:
            {
                code.AppendLine("var val = (UInt32)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt32));");
                
                break;
            }

            case Type _ when type == typeof(UInt64) || typeName == Constants.Types.UInt64 || typeName == Constants.Types.SizeT || typeName == Constants.Types.Id || typeName == Constants.Types.DataSizeT:
            {
                code.AppendLine("var val = (UInt64)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                
                break;
            }

            case Type _ when type == typeof(sbyte) || typeName == Constants.Types.Int8:
            {
                code.AppendLine("stream.WriteByte(unchecked((byte) (sbyte) data));");
                break;
            }

            case Type _ when type == typeof(Int16) || typeName == Constants.Types.Int16:
            {
                code.AppendLine("var val = (Int16)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int16));");
                
                break;
            }

            case Type _ when type == typeof(Int32) || typeName == Constants.Types.Int32:
            {
                code.AppendLine("var val = (Int32)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int32));");
                
                break;
            }

            case Type _ when type == typeof(Int64) || typeName == Constants.Types.Int64:
            {
                code.AppendLine("var val = (Int64)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                
                break;
            }

            case Type _ when typeName == Constants.Types.RefCountT:
            {
                code.AppendLine("var val = (UInt64)data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                
                break;
            }

            case Type _ when type == typeof(Guid) || typeName == Constants.Types.Uuid:
            {
                code.AppendLine("var val = (Guid)data;");
                code.AppendLine("stream.Write(val.ToByteArray(), 0, 16);");
                
                break;
            }

            default:
                throw new NotImplementedException($"Serialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }

    private void AddNumericDer<T>(ref StringBuilder code, string typeName)
    {
        var type = typeof(T);

        switch (type)
        {
            case Type _ when type == typeof(byte) || typeName == Constants.Types.UInt8:
            {
                code.AppendLine("data = (byte) stream.ReadByte();");
                break;
            }

            case Type _ when type == typeof(UInt16) || typeName == Constants.Types.UInt16:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(UInt16)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToUInt16(buffer);");

                break;
            }

            case Type _ when type == typeof(UInt32) || typeName == Constants.Types.UInt32:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(UInt32)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToUInt32(buffer);");

                break;
            }

            case Type _ when type == typeof(UInt64) || typeName == Constants.Types.UInt64 || typeName == Constants.Types.SizeT || typeName == Constants.Types.Id || typeName == Constants.Types.DataSizeT:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(UInt64)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToUInt64(buffer);");

                break;
            }

            case Type _ when type == typeof(sbyte) || typeName == Constants.Types.Int8:
            {
                code.AppendLine("data = unchecked((sbyte) (byte) stream.ReadByte());");
                break;
            }

            case Type _ when type == typeof(Int16) || typeName == Constants.Types.Int16:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(Int16)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToInt16(buffer);");

                break;
            }

            case Type _ when type == typeof(Int32) || typeName == Constants.Types.Int32:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(Int32)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToInt32(buffer);");

                break;
            }

            case Type _ when type == typeof(Int64) || typeName == Constants.Types.Int64:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(Int64)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToInt64(buffer);");

                break;
            }

            case Type _ when typeName == Constants.Types.RefCountT:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(UInt64)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToUInt64(buffer);");

                break;
            }

            case Type _ when type == typeof(Guid) || typeName == Constants.Types.Uuid:
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[16];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = new Guid(buffer);");
                
                break;
            }

            default:
                throw new NotImplementedException($"Deserialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
