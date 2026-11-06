using Seed.CTTI.MetaData;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? GetMetaDataType(RTTI.Record.RttiTypeRecord record)
    {
        return record.TypeName switch
        {
            Constants.Types.RTTISection => typeof(RTTI.RTTISection),
            Constants.Types.TypeMetaDataRecord => typeof(TypeMetaDataRecord),
            Constants.Types.SensitiveTypeMetaDataRecord => typeof(TypeMetaDataRecord),
            Constants.Types.EmptyAttributeValueT => typeof(EmptyAttributeValueT),
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

            case Constants.Types.EmptyAttributeValueT:
                {
                    code.AppendLine(Constants.SerCodeBlocks.EmptyAttributeValueT);
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

            case Constants.Types.EmptyAttributeValueT:
                {
                    code.AppendLine(Constants.DerCodeBlocks.EmptyAttributeValueT);
                    break;
                }
        }
    }
}
