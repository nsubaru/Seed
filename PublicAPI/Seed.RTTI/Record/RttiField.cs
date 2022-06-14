namespace Seed.RTTI.Record;

/// <summary>
/// A base RTTI field class
/// </summary>
/// <typeparam name="THeap">Heap type</typeparam>
public unsafe class RttiField : RttiIterableObject<RttiField>
{
    internal RttiIterator<RttiField>? it;

    FieldMetaData* GetRecord() { return (FieldMetaData*)it!.item; }

    public RttiField()
    {
        it = null;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="it">Iterator</param>
    public RttiField(RttiIterator<RttiField> it)
    {
        this.it = it;
    }

    public RttiField CreateInstance(RttiIterator<RttiField> iterator)
    {
        return new RttiField(iterator);
    }

    /// <summary>
    /// Receiving the field record with type ID
    /// </summary>
    /// <returns>Record with type ID</returns>
    public SizeT TypeId => (SizeT)GetRecord()->TypeId.TypeId + it!.imageBase;

    /// <summary>
    /// Receiving the field record with full type ID
    /// </summary>
    /// <returns>Record with type ID</returns>
    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    /// <summary>
    /// Receiving the field record with type class
    /// </summary>
    /// <returns>Record with type class</returns>
    public ETypes TypeClass => GetRecord()->TypeClass;

    /// <summary>
    /// Receiving the field record with binary size
    /// </summary>
    /// <returns>Record with binary size</returns>
    public SizeT BinarySize => GetRecord()->MetaDataForDynamic.BinarySize;

    /// <summary>
    /// Receiving the field record with type modifiers
    /// </summary>
    /// <returns>Record with type modifiers</returns>
    public TypeModificator TypeModificators => GetRecord()->TypeModificators;

    /// <summary>
    /// Receiving the record with field name
    /// </summary>
    /// <returns>Record with field name</returns>
    public string FieldName => Marshal.PtrToStringAuto((IntPtr) GetRecord()->FieldName.Text) ?? string.Empty;

    /// <summary>
    /// Receiving the field record with type name
    /// </summary>
    /// <returns>Record with type name</returns>
    internal string GetTypeName()
    {
        var pTypeName = (TextMetaData*)(((Byte*)GetRecord()) +
            ((SizeT)Marshal.OffsetOf<FieldMetaData>("FieldName").ToInt64()) +
            ((SizeT)Marshal.OffsetOf<TextMetaData>("Text").ToInt64()) +
            (GetRecord()->FieldName.Size * sizeof(Char)));

        return Marshal.PtrToStringAuto((IntPtr) pTypeName->Text) ?? string.Empty;
    }

    public string TypeName => GetTypeName();

    static internal RttiCollection<RttiField> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiField>(record, imageBase, isValidCxxTypeId);
    }
}
