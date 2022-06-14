namespace Seed.RTTI.Record.Object;

/// <summary>
/// A base RTTI argument record class
/// </summary>
public unsafe class RttiArgumentRecord : RttiIterableObject<RttiArgumentRecord>
{
    internal RttiIterator<RttiArgumentRecord>? it;

    ArgumentMetaData* GetRecord() { return (ArgumentMetaData*)it!.item; }

    public RttiArgumentRecord()
    {
        it = null;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public RttiArgumentRecord(RttiIterator<RttiArgumentRecord> it)
    {
        this.it = it;
    }

    public RttiArgumentRecord CreateInstance(RttiIterator<RttiArgumentRecord> iterator)
    {
        return new RttiArgumentRecord(iterator);
    }

    /// <summary>
    /// Receiving the argument record with type class
    /// </summary>
    /// <returns>Record with type class</returns>
    public ETypes TypeClass => GetRecord()->TypeClass;

    /// <summary>
    /// Receiving the argument record with type ID
    /// </summary>
    /// <returns>Record with type ID</returns>
    public SizeT TypeId => GetRecord()->TypeId.TypeId + it!.imageBase;

    /// <summary>
    /// Receiving the argument record with full type ID
    /// </summary>
    /// <returns>Record with full type ID</returns>
    public UniversalTypeIdentificator FullTypeId => GetRecord()->TypeId;

    /// <summary>
    /// Receiving the argument record with type name
    /// </summary>
    /// <returns>Record with type name</returns>
    public string TypeName => Marshal.PtrToStringAuto((IntPtr)  GetRecord()->TypeName.Text) ?? string.Empty;

    /// <summary>
    /// Receiving the argument record with flags
    /// </summary>
    /// <returns>Record with flags</returns>
    public CvRefModificators Flags => GetRecord()->Flags;

    /// <summary>
    /// Receiving the argument record with binary size
    /// </summary>
    /// <returns>Record with binary size</returns>
    public SizeT BinarySize => GetRecord()->MetaDataForDynamic.BinarySize;

    /// <summary>
    /// Indicates if the argument record is serializable
    /// </summary>
    /// <returns>"True" if the record is serializable</returns>
    public bool IsSerializable => GetRecord()->IsSerializable;

    static internal RttiCollection<RttiArgumentRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiArgumentRecord>(record, imageBase, isValidCxxTypeId);
    }
}
