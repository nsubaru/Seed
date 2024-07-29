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
            Constants.Types.Int8                => typeof(System.SByte),
            Constants.Types.Int16               => typeof(System.Int16),
            Constants.Types.Int32               => typeof(System.Int32),
            Constants.Types.Int64               => typeof(System.Int64),
            Constants.Types.UInt8               => typeof(System.Byte),
            Constants.Types.UInt16              => typeof(System.UInt16),
            Constants.Types.UInt32              => typeof(System.UInt32),
            Constants.Types.UInt64              => typeof(System.UInt64),
            Constants.Types.SizeT               => typeof(System.UInt64),
            Constants.Types.Id                  => typeof(System.UInt64),
            Constants.Types.DataSizeT           => typeof(System.UInt64),
            Constants.Types.Double              => typeof(System.Double),
            Constants.Types.Float               => typeof(float),
            Constants.Types.Uuid                => typeof(System.Guid),
            Constants.Types.Boolean             => typeof(System.Boolean),
            Constants.Types.AChar               => typeof(System.Byte),
            Constants.Types.UChar               => typeof(System.Char),
            Constants.Types.U8Char              => typeof(System.Byte),
            Constants.Types.U16Char             => typeof(System.Char),
            Constants.Types.U32Char             => typeof(System.Int32),
            Constants.Types.String              => typeof(System.String),
            Constants.Types.SensitiveString     => typeof(System.Security.SecureString),
            Constants.Types.ExceptionString     => typeof(System.String),
            Constants.Types.UniString           => typeof(System.String),
            Constants.Types.SensitiveUniString  => typeof(System.Security.SecureString),
            Constants.Types.ExceptionUniString  => typeof(System.String),
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
