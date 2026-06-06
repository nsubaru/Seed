using Seed.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    Type? TryGetAsEnumBitFlag(RTTI.Record.RttiTypeRecord record)
    {
        if (!record.TypeName.StartsWith(Constants.Types.EnumBitFlag))
        {
            return null;
        }

        var enumTypeRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId == record.Fields.First().FullTypeId);
        var underlyingTypeRecord = allRttiTypes.FirstOrDefault(x => x.FullTypeId == record.Fields.Last().FullTypeId);

        if (enumTypeRecord == null || underlyingTypeRecord == null)
        {
            return null;
        }

        var enumType = BuildClrType(enumTypeRecord);
        var underlyingType = BuildClrType(underlyingTypeRecord);

        if (enumType == null || underlyingType == null)
        {
            return null;
        }

        return enumType;
    }

    private void AddEnumBitFlagSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("this.SerializeImpl(value, stream);");
    }

    private void AddEnumBitFlagDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = ({GetFriendlyTypeName(typeof(T).GetEnumUnderlyingType())}) data;");
        code.AppendLine("value = this.DeserializeImpl(value, stream);");
        code.AppendLine($"data = ({GetFriendlyTypeName(typeof(T))}) value;");
    }
}
