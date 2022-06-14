namespace Seed.RTTI.Record.Object;

/// <summary>
/// A base RTTI interface record class
/// </summary>
public unsafe class RttiIntefaceRecord : RttiIterableObject<RttiIntefaceRecord>
{
    internal RttiIterator<RttiIntefaceRecord>? it;

    InterfaceRecord* GetRecord() { return (InterfaceRecord*)it!.item; }

    public RttiIntefaceRecord()
    {
        it = null;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public RttiIntefaceRecord(RttiIterator<RttiIntefaceRecord> it)
    {
        this.it = it;
    }

    public RttiIntefaceRecord CreateInstance(RttiIterator<RttiIntefaceRecord> iterator)
    {
        return new RttiIntefaceRecord(iterator);
    }

    /// <summary>
    /// Receiving the interface record with type ID
    /// </summary>
    /// <returns>Record with type ID</returns>
    public SizeT TypeId => GetRecord()->TypeId.TypeId + it!.imageBase;

    /// <summary>
    /// Receiving the interface record with full type ID
    /// </summary>
    /// <returns>Record with full type ID</returns>
    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    /// <summary>
    /// Receiving the interface record with type name
    /// </summary>
    /// <returns>Record with type name</returns>
    public string TypeName => Marshal.PtrToStringAuto((IntPtr)  GetRecord()->TypeName.Text) ?? string.Empty;

    static internal RttiCollection<RttiIntefaceRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiIntefaceRecord>(record, imageBase, isValidCxxTypeId);
    }
}
