namespace Seed.RTTI.Record.Object;

public unsafe class RttiArgumentRecord : RttiIterableObject<RttiArgumentRecord>
{
    internal RttiIterator<RttiArgumentRecord>? it;

    ArgumentMetaData* GetRecord() { return (ArgumentMetaData*)it!.item; }

    public RttiArgumentRecord()
    {
        it = null;
    }

    public RttiArgumentRecord(RttiIterator<RttiArgumentRecord> it)
    {
        this.it = it;
    }

    public RttiArgumentRecord CreateInstance(RttiIterator<RttiArgumentRecord> iterator)
    {
        return new RttiArgumentRecord(iterator);
    }

    public ETypes TypeClass => GetRecord()->TypeClass;

    public SizeT TypeId => GetRecord()->TypeId.TypeId + it!.imageBase;

    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    public string TypeName => Marshal.PtrToStringAuto((IntPtr)  GetRecord()->TypeName.Text) ?? string.Empty;

    public CvRefModificators Flags => GetRecord()->Flags;

    public SizeT BinarySize => GetRecord()->MetaDataForDynamic.BinarySize;

    public bool IsSerializable => GetRecord()->IsSerializable;

    static internal RttiCollection<RttiArgumentRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiArgumentRecord>(record, imageBase, isValidCxxTypeId);
    }
}
