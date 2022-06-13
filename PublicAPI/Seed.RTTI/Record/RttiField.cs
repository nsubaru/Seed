namespace Seed.RTTI.Record;

public unsafe class RttiField : RttiIterableObject<RttiField>
{
    internal RttiIterator<RttiField>? it;

    FieldMetaData* GetRecord() { return (FieldMetaData*)it!.item; }

    public RttiField()
    {
        it = null;
    }

    public RttiField(RttiIterator<RttiField> it)
    {
        this.it = it;
    }

    public RttiField CreateInstance(RttiIterator<RttiField> iterator)
    {
        return new RttiField(iterator);
    }

    public SizeT TypeId => (SizeT)GetRecord()->TypeId.TypeId + it!.imageBase;

    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    public ETypes TypeClass => GetRecord()->TypeClass;

    public SizeT BinarySize => GetRecord()->MetaDataForDynamic.BinarySize;

    public TypeModificator TypeModificators => GetRecord()->TypeModificators;

    public string FieldName => Marshal.PtrToStringAuto((IntPtr) GetRecord()->FieldName.Text) ?? string.Empty;

    internal string GetTypeName()
    {
        var pTypeName = (TextMetaData*)((Byte*)GetRecord()) +
            (SizeT)Marshal.OffsetOf<FieldMetaData>("FieldName") +
            (SizeT)Marshal.OffsetOf<TextMetaData>("Text") +
            (GetRecord()->FieldName.Size * sizeof(Char));

        return Marshal.PtrToStringAuto((IntPtr) pTypeName->Text) ?? string.Empty;
    }

    public string TypeName => GetTypeName();

    static internal RttiCollection<RttiField> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiField>(record, imageBase, isValidCxxTypeId);
    }
}
