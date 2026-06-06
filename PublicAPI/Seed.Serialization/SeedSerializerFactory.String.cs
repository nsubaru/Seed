using System;
using System.Collections.Generic;
using System.Security;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddStringSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"ulong count = data.Length != 0 ? ((ulong)data.Length) + 1ul : 0ul;");
        code.AppendLine($"SerializeImpl(count, stream);");
        code.AppendLine($"if (data.Length == 0) return;");


        if (typeRecord.TypeName == Constants.Types.SensitiveString ||
            typeRecord.TypeName == Constants.Types.SensitiveUniString ||
            typeof(T) == typeof(SecureString))
        {
            // Use unsafe for dirrect bind marshaled ptr to span
            code.AppendLine("var ptr = IntPtr.Zero;");
            code.AppendLine("try {");
            code.AppendLine("    ptr = Marshal.SecureStringToBSTR(data);");
            code.AppendLine("    unsafe {{");
            code.AppendLine("        Span<byte> chars = new Span<byte>(ptr.ToPointer(), data.Length * sizeof(char));");
            code.AppendLine("        stream.Write(chars);");
            code.AppendLine("    }}");
            code.AppendLine("}");
            code.AppendLine("finally { if (ptr != IntPtr.Zero) Marshal.ZeroFreeBSTR(ptr); }");
        }
        else if (typeRecord.TypeName == Constants.Types.String ||
                 typeRecord.TypeName == Constants.Types.ExceptionString ||
                 typeRecord.TypeName == Constants.Types.UniString ||
                 typeRecord.TypeName == Constants.Types.ExceptionUniString ||
                 typeof(T) == typeof(String))
        {
            code.AppendLine("const int MaxStackAllocSize = 512;");  // 512 bytes (256 chars)

            code.AppendLine("int length = data.Length * sizeof(char);");
            code.AppendLine("Span<byte> buffer = length <= MaxStackAllocSize ? stackalloc byte[length] : new byte[length];");
            code.AppendLine("Encoding.Unicode.GetBytes(data, buffer);");
            code.AppendLine("stream.Write(buffer);");
        }
        else
        {
            throw new NotImplementedException($"Serialization of type '{typeRecord.TypeName}' not implemented");
        }

        code.AppendLine(@"stream.Write(Encoding.Unicode.GetBytes(new char[] { '\0' }), 0, sizeof(char));");
    }

    private void AddStringDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine("const int MaxStackAllocSize = 512;");  // 512 bytes for safe stack allocation

        code.AppendLine("var strSize = (UInt64) 0;");
        code.AppendLine("strSize = DeserializeImpl(strSize, stream);");
        code.AppendLine("if (strSize == 0) return CreateObject<" + GetFriendlyTypeName(typeof(T)) + ">();");

        code.AppendLine("strSize *= 2;");  // Convert to byte size
        code.AppendLine("strSize -= 2;");  // Remove null terminator

        // Handle SecureString separately
        if (typeRecord.TypeName == Constants.Types.SensitiveString ||
            typeRecord.TypeName == Constants.Types.SensitiveUniString ||
            typeof(T) == typeof(SecureString))
        {
            code.AppendLine("Span<byte> buffer = strSize <= MaxStackAllocSize ? stackalloc byte[(int)strSize] : new byte[strSize];");
            code.AppendLine("stream.Read(buffer);");
            code.AppendLine("var plain = Encoding.Unicode.GetString(buffer);");

            code.AppendLine("data = plain.ToCharArray().Aggregate(new SecureString(), (s, c) => { s.AppendChar(c); return s; }, (s) => { s.MakeReadOnly(); return s; });");
        }
        else if (typeRecord.TypeName == Constants.Types.String ||
                 typeRecord.TypeName == Constants.Types.ExceptionString ||
                 typeRecord.TypeName == Constants.Types.UniString ||
                 typeRecord.TypeName == Constants.Types.ExceptionUniString ||
                 typeof(T) == typeof(String))
        {
            code.AppendLine("Span<byte> buffer = strSize <= MaxStackAllocSize ? stackalloc byte[(int)strSize] : new byte[strSize];");
            code.AppendLine("stream.Read(buffer);");
            code.AppendLine("data = Encoding.Unicode.GetString(buffer);");
        }
        else
        {
            throw new NotImplementedException($"Deserialization of type '{typeRecord.TypeName}' not implemented");
        }

        // Important as serialized string always have zero terminator but C# strings do not use it
        code.AppendLine($"stream.Seek(2, SeekOrigin.Current);");
    }
}
