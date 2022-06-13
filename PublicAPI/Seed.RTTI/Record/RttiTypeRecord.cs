using Seed.RTTI.Record.Object;

namespace Seed.RTTI.Record;

public unsafe class RttiTypeRecord : IComparable<RttiTypeRecord>
{
    private MetaData* record;
    private SizeT imageBase;
    private bool isValidCxxTypeId;

    internal RttiTypeRecord(MetaData* record, ulong imageBase, bool isValidCxxTypeId)
    {
        this.record = record;
        this.imageBase = imageBase;
        this.isValidCxxTypeId = isValidCxxTypeId;
    }

    public RttiTypeRecord()
    {
        this.record = null;
        this.imageBase = 0;
        this.isValidCxxTypeId = false;
    }

    public bool IsValid => record != null;

    public bool IsValidCxxTypeId => isValidCxxTypeId;

    public ID TypeId => record->TypeId.TypeId + imageBase;

    public UniversalTypeIdentificator FullTypeId => record == null ? new UniversalTypeIdentificator() : record->TypeId;

    public ETypes TypeClass => record == null ? ETypes.Undefine : record->TypeClass;

    public SizeT BinarySize => record->MetaDataForDynamic.BinarySize;

    public TypeModificator TypeModificators => record->TypeModificators;

    public string TypeName => Marshal.PtrToStringAuto((IntPtr) record->TypeName.Text) ?? string.Empty;

    internal string GetCxxTypeName()
    {
        var pTypeName = (TextMetaData*)(
             ((Byte*)record) +
             (SizeT)Marshal.OffsetOf<MetaData>("TypeName") + (SizeT)Marshal.OffsetOf<TextMetaData>("Text") +
             (record->TypeName.Size * sizeof(Char))
        );

        return Marshal.PtrToStringAuto((IntPtr) pTypeName->Text) ?? string.Empty;
    }

    public string CxxTypeName => GetCxxTypeName();

    internal string GetSerializationSchema()
    {
        var pTypeName = (TextMetaData*)(
             ((Byte*)record) +
             (SizeT)Marshal.OffsetOf<MetaData>("TypeName") + (SizeT)Marshal.OffsetOf<TextMetaData>("Text") +
             (record->TypeName.Size * sizeof(Char))
        );

        var pSchema = (TextMetaData*)(((Byte*) pTypeName->Text) + (pTypeName->Size * sizeof(Char)));

        return Marshal.PtrToStringAuto((IntPtr) pSchema->Text) ?? string.Empty;
    }

    public string SerializationSchema => GetSerializationSchema();

    private SizeT GetCTTIMetaDataOffset()
    {
        return record->OffsetToCttiMetaData;
    }

    private SizeT GetItemsCollectionOffset()
    {
        return GetCTTIMetaDataOffset() + *(SizeT*) (((Byte*)record) + GetCTTIMetaDataOffset());
    }

    private SizeT GetCustomCttiTypeStrcutSchemaRttiOffset()
    {
        var metaData = (StructCustomTypeMetaData*) (((Byte*)record) + GetCTTIMetaDataOffset());

        return  (
                    metaData->SchemaFormat == ECustomTypeSchemaFormat.StructSchema ||
                    metaData->SchemaFormat == ECustomTypeSchemaFormat.RemoteStructSchema
                )
                ? metaData->OffsetToCollection
                : 0;
    }

    private SizeT GetFieldsRecordsOffset()
    {
        switch (record->TypeClass)
        {
            case ETypes.Tuple:
                return GetItemsCollectionOffset();

            case ETypes.Enumeration:
                return GetItemsCollectionOffset();

            case ETypes.FixedArray:
                return GetItemsCollectionOffset();

            case ETypes.Struct:
                return GetItemsCollectionOffset();

            case ETypes.Union:
                return GetItemsCollectionOffset();

            case ETypes.Collection:
                return GetItemsCollectionOffset();

            case ETypes.String:
            case ETypes.Pointer:
            case ETypes.Box:
            case ETypes.Pair:
                return GetItemsCollectionOffset();

            case ETypes.FixedCollection:
                return GetItemsCollectionOffset();

            case ETypes.AssociableCollection:
            case ETypes.Reference:
                return GetItemsCollectionOffset();

            case ETypes.Custom:
                return GetCustomCttiTypeStrcutSchemaRttiOffset();

            case ETypes.Object:

            default:
                return 0;
        }
    }

    private RttiCollection<RttiField> GetFields()
    {
        var offset = GetFieldsRecordsOffset();

        if (offset == 0)
        {
            return RttiField.CreateCollection(null, IsValidCxxTypeId, imageBase);
        }
        else
        {
            return RttiField.CreateCollection((Collection*)(((Byte*) record) + offset), IsValidCxxTypeId, imageBase);
        }
    }

    public RttiCollection<RttiField> Fields => GetFields();

    private ObjectMetaData* GetObjectRecord()
    {
        return (ObjectMetaData*)(((Byte*)record) + GetCTTIMetaDataOffset());
    }

    private ObjectCustomTypeMetaData* GetCustomTypeRecord()
    {
        return (ObjectCustomTypeMetaData*)(((Byte*)record) + GetCTTIMetaDataOffset());
    }

