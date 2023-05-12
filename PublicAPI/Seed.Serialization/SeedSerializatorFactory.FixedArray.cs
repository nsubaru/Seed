using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private int TryGetFixedRank(string typeSceme) => typeSceme.Count(x => x == '[');

    private Type? TryBuildFixedArrayType(RTTI.Record.RttiTypeRecord record, RTTI.RTTISection section)
    {
        var rank = TryGetFixedRank(record.TypeName);

        var innerTypeRecord = GetRecordByField(section, record.Fields.Skip(0).First());

        for (int i = 0; i < rank - 1; i++)
        {
            innerTypeRecord = GetRecordByField(section, innerTypeRecord!.Fields.Skip(0).First());
        }

        if (innerTypeRecord == null)
            return null;

        var innerType = BuildClrType(innerTypeRecord);

        if (innerType == null)
            return null;

        var type = innerType.MakeArrayType();

        for (int i = 0; i < rank - 1; i++)
        {
            type = type.MakeArrayType();
        }

        return type;
    }
}
