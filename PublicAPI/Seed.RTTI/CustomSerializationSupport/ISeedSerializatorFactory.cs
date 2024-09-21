using Seed.RTTI.Record;
using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.RTTI.CustomSerializationSupport;

public interface ISeedSerializatorFactory
{
    Type? BuildClrType(RttiTypeRecord record);

    RTTISection RTTISection { get; }
}