    private RttiCollection<RttiConstructorRecord> GetConstructors()
    {
        if(record->TypeClass == ETypes.Object ||record->TypeClass == ETypes.Abstract)
        {
            var objRecord = GetObjectRecord();

            return RttiConstructorRecord.CreateCollection(
                (Collection*) (((Byte*)objRecord) + objRecord->ConstructorsOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else if (record->TypeClass == ETypes.Custom &&
            (
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.ObjectSchema ||
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.RemoteObjectSchema
            ))
        {
            var customObjRecord = GetCustomTypeRecord();

            return RttiConstructorRecord.CreateCollection(
                (Collection*)(((Byte*)customObjRecord) + customObjRecord->ConstructorsOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else
        {
            return RttiConstructorRecord.CreateCollection(null, isValidCxxTypeId, imageBase);
        }
    }

    private RttiCollection<RttiIntefaceRecord> GetInterfaces()
    {
        if (record->TypeClass == ETypes.Object || record->TypeClass == ETypes.Abstract)
        {
            var objRecord = GetObjectRecord();

            return RttiIntefaceRecord.CreateCollection(
                (Collection*)(((Byte*)objRecord) + objRecord->InterfacesOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else if (record->TypeClass == ETypes.Custom &&
            (
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.ObjectSchema ||
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.RemoteObjectSchema
            ))
        {
            var customObjRecord = GetCustomTypeRecord();

            return RttiIntefaceRecord.CreateCollection(
                (Collection*)(((Byte*)customObjRecord) + customObjRecord->InterfacesOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else
        {
            return RttiIntefaceRecord.CreateCollection(null, isValidCxxTypeId, imageBase);
        }
    }

    private RttiCollection<RttiProperty> GetProperties()
    {
        if (record->TypeClass == ETypes.Object || record->TypeClass == ETypes.Abstract)
        {
            var objRecord = GetObjectRecord();

            return RttiProperty.CreateCollection(
                (Collection*)(((Byte*)objRecord) + objRecord->PropertiesOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else if (record->TypeClass == ETypes.Custom &&
            (
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.ObjectSchema ||
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.RemoteObjectSchema
            ))
        {
            var customObjRecord = GetCustomTypeRecord();

            return RttiProperty.CreateCollection(
                (Collection*)(((Byte*)customObjRecord) + customObjRecord->PropertiesOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else
        {
            return RttiProperty.CreateCollection(null, isValidCxxTypeId, imageBase);
        }
    }

    private RttiCollection<RttiMethodRecord> GetMethods()
    {
        if (record->TypeClass == ETypes.Object || record->TypeClass == ETypes.Abstract)
        {
            var objRecord = GetObjectRecord();

            return RttiMethodRecord.CreateCollection(
                (Collection*)(((Byte*)objRecord) + objRecord->MethodsOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else if (record->TypeClass == ETypes.Custom &&
            (
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.ObjectSchema ||
                GetCustomTypeRecord()->SchemaFormat == ECustomTypeSchemaFormat.RemoteObjectSchema
            ))
        {
            var customObjRecord = GetCustomTypeRecord();

            return RttiMethodRecord.CreateCollection(
                (Collection*)(((Byte*)customObjRecord) + customObjRecord->MethodsOffset),
                isValidCxxTypeId,
                imageBase);
        }
        else
        {
            return RttiMethodRecord.CreateCollection(null, isValidCxxTypeId, imageBase);
        }
    }

    public RttiCollection<RttiConstructorRecord> Constructors => GetConstructors();

    public RttiCollection<RttiIntefaceRecord> Interfaces => GetInterfaces();

    public RttiCollection<RttiProperty> Properties => GetProperties();

    public RttiCollection<RttiMethodRecord> Methods => GetMethods();

    private ECustomTypeSchemaFormat GetCustomTypeSchemaFormat()
    {
        if (record->TypeClass == ETypes.Custom)
        {
            var customObjRecord = GetCustomTypeRecord();

            return customObjRecord->SchemaFormat;
        }
        else
        {
            return ECustomTypeSchemaFormat.Undefined;
        }
    }

    public ECustomTypeSchemaFormat CustomTypeSchemaFormat => GetCustomTypeSchemaFormat();

    private SizeT GetArrayItemsCount()
    {
        if (record->TypeClass == ETypes.FixedArray || record->TypeClass == ETypes.FixedCollection)
        {
            var collectionRecord = (Collection*)(((Byte*) record) + GetCTTIMetaDataOffset());

            return collectionRecord->Count;
        }
        else
        {
            return 0;
        }
    }

    public SizeT ArrayItemsCount => GetArrayItemsCount();

    public int CompareTo(RttiTypeRecord? other)
    {
        if (other == null)
            return 1;

        if (this.record == other.record)
            return 0;

        if (this.record == null)
            return -1;
        else if (other.record == null)
            return 1;

        if (isValidCxxTypeId && other.isValidCxxTypeId)
            return this.record->TypeId.Id.CompareTo(other.record->TypeId.Id);
        else
            return this.CxxTypeName.CompareTo(other.CxxTypeName);
    }
}
