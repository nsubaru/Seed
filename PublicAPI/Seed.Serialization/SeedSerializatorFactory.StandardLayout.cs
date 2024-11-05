using Microsoft.Extensions.Logging;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private Type? GetStandardLayoutType(RTTI.Record.RttiTypeRecord record)
    {
        if (record.TypeName == Constants.Types.EmptyAttributeValueT)
        {
            return typeof(Seed.CTTI.EmptyAttributeValueT);
        }

        return null;
    }

    private void AddStandardLayoutSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == Constants.Types.EmptyAttributeValueT)
        {
            code.AppendLine("return;");
        }
        else
        {
            logger.LogWarning($"Serialization of standard layout type ' {typeRecord.TypeName} ' not implemented");
            return;
        }
    }

    private void AddStandardLayoutDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == Constants.Types.EmptyAttributeValueT)
        {
            code.AppendLine("return;");
        }
        else
        {
            logger.LogWarning($"Deserialization of standard layout type ' {typeRecord.TypeName} ' not implemented");
            return;
        }
    }
}