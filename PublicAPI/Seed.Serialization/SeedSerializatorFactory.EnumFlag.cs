using Seed.Core;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? TryGetAsEnumFlag(RTTI.Record.RttiTypeRecord record)
    {
        if (!record.TypeName.StartsWith(Constants.Types.EnumFlag))
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

        return typeof(EnumFlag<,>).MakeGenericType(enumType, underlyingType);
    }

    private void AddEnumFlagSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = data.AsInteger();");
        code.AppendLine("this.SerializeImpl(value, stream);");
    }

    private void AddEnumFlagDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var value = data.AsInteger();");
        code.AppendLine("value = this.DeserializeImpl(value, stream);");
        code.AppendLine($"data =  new {GetFriendlyTypeName(typeof(T))} (value);");
    }
}
