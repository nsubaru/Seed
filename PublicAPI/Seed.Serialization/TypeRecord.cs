using Seed.RTTI.Record;

namespace Seed.Serialization;

public struct TypeRecord
{
    public string TypeName { get; set; }
    public string SerializationSchema { get; set; }
    public Type ClrType { get; set; }
    public CTTI.ETypes Type { get; set; }
    public Guid FullTypeId { get; set; }
    public RttiTypeRecord RttiType { get; set; }
}
