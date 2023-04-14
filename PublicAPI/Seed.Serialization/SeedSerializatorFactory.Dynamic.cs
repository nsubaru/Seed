using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddDynamicSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var record = typesDict.Where(x => x.Value.ClrType == data.Value.GetType()).First();");
        code.AppendLine($"UniversalTypeIdentificator id = new UniversalTypeIdentificator();");
        code.AppendLine($"id.Id = record.Key;");
        code.AppendLine($"Serialize(id, stream);");
        code.AppendLine($"record.Value.SerializeImpl.Invoke(this, new object[] {{data.Value,stream}});");
    }

    private void AddDynamicDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"UniversalTypeIdentificator id = new UniversalTypeIdentificator();");
        code.AppendLine($"dynamic idDyn = id;");
        code.AppendLine($"Deserialize(ref idDyn, stream);");
        code.AppendLine($"id = (UniversalTypeIdentificator) idDyn;");
        code.AppendLine($"data.Value = typesDict[id.Id].DeserializeImpl.Invoke(this, new object[] {{data.Value,stream}});");
    }
}
