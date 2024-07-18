using Seed.CTTI.MetaData;
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
            Constants.Types.RTTISection => typeof(RTTI.RTTISection),
            Constants.Types.TypeMetaDataRecord => typeof(TypeMetaDataRecord),
            Constants.Types.SensitiveTypeMetaDataRecord => typeof(TypeMetaDataRecord),
            _ => null,
        };
    }

    private void AddMetaDataSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case Constants.Types.TypeMetaDataRecord:
            case Constants.Types.SensitiveTypeMetaDataRecord:
                {
                    code.AppendLine(Constants.SerCodeBlocks.TypeMetaDataRecord);
                    break;
                }

            case Constants.Types.RTTISection:
                {
                    code.AppendLine(Constants.SerCodeBlocks.RTTISection);
                    break;
                }
        }
    }

    private void AddMetaDataDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case Constants.Types.TypeMetaDataRecord:
            case Constants.Types.SensitiveTypeMetaDataRecord:
                {
                    code.AppendLine(Constants.DerCodeBlocks.TypeMetaDataRecord);
                    break;
                }

            case Constants.Types.RTTISection:
                {
                    code.AppendLine(Constants.DerCodeBlocks.RTTISection);
                    break;
                }
        }
    }
}
