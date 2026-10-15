using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddFloatNumericSer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::Float":
            {
                code.AppendLine("var val = (float) data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(float));");

                break;
            }

            case "Seed::Core::Double":
            {
                code.AppendLine("var val = (double) data;");
                code.AppendLine("stream.Write(BitConverter.GetBytes(val), 0, sizeof(double));");

                break;
            }

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
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(float)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToSingle(buffer);");

                break;
            }

            case "Seed::Core::Double":
            {
                code.AppendLine("Span<byte> buffer = stackalloc byte[(int) sizeof(double)];");
                code.AppendLine("stream.Read(buffer);");
                code.AppendLine("data = BitConverter.ToDouble(buffer);");

                break;
            }
            
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeName} ' and type class ' {typeName} ' not implemented");
        }
    }
}
