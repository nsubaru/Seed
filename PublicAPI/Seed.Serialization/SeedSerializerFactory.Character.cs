using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddCharacterTypeSer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case Constants.Types.AChar:
                {
                    code.AppendLine("var val = (byte)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));");
                }
                break;
            case Constants.Types.UChar:
                {
                    code.AppendLine("var val = (char)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));");
                }
                break;
            case Constants.Types.U8Char:
                {
                    code.AppendLine("var val = (byte)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(byte));");
                }
                break;
            case Constants.Types.U16Char:
                {
                    code.AppendLine("var val = (char)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(char));");
                }
                break;
            case Constants.Types.U32Char:
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
            case Constants.Types.AChar:
                {
                    code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(byte)];");
                    code.AppendLine("stream.Read(buffer);");
                    code.AppendLine("data = buffer[0];");
                }
                break;
            case Constants.Types.UChar:
                {
                    code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(char)];");
                    code.AppendLine("stream.Read(buffer);");
                    code.AppendLine("data = BitConverter.ToChar(buffer);");
                }
                break;
            case Constants.Types.U8Char:
                {
                    code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(byte)];");
                    code.AppendLine("stream.Read(buffer);");
                    code.AppendLine("data = buffer[0];");
                }
                break;
            case Constants.Types.U16Char:
                {
                    code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(char)];");
                    code.AppendLine("stream.Read(buffer);");
                    code.AppendLine("data = BitConverter.ToChar(buffer);");
                }
                break;
            case Constants.Types.U32Char:
                {
                    code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(uint)];");
                    code.AppendLine("stream.Read(buffer);");
                    code.AppendLine("data = BitConverter.ToUInt32(buffer);");
                }
                break;
            default:
                throw new NotImplementedException($"Deserialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
