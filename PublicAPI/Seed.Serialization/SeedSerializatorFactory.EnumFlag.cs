using Seed.Core;
using System;
using System.Collections.Generic;
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

        var p1 = record.TypeName.IndexOf('<') + 1;
        var p2 = record.TypeName.IndexOf('>');
        var types = record.TypeName.Substring(p1, p2 - p1).Split(", ");
        var enumTypeRecord = this.allRttiTypes.FirstOrDefault(x => x.TypeName == types[0]);
        var underlyingTypeRecord = this.allRttiTypes.FirstOrDefault(x => x.TypeName == types[1]);

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
