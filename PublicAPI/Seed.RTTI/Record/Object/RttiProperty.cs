namespace Seed.RTTI.Record.Object;

/// <summary>
/// A base RTTI property record class
/// </summary>
/// <typeparam name="THeap">Heap type</typeparam>
public unsafe class RttiProperty : RttiIterableObject<RttiProperty>
{
    internal RttiIterator<RttiProperty>? it;

    PropertyRecord* GetRecord() { return (PropertyRecord*)it!.item; }

    public RttiProperty()
    {
        it = null;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="from">Source</param>
    public RttiProperty(RttiIterator<RttiProperty> it)
    {
        this.it = it;
    }

    public RttiProperty CreateInstance(RttiIterator<RttiProperty> iterator)
    {
        return new RttiProperty(iterator);
    }

    /// <summary>
    /// Receiving the property record with type ID
    /// </summary>
    /// <returns>Record with type ID</returns>
    public SizeT TypeId => (SizeT)GetRecord()->PropertyType.TypeId.TypeId + it!.imageBase;

    /// <summary>
    /// Receiving the property record with full type ID
    /// </summary>
    /// <returns>Record with full type ID</returns>
    public UniversalTypeIdentificator FullTypeId => GetRecord()->PropertyType.TypeId;

    /// <summary>
    /// Receiving the property record with type class
    /// </summary>
    /// <returns>Record with type class</returns>
    public ETypes TypeClass => GetRecord()->PropertyType.TypeClass;

    /// <summary>
    /// Receiving the property record with binary size
    /// </summary>
    /// <returns>Record with binary size</returns>
    public SizeT BinarySize => GetRecord()->PropertyType.MetaDataForDynamic.BinarySize;

    /// <summary>
    /// Receiving the property record with type modifiers
    /// </summary>
    /// <returns>Record with type modifiers</returns>
    public TypeModificator TypeModificators => GetRecord()->PropertyType.TypeModificators;

    /// <summary>
    /// Receiving the property record with name
    /// </summary>
    /// <returns>Property name</returns>
    public string Name => Marshal.PtrToStringAuto((IntPtr) GetRecord()->PropertyType.FieldName.Text) ?? string.Empty;

    /// <summary>
    /// Receiving the property record with type name
    /// </summary>
    /// <returns>Property type name</returns>
    internal string GetTypeName()
    {
        var pTypeName = (TextMetaData*)(((Byte*)GetRecord()) +
            ((SizeT)Marshal.OffsetOf<PropertyRecord>("PropertyType").ToInt64()) +
            ((SizeT)Marshal.OffsetOf<FieldMetaDataRecord>("FieldName").ToInt64()) +
            ((SizeT)Marshal.OffsetOf<TextMetaData>("Text").ToInt64()) +
            (GetRecord()->PropertyType.FieldName.Size * sizeof(Char)));

        return Marshal.PtrToStringAuto((IntPtr) pTypeName->Text) ?? string.Empty;
    }

    public string TypeName => GetTypeName();

    /// <summary>
    /// Receiving the property record with getters
    /// </summary>
    /// <returns>Record with getters</returns>
    internal RttiCollection<RttiMethodRecord> GetGetters()
    {
        var @base =  (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToGetters);

        return RttiMethodRecord.CreateCollection(@base, it!.isValidCxxTypeId, it!.imageBase);
    }

    public RttiCollection<RttiMethodRecord> Getters => GetGetters();

    /// <summary>
    /// Receiving the property record with setters
    /// </summary>
    /// <returns>Record with setters</returns>
    internal RttiCollection<RttiMethodRecord> GetSetters()
    {
        var @base = (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToSetters);

        return RttiMethodRecord.CreateCollection(@base, it!.isValidCxxTypeId, it!.imageBase);
    }

    public RttiCollection<RttiMethodRecord> Setters => GetSetters();

    /// <summary>
    /// Indicates if the property is rw
    /// </summary>
    /// <returns>"True" if the record is rw</returns>
    internal bool GetIsRw()
    {
        var gettersRw = (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToGetters);
        var settersRw = (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToSetters);

        return gettersRw->Count > 0 && settersRw->Count > 0;
    }

    public bool IsRw => GetIsRw();

    static internal RttiCollection<RttiProperty> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiProperty>(record, imageBase, isValidCxxTypeId);
    }
}
