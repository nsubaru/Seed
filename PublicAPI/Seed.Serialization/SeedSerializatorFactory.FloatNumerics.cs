using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddFloatNumericSer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::Float":
                {
                    code.AppendLine("var val = (float)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(float));");
                }
                break;
            case "Seed::Core::Double":
                {
                    code.AppendLine("var val = (double)data;");
                    code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(double));");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }

    private void AddFloatNumericDer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::Float":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(float)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(float));");
                    code.AppendLine("data = BitConverter.ToSingle(buffer, 0);");
                }
                break;
            case "Seed::Core::Double":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(double)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(double));");
                    code.AppendLine("data = BitConverter.ToDouble(buffer, 0);");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
