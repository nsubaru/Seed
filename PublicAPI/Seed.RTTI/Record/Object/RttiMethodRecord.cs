namespace Seed.RTTI.Record.Object;

public unsafe class RttiMethodRecord : RttiIterableObject<RttiMethodRecord>
{
    internal RttiIterator<RttiMethodRecord>? it;

    MethodMetaData* GetRecord() { return (MethodMetaData*)it!.item; }

    public RttiMethodRecord()
    {
        it = null;
    }

    public RttiMethodRecord(RttiIterator<RttiMethodRecord> it)
    {
        this.it = it;
    }

    public RttiMethodRecord CreateInstance(RttiIterator<RttiMethodRecord> iterator)
    {
        return new RttiMethodRecord(iterator);
    }

    internal RttiCollection<RttiArgumentRecord> GetArguments()
    {
        var collectionBase = (Collection*)((byte*)GetRecord() + GetRecord()->MethodArgsOffset);
        return RttiArgumentRecord.CreateCollection(collectionBase, it!.isValidCxxTypeId, it!.imageBase);
    }

    public CvRefModificators MethodCVRefModificators => GetRecord()->MethodCVRefModificators;

    public string Name => Marshal.PtrToStringAuto((IntPtr)  GetRecord()->MethodName.Text) ?? string.Empty;

    public RttiCollection<RttiArgumentRecord> Arguments => GetArguments();

    static internal RttiCollection<RttiMethodRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiMethodRecord>(record, imageBase, isValidCxxTypeId);
    }
}
