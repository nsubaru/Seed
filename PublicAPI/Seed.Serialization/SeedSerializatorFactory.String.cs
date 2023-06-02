using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddStringSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == "Seed::Core::SensitiveString")
        {
            code.AppendLine($"char[] chars;");
            code.AppendLine($"var ptr = IntPtr.Zero;");
            code.AppendLine($"try{{");
            code.AppendLine($"ptr = Marshal.SecureStringToBSTR(data);");
            code.AppendLine($"chars = new char[data.Length];");
            code.AppendLine($"Marshal.Copy(ptr, chars, 0, data.Length);}}");
            code.AppendLine($"finally{{");
            code.AppendLine($"if (ptr != IntPtr.Zero){{");
            code.AppendLine($"Marshal.ZeroFreeBSTR(ptr);}}}}");
            code.AppendLine($"var bytes = Encoding.Unicode.GetBytes(chars);");
        }
        else
        {
            code.AppendLine($"var bytes = Encoding.Unicode.GetBytes(data);");
        }
        code.AppendLine($"SerializeImpl(((ulong)data.Length + 1) * 2, stream);");
        code.AppendLine($"stream.Write(bytes, 0, bytes.Length);");
        code.AppendLine(@"stream.Write(Encoding.Unicode.GetBytes(new char[] { '\0' }), 0, 1 * sizeof(char));");
    }

    private void AddStringDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var strSize = (UInt64)0;");
        code.AppendLine($"strSize = DeserializeImpl(strSize, stream);");
        code.AppendLine($"if (strSize == 0) return CreateObject<{GetFriendlyTypeName(typeof(T))}>();");
        code.AppendLine($"strSize -= 2;");
        code.AppendLine($"byte[] buffer = new byte[strSize];");
        code.AppendLine($"stream.Read(buffer, 0, (int)strSize);");

        if (typeRecord.TypeName == "Seed::Core::SensitiveString")
        {
            code.AppendLine($"var plain = Encoding.Unicode.GetString(buffer);");
            code.AppendLine(@"data = plain.ToCharArray().Aggregate(new SecureString(), (s, c) => {s.AppendChar(c); return s; }, (s) => {s.MakeReadOnly(); return s; });");
        }
        else
        {
            code.AppendLine($"data = Encoding.Unicode.GetString(buffer);");
        }

        code.AppendLine($"stream.Seek(2, SeekOrigin.Current);");
    }
}
