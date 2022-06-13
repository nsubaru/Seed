namespace Seed.RTTI.Record.Object;

public unsafe class RttiProperty : RttiIterableObject<RttiProperty>
{
    internal RttiIterator<RttiProperty>? it;

    PropertyRecord* GetRecord() { return (PropertyRecord*)it!.item; }

    public RttiProperty()
    {
        it = null;
    }

    public RttiProperty(RttiIterator<RttiProperty> it)
    {
        this.it = it;
    }

    public RttiProperty CreateInstance(RttiIterator<RttiProperty> iterator)
    {
        return new RttiProperty(iterator);
    }

    public SizeT TypeId => (SizeT)GetRecord()->PropertyType.TypeId.TypeId + it!.imageBase;

    public UniversalTypeIdentificator FullTypeId => GetRecord()->PropertyType.TypeId;

    public ETypes TypeClass => GetRecord()->PropertyType.TypeClass;

    public SizeT BinarySize => GetRecord()->PropertyType.MetaDataForDynamic.BinarySize;

    public TypeModificator TypeModificators => GetRecord()->PropertyType.TypeModificators;

    public string Name => Marshal.PtrToStringAuto((IntPtr) GetRecord()->PropertyType.FieldName.Text) ?? string.Empty;

    internal string GetTypeName()
    {
        var pTypeName = (TextMetaData*)((Byte*)GetRecord()) +
            (SizeT)Marshal.OffsetOf<PropertyRecord>("PropertyType") +
            (SizeT)Marshal.OffsetOf<FieldMetaDataRecord>("FieldName") +
            (SizeT)Marshal.OffsetOf<TextMetaData>("Text") +
            (GetRecord()->PropertyType.FieldName.Size * sizeof(Char));

        return Marshal.PtrToStringAuto((IntPtr) pTypeName->Text) ?? string.Empty;
    }

    public string TypeName => GetTypeName();

    internal RttiCollection<RttiMethodRecord> GetGetters()
    {
        var @base =  (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToGetters);

        return RttiMethodRecord.CreateCollection(@base, it!.isValidCxxTypeId, it!.imageBase);
    }

    public RttiCollection<RttiMethodRecord> Getters => GetGetters();

    internal RttiCollection<RttiMethodRecord> GetSetters()
    {
        var @base = (Collection*) (((Byte*) GetRecord()) + GetRecord()->OffsetToSetters);

        return RttiMethodRecord.CreateCollection(@base, it!.isValidCxxTypeId, it!.imageBase);
    }

    public RttiCollection<RttiMethodRecord> Setters => GetSetters();

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
