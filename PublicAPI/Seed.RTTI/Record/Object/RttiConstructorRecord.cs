namespace Seed.RTTI.Record.Object;

public unsafe class RttiConstructorRecord : RttiIterableObject<RttiConstructorRecord>
{
    internal RttiIterator<RttiConstructorRecord>? it;

    Constructor* GetRecord() { return (Constructor*) it!.item; }

    public RttiConstructorRecord()
    {
        it = null;
    }

    public RttiConstructorRecord(RttiIterator<RttiConstructorRecord> it)
    {
        this.it = it;
    }

    public RttiConstructorRecord CreateInstance(RttiIterator<RttiConstructorRecord> iterator)
    {
        return new RttiConstructorRecord(iterator);
    }

    internal RttiCollection<RttiArgumentRecord> GetArguments()
    {
        var collectionBase = (Collection*)((byte*)GetRecord() + GetRecord()->OffsetToArgs);

        return RttiArgumentRecord.CreateCollection(collectionBase, it!.isValidCxxTypeId, it!.imageBase);
    }

    public RttiCollection<RttiArgumentRecord> Arguments => GetArguments();

    static internal RttiCollection<RttiConstructorRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiConstructorRecord>(record, imageBase, isValidCxxTypeId);
    }
}
