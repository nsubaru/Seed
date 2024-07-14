using Seed.Core;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddNumericSer<T>(ref StringBuilder code, string typeName)
    {
        var type = typeof(T);
        switch (type)
        {
            case Type _ when type == typeof(byte) || typeName == "Seed::Core::UInt8": 
                {
                    code.AppendLine("stream.WriteByte((byte) data);");
                    break; 
                }
            case Type _ when type == typeof(UInt16) || typeName == "Seed::Core::UInt16": 
                {
                    code.AppendLine("var val = (UInt16)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt16));");
                    break;
                }
            case Type _ when type == typeof(UInt32) || typeName == "Seed::Core::UInt32": 
                {
                    code.AppendLine("var val = (UInt32)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt32));");
                    break; 
                }
            case Type _ when type == typeof(UInt64) || typeName == "Seed::Core::UInt64" || typeName == "Seed::Core::SizeT" || typeName == "Seed::Core::ID" || typeName == "Seed::Core::Memory::DataSizeT": 
                {
                    code.AppendLine("var val = (UInt64)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                    break;
                }
            case Type _ when type == typeof(sbyte) || typeName == "Seed::Core::Int8":
                {
                    code.AppendLine("stream.WriteByte(unchecked((byte) (sbyte) data));");
                    break; 
                }
            case Type _ when type == typeof(Int16) || typeName == "Seed::Core::Int16": 
                {
                    code.AppendLine("var val = (Int16)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int16));");
                    break; 
                }
            case Type _ when type == typeof(Int32) || typeName == "Seed::Core::Int32": 
                {
                    code.AppendLine("var val = (Int32)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(Int32));");
                    break;
                }
            case Type _ when type == typeof(Int64) || typeName == "Seed::Core::Int64": 
                {
                    code.AppendLine("var val = (Int64)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                    break; 
                }
            case Type _ when typeName == "Seed::Core::RefCountT": 
                {
                    code.AppendLine("var val = (UInt64)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(UInt64));");
                    break; 
                }
            case Type _ when type == typeof(Guid) || typeName == "Seed::Core::UUID": 
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
            case Type _ when type == typeof(byte) || typeName == "Seed::Core::UInt8":
                {
                    code.AppendLine("data = (byte) stream.ReadByte();");
                    break;
                }
            case Type _ when type == typeof(UInt16) || typeName == "Seed::Core::UInt16":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(UInt16)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(UInt16));");
                    code.AppendLine("data = BitConverter.ToUInt16(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(UInt32) || typeName == "Seed::Core::UInt32":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(UInt32)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(UInt32));");
                    code.AppendLine("data = BitConverter.ToUInt32(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(UInt64) || typeName == "Seed::Core::UInt64" || typeName == "Seed::Core::SizeT" || typeName == "Seed::Core::ID" || typeName == "Seed::Core::Memory::DataSizeT":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(UInt64)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(UInt64));");
                    code.AppendLine("data = BitConverter.ToUInt64(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(sbyte) || typeName == "Seed::Core::Int8":
                {
                    code.AppendLine("data = unchecked((sbyte) (byte) stream.ReadByte());");
                    break;
                }
            case Type _ when type == typeof(Int16) || typeName == "Seed::Core::Int16":
                {
                    code.AppendLine(" byte[] buffer = new byte[sizeof(Int16)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(Int16));");
                    code.AppendLine("data = BitConverter.ToInt16(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(Int32) || typeName == "Seed::Core::Int32":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(Int32)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(Int32));");
                    code.AppendLine("data = BitConverter.ToInt32(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(Int64) || typeName == "Seed::Core::Int64":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(Int64)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(Int64));");
                    code.AppendLine("data = BitConverter.ToInt64(buffer, 0);");
                    break;
                }
            case Type _ when typeName == "Seed::Core::RefCountT":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(UInt64)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(UInt64));");
                    code.AppendLine("data = BitConverter.ToUInt64(buffer, 0);");
                    break;
                }
            case Type _ when type == typeof(Guid) || typeName == "Seed::Core::UUID":
                {
                    code.AppendLine("byte[] rawData = new byte[16];");
                    code.AppendLine("stream.Read(rawData, 0, 16);");
                    code.AppendLine("data = new Guid(rawData);");
                    break;
                }
            default:
                throw new NotImplementedException($"Deserialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
