using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization
{
    internal struct RttyTypeRecord
    {
        public string TypeName { get; set; }
        public string SerializationSchema { get; set; }
        public Type ClrType { get; set; }
        public CTTI.ETypes Type { get; set; }
        public Guid FullTypeId { get; set; }
    }
}
