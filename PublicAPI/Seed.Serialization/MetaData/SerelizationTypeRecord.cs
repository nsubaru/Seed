using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization.MetaData
{
    public struct SerializationTypeRecord
    {
        public string SeedKernelTypeName { get => SeedKernelTypeShema.TypeName; set => SeedKernelTypeShema.TypeName = value; }
        public Type ClrType { get; set; }
        public ETypes SeedKernelTypeClass { get => SeedKernelTypeShema.Type; set => SeedKernelTypeShema.Type = value; }
        public TypeMetaDataRecord SeedKernelTypeShema { get; set; }
    }
}
