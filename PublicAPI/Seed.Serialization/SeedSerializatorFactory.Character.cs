using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddCharacterTypeSer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::AChar":
                {
                    code.AppendLine("var val = (byte)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));");
                }
                break;
            case "Seed::Core::UChar":
                {
                    code.AppendLine("var val = (char)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));");
                }
                break;
            case "Seed::Core::U8Char":
                {
                    code.AppendLine("var val = (byte)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));");
                }
                break;
            case "Seed::Core::U16Char":
                {
                    code.AppendLine("var val = (byte)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));");
                }
                break;
            case "Seed::Core::U32Char":
                {
                    code.AppendLine("var val = (UInt32)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(int));");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }

    private void AddCharacterTypeDer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::AChar":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(byte)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(byte));");
                    code.AppendLine("data = buffer[0];");
                }
                break;
            case "Seed::Core::UChar":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(char)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(char));");
                    code.AppendLine("data = BitConverter.ToChar(buffer, 0);");
                }
                break;
            case "Seed::Core::U8Char":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(byte)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(byte));");
                    code.AppendLine("data = buffer[0];");
                }
                break;
            case "Seed::Core::U16Char":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(char)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(char));");
                    code.AppendLine("data = BitConverter.ToChar(buffer, 0);");
                }
                break;
            case "Seed::Core::U32Char":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(Int32)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(Int32));");
                    code.AppendLine("data = BitConverter.ToUInt32(buffer, 0);");
                }
                break;
            default:
                throw new NotImplementedException($"Deserialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
