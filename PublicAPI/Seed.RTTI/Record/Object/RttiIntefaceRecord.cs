namespace Seed.RTTI.Record.Object;

public unsafe class RttiIntefaceRecord : RttiIterableObject<RttiIntefaceRecord>
{
    internal RttiIterator<RttiIntefaceRecord>? it;

    InterfaceRecord* GetRecord() { return (InterfaceRecord*)it!.item; }

    public RttiIntefaceRecord()
    {
        it = null;
    }

    public RttiIntefaceRecord(RttiIterator<RttiIntefaceRecord> it)
    {
        this.it = it;
    }

    public RttiIntefaceRecord CreateInstance(RttiIterator<RttiIntefaceRecord> iterator)
    {
        return new RttiIntefaceRecord(iterator);
    }

    public SizeT TypeId => GetRecord()->TypeId.TypeId + it!.imageBase;

    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    public string TypeName => Marshal.PtrToStringAuto((IntPtr)  GetRecord()->TypeName.Text) ?? string.Empty;

    static internal RttiCollection<RttiIntefaceRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiIntefaceRecord>(record, imageBase, isValidCxxTypeId);
    }
}
