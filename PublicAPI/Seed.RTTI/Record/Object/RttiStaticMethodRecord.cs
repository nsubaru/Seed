namespace Seed.RTTI.Record.Object;

/// <summary>
/// A base RTTI method record class
/// </summary>
/// <typeparam name="THeap">Heap type</typeparam>
public unsafe class RttiStaticMethodRecord : IRttiIterableObject<RttiStaticMethodRecord>
{
    internal RttiIterator<RttiStaticMethodRecord>? it;

    StaticMethodMetaData* GetRecord() { return (StaticMethodMetaData*)it!.item; }

    /// <summary>
    /// Constructor
    /// </summary>
    public RttiStaticMethodRecord()
    {
        it = null;
    }

    /// <summary>
    /// Constructor from iterator
    /// </summary>
    public RttiStaticMethodRecord(RttiIterator<RttiStaticMethodRecord> it)
    {
        this.it = it;
    }

    /// <summary>
    /// Create instance from iterator
    /// </summary>
    /// <param name="iterator">Object data</param>
    /// <returns>New instance</returns>
    public RttiStaticMethodRecord CreateInstance(RttiIterator<RttiStaticMethodRecord> iterator)
    {
        return new RttiStaticMethodRecord(iterator);
    }

    /// <summary>
    /// Receiving the method arguments
    /// </summary>
    /// <returns>Method arguments</returns>
    internal RttiCollection<RttiArgumentRecord> GetArguments()
    {
        if (GetRecord() != null)
        {
            var collectionBase = (Collection*)((byte*)GetRecord() + GetRecord()->MethodArgsOffset);
            return RttiArgumentRecord.CreateCollection(collectionBase, it!.isValidCxxTypeId, it!.imageBase);
        }
        else
        {
            return RttiArgumentRecord.CreateCollection(null, false, 0);
        }
    }

    /// <summary>
    /// Method name
    /// </summary>
    public string Name => Marshal.PtrToStringAuto((IntPtr)GetRecord()->MethodName.Text) ?? string.Empty;

    /// <summary>
    /// Method ID
    /// </summary>
    public ID Id => it?.idx ?? ID.MaxValue;

    /// <summary>
    /// Method arguments
    /// </summary>
    public RttiCollection<RttiArgumentRecord> Arguments => GetArguments();

    /// <summary>
    /// Is method async (return task)
    /// </summary>
    public bool IsAsync => GetRecord()->IsAsync;

    internal Collection* GetAttributesCollectionBase() => (Collection*)((byte*)GetRecord() + GetRecord()->MethodAttributesOffset);

    /// <summary>
    /// Attributes list
    /// </summary>
    public RttiCollection<RttiAttribute> Attributes => RttiAttribute.CreateCollection(GetAttributesCollectionBase(), it!.isValidCxxTypeId, it!.imageBase);

    /// <summary>
    /// Create collection of objects
    /// </summary>
    /// <param name="record">Collection pointer</param>
    /// <param name="isValidCxxTypeId">True, if CxxTypeInfo available</param>
    /// <param name="imageBase">Rtti section base</param>
    /// <returns>Rtti collection of objects</returns>
    static internal RttiCollection<RttiStaticMethodRecord> CreateCollection(Collection* record, bool isValidCxxTypeId, SizeT imageBase)
    {
        return new RttiCollection<RttiStaticMethodRecord>(record, imageBase, isValidCxxTypeId);
    }
}
