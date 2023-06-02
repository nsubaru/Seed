using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? TryGetAsPrimitive(RTTI.Record.RttiTypeRecord record)
    {
        var res = TryGetAsEnumFlag(record);

        if (res != null)
            return res;

        return record.TypeName switch
        {
            "Seed::Core::Int8" => typeof(System.SByte),
            "Seed::Core::Int16" => typeof(System.Int16),
            "Seed::Core::Int32" => typeof(System.Int32),
            "Seed::Core::Int64" => typeof(System.Int64),
            "Seed::Core::UInt8" => typeof(System.Byte),
            "Seed::Core::UInt16" => typeof(System.UInt16),
            "Seed::Core::UInt32" => typeof(System.UInt32),
            "Seed::Core::UInt64" => typeof(System.UInt64),
            "Seed::Core::SizeT" => typeof(System.UInt64),
            "Seed::Core::Memory::DataSizeT" => typeof(System.UInt64),
            "Seed::Core::Double" => typeof(System.Double),
            "Seed::Core::Float" => typeof(float),
            "Seed::Core::UUID" => typeof(System.Guid),
            "Seed::Core::Boolean" => typeof(System.Boolean),
            "Seed::Core::AChar" => typeof(System.Byte),
            "Seed::Core::UChar" => typeof(System.Char),
            "Seed::Core::U8Char" => typeof(System.Byte),
            "Seed::Core::U16Char" => typeof(System.Char),
            "Seed::Core::U32Char" => typeof(System.Int32),
            "Seed::Core::String" => typeof(System.String),
            "Seed::Core::SensitiveString" => typeof(System.Security.SecureString),
            _ => null
        };
    }

    private void AddSimpleTypeSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.RttiType.TypeName.StartsWith("Seed::Core::EnumFlag"))
        {
            AddEnumFlagSer<T>(ref code, typeRecord);
            return;
        }    

        switch (typeRecord.Type)
        {
            case CTTI.ETypes.FloatNumeric:
                AddFloatNumericSer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Character:
                AddCharacterTypeSer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Numeric:
                AddNumericSer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Logic:
                AddLogicSer<T>(ref code, typeRecord.TypeName);
                break;
            default:
                break;
        }
    }

    private void AddSimpleTypeDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        if (typeRecord.RttiType.TypeName.StartsWith("Seed::Core::EnumFlag"))
        {
            AddEnumFlagDer<T>(ref code, typeRecord);
            return;
        }

        switch (typeRecord.Type)
        {
            case CTTI.ETypes.FloatNumeric:
                AddFloatNumericDer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Character:
                AddCharacterTypeDer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Numeric:
                AddNumericDer<T>(ref code, typeRecord.TypeName);
                break;
            case CTTI.ETypes.Logic:
                AddLogicDer<T>(ref code, typeRecord.TypeName);
                break;
        }
    }
}
