using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? GetMetaDataType(RTTI.Record.RttiTypeRecord record)
    {
        return record.TypeName switch
        {
            "Seed::Core::RTTI::RTTISection" => typeof(RTTI.RTTISection),
            "Seed::Core::Serialization::TypeMetaDataRecord" or "Seed::Core::Serialization::SensitiveTypeMetaDataRecord" => typeof(Seed.CTTI.MetaData.TypeMetaDataRecord),
            _ => null,
        };
    }

    private void AddMetaDataSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord" || typeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
        {
            code.AppendLine($"string serializationText = data.ToString(\"S\");");
        }
        else if (typeRecord.TypeName == "Seed::Core::RTTI::RTTISection")
        {
            code.AppendLine($"string serializationText = data.ToString();");
        }

        code.AppendLine($"SerializeImpl(serializationText, stream);");
    }

    private void AddMetaDataDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.TypeName == "Seed::Core::Serialization::TypeMetaDataRecord" || typeRecord.TypeName == "Seed::Core::Serialization::SensitiveTypeMetaDataRecord")
        {
            code.AppendLine($"string testMetaData = string.Empty;");
            code.AppendLine($"string testMetaData = \"\";");
            code.AppendLine($"testMetaData = DeserializeImpl(testMetaData, stream);");
            code.AppendLine($"data = new TypeMetaDataDecoder().DecodeTypeMetaData(testMetaData);");
        }
        else if (typeRecord.TypeName == "Seed::Core::RTTI::RTTISection")
        {
            code.AppendLine($"MemoryStream ms = new MemoryStream();");
            code.AppendLine($"stream.CopyTo(ms);");
            code.AppendLine($"var bytes = ms.ToArray();");
            code.AppendLine($"data = new RTTI.RTTISection(bytes);");
        }
    }
}
