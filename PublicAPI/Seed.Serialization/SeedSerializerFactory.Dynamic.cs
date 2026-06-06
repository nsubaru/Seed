using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddDynamicSer<T>(ref StringBuilder code, TypeRecord _)
    {
        code.AppendLine($"var record = reverseTypeDict[data.Value.GetType()];");
        code.AppendLine($"UniversalTypeIdentifier id = new UniversalTypeIdentifier();");
        code.AppendLine($"id.Id = record.Value.FullTypeId;");
        code.AppendLine($"Serialize(id, stream);");
        code.AppendLine($"record.Value.SerializeImpl.Invoke(this, new object[] {{data.Value,stream}});");
    }

    private void AddDynamicDer<T>(ref StringBuilder code, TypeRecord _)
    {
        code.AppendLine($"UniversalTypeIdentifier id = new UniversalTypeIdentifier();");
        code.AppendLine($"dynamic idDyn = id;");
        code.AppendLine($"Deserialize(ref idDyn, stream);");
        code.AppendLine($"id = (UniversalTypeIdentifier) idDyn;");
        code.AppendLine($"data.Value = typesDict[id.Id].DeserializeImpl.Invoke(this, new object[] {{data.Value,stream}});");
    }
}
