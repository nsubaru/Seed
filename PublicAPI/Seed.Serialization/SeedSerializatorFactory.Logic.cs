using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddLogicSer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::Boolean":
                {
                    code.AppendLine($"bool val = (bool) data;");
                    code.AppendLine($"stream.Write(BitConverter.GetBytes(val), 0, 1);");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  '{typeName}' and type class '{typeName}' not implemented");
        }
    }

    private void AddLogicDer<T>(ref StringBuilder code, string typeName)
    {
        switch (typeName)
        {
            case "Seed::Core::Boolean":
                {
                    code.AppendLine("byte[] buffer = new byte[sizeof(bool)];");
                    code.AppendLine("stream.Read(buffer, 0, sizeof(bool));");
                    code.AppendLine("data = BitConverter.ToBoolean(buffer, 0);");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  '{typeName}' and type class '{typeName}' not implemented");
        }
    }
}
