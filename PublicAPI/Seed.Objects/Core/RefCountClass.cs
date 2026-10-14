using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core;

public class RefCountClass
{
    [SerializableStructFieldManual("ObjId", "Seed::Core::SizeT")]
    public SizeT ObjId
    {
        get;
        set;
    }
}
