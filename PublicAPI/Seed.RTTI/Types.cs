using Seed.Core.Configurations;
using Seed.Core.RTTI;

namespace Seed.RTTI;
//HACK: Enum values is incorrect,
//      because Seed kernel EnumFlags use item index starting from 1 as index of bit
//      whereas C# [Flags] attributed enum use enum value as bit mask
[Flags]
public enum TypeModificator : byte
{
    Serializable = 1,
    TemplateType = 2
}

[Flags]
public enum CvRefModificators : SizeT
{
    Ref = 1,
    RValueRef = 2,
    Const = 4,
    Volatile = 8
}

[SerializableEnum("Seed::Core::CTTI::TypeTraits::ECustomTypeSchemaFormat")]
public enum ECustomTypeSchemaFormat : byte
{
    None,
    StructSchema,
    StructWithAccessorMethodSchema,
    ObjectSchema,
    RemoteStructSchema,
    RemoteObjectSchema,
    Undefined
}

/// <summary>
/// Possible CTTI attribute targets
/// </summary>
[SerializableEnum("Seed::Core::CTTI::TypeTraits::EAttributeTargetType")]
public enum EAttributeTargetType : UInt8
{
    /// <summary>
    /// Type itself is attribute target (e.g. attribute is class attribute)
    /// </summary>
    Type,
    /// <summary>
    /// Method is attribute target (e.g. attribute is method attribute)
    /// </summary>
    Method,
    /// <summary>
    /// Function or static method is attribute target (e.g. attribute is function attribute or attribute is static method attribute)
    /// </summary>
    Function,
    /// <summary>
    /// Field or property is attribute target (e.g. attribute is field attribute or attribute is property attribute)
    /// </summary>
    Field,
    /// <summary>
    /// Unknown attribute target
    /// </summary>
    Unknown = UInt8.MaxValue
};
/// <summary>
/// Attribute value category, can be data, text or type
/// <para> None - attribute value unavailable </para>
/// <para> Data - is any trivial copy type </para>
/// <para> Text - is string </para>
/// <para> AsciiText - is Ascii string </para>
/// <para> Type - is type information, value is FullTypeId of type, also store reference to RttiTypeRecord </para>
/// </summary>
public enum EAttributeValueCategories : UInt8
{
    None,
    Data,
    Text,
    AsciiText,
    Type
};
[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct TextMetaData
{
    [FieldOffset(0)]
    public SizeT Size;
    
    [FieldOffset(8)]
    public fixed Char Text[1];
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct MetaDataForDynamicRecord
{
    [FieldOffset(0)]
    public TEmptyCtor EmptyConstruct;

    [FieldOffset(8)]
    public TCopyCtor CopyCtor;

    [FieldOffset(16)]
    public TMoveCtor MoveCtor;

    [FieldOffset(24)]
    public TDtor Dtor;

    [FieldOffset(32)]
    public TGetRttiRecord GetRttiRecord;

    [FieldOffset(40)]
    public SizeT BinarySize;

    [FieldOffset(48)]
    public TSerialize FSerialize;

    [FieldOffset(56)]
    public TDeserialize FDeserialize;

    [FieldOffset(64)]
    public TSerializationSize FSerializationSize;

    [FieldOffset(72)]
    public TUpdateSerialize FUpdateSerialization;

    [FieldOffset(80)]
    public TSerializationMetadata FSerializationMetadata;

    [FieldOffset(88)]
    public TToString FToString;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct MetaData
{
    [FieldOffset(0)]
    public SizeT OffsetToCttiMetaData;

    [FieldOffset(8)]
    public SizeT OffsetToAttributes;

    [FieldOffset(16)]
    public UniversalTypeIdentifier TypeId;

    [FieldOffset(32)]
    public ETypes TypeClass;

    [FieldOffset(33)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(129)]
    public TypeModificator TypeModificators;

    [FieldOffset(130)]
    public TextMetaData TypeName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
unsafe struct StructMetaData
{
    [FieldOffset(0)]
    public SizeT OffsetToCollection;

    [FieldOffset(8)]
    public TUnsafeGet BaseGetter;

    [FieldOffset(16)]
    public Boolean IsFake;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct Collection
{
    [FieldOffset(0)]
    public SizeT OffsetToFirstItem;

    [FieldOffset(8)]
    public SizeT Count;

    [FieldOffset(16)]
    public SizeT SizeOfCollection;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct DynamicSizedCollection
{
    [FieldOffset(0)]
    public SizeT OffsetToFirstItem;

    [FieldOffset(8)]
    public TDynamicForEach DynamicForEach;
    
    [FieldOffset(16)]
    public TUnsafeGet SizeGetter;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct FixedSizedCollection
{
    [FieldOffset(0)]
    public SizeT OffsetToFirstItem;

    [FieldOffset(8)]
    public TDynamicForEach DynamicForEach;

    [FieldOffset(16)]
    public SizeT CountOfItems;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct AssociativeCollection
{
    [FieldOffset(0)]
    public SizeT OffsetToFirstItem;

    [FieldOffset(8)]
    public TDynamicForEach DynamicForEach;

    [FieldOffset(16)]
    public TUnsafeGet SizeGetter;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct PairMetaData
{
    [FieldOffset(0)]
    public SizeT OffsetToFirstItem;

    [FieldOffset(8)]
    public TPairComponentUnsafeGet KeyGetter;

    [FieldOffset(16)]
    public TPairComponentUnsafeGet ValueGetter;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ItemBase
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct FieldMetaDataRecord
{
    [FieldOffset(0)]
    public SizeT OffsetToAttributesList;

    [FieldOffset(8)]
    public UniversalTypeIdentifier TypeId;

    [FieldOffset(24)]
    public ETypes TypeClass;

    [FieldOffset(25)]
    public SizeT Offset;

    [FieldOffset(33)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(129)]
    public TypeModificator TypeModificators;

    [FieldOffset(130)]
    public TextMetaData FieldName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct FieldMetaData
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public SizeT OffsetToAttributesList;

    [FieldOffset(16)]
    public UniversalTypeIdentifier TypeId;

    [FieldOffset(32)]
    public ETypes TypeClass;

    [FieldOffset(33)]
    public SizeT Offset;

    [FieldOffset(41)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(137)]
    public TypeModificator TypeModificators;

    [FieldOffset(138)]
    public TextMetaData FieldName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ItemMetaData
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public UniversalTypeIdentifier TypeId;

    [FieldOffset(24)]
    public ETypes TypeClass;

    [FieldOffset(25)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(121)]
    public TypeModificator TypeModificators;

    [FieldOffset(122)]
    public TextMetaData TypeName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ArgumentMetaData
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public UniversalTypeIdentifier TypeId;
    
    [FieldOffset(24)]
    public ETypes TypeClass;

    [FieldOffset(25)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(121)]
    public Boolean IsSerializable;

    [FieldOffset(122)]
    public CvRefModificators Flags;

    [FieldOffset(130)]
    public TextMetaData TypeName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct Constructor
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public TGetRttiRecord GetRttiRecord;

    [FieldOffset(16)]
    public TDynamicCtorCaller DynamicCtorCaller;

    [FieldOffset(24)]
    public SizeT OffsetToArgs;

    [FieldOffset(32)]
    public Collection Args;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ObjectMetaData
{
    [FieldOffset(0)]
    public SizeT ConstructorsOffset;

    [FieldOffset(8)]
    public SizeT InterfacesOffset;

    [FieldOffset(16)]
    public SizeT PropertiesOffset;

    [FieldOffset(24)]
    public SizeT MethodsOffset;

    [FieldOffset(32)]
    public SizeT StaticMethodsOffset;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct InterfaceRecord
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public UniversalTypeIdentifier TypeId;

    [FieldOffset(24)]
    public MetaDataForDynamicRecord MetaDataForDynamic;

    [FieldOffset(120)]
    public DynamicConverterFunc DynamicConverterTo;

    [FieldOffset(128)]
    public DynamicConverterFunc DynamicConverterFrom;

    [FieldOffset(136)]
    public TextMetaData TypeName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct MethodMetaData
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public SizeT MethodArgsOffset;

    [FieldOffset(16)]
    public SizeT MethodAttributesOffset;

    [FieldOffset(24)]
    public CvRefModificators MethodCVRefModificators;

    [FieldOffset(32)]
    public bool IsAsync;

    [FieldOffset(33)]
    public DynamicCallerFunc DynamicCaller;

    [FieldOffset(41)]
    public DynamicCallerFunc AsyncDynamicCaller;

    [FieldOffset(49)]
    public TextMetaData MethodName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct StaticMethodMetaData
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public SizeT MethodArgsOffset;

    [FieldOffset(16)]
    public SizeT MethodAttributesOffset;

    [FieldOffset(24)]
    public bool IsAsync;

    [FieldOffset(25)]
    public StaticDynamicCallerFunc DynamicCaller;

    [FieldOffset(33)]
    public StaticDynamicCallerFunc AsyncDynamicCaller;

    [FieldOffset(41)]
    public TextMetaData MethodName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct PropertyRecord
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;

    [FieldOffset(8)]
    public SizeT OffsetToGetters;

    [FieldOffset(16)]
    public SizeT OffsetToSetters;

    [FieldOffset(24)]
    public SizeT OffsetToAttributes;

    [FieldOffset(32)]
    public TUnsafeGet UnsafeGet;

    [FieldOffset(40)]
    public FieldMetaDataRecord PropertyType;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct CustomTypeMetaData
{
    [FieldOffset(0)]
    public ECustomTypeSchemaFormat SchemaFormat;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct StructCustomTypeMetaData
{
    [FieldOffset(0)]
    public ECustomTypeSchemaFormat SchemaFormat;

    [FieldOffset(1)]
    public SizeT OffsetToCollection;

    [FieldOffset(9)]
    public TUnsafeGet BaseGetter;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ObjectPropertiesMetaData
{
    [FieldOffset(0)]
    public SizeT OffsetToCollection;

    [FieldOffset(8)]
    public TUnsafeGet BaseGetter;
    
    [FieldOffset(16)]
    public Boolean IsFake;

    [FieldOffset(24)]
    public SizeT PropertiesCount;
    
    [FieldOffset(32)]
    public TObjectPropertiesGetObjId GetObjId;

    [FieldOffset(40)]
    public TObjectPropertiesForEach ForEach;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct NumericMetaData
{
    [FieldOffset(0)]
    public Boolean IsEnumFlag;

    [FieldOffset(1)]
    public Boolean IsSigned;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct NumericEnumFlagMetaData
{
    [FieldOffset(0)]
    public Boolean IsEnumFlag;

    [FieldOffset(1)]
    public Boolean IsSigned;

    [FieldOffset(2)]
    public Collection InnerTypes;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct ObjectCustomTypeMetaData
{
    [FieldOffset(0)]
    public ECustomTypeSchemaFormat SchemaFormat;

    [FieldOffset(1)]
    public SizeT ConstructorsOffset;

    [FieldOffset(9)]
    public SizeT InterfacesOffset;

    [FieldOffset(17)]
    public SizeT PropertiesOffset;

    [FieldOffset(25)]
    public SizeT MethodsOffset;

    [FieldOffset(33)]
    public SizeT StaticMethodsOffset;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Unicode, Pack = 0)]
internal unsafe struct AttributeRecord
{
    [FieldOffset(0)]
    public SizeT SizeOfItem;
    
    [FieldOffset(8)]
    public SizeT OffsetToAttributeName;

    [FieldOffset(16)]
    public SizeT OffsetToAttributeValueTypeName;

    [FieldOffset(24)]
    public SizeT OffsetToAttributeValue;

    [FieldOffset(32)]
    public EAttributeTargetType AttributeTargetType;

    [FieldOffset(33)]
    public EAttributeValueCategories ValueCategory;

    [FieldOffset(34)]
    public MetaDataForDynamicRecord ValueTypeInformation;

    [FieldOffset(130)]
    public UniversalTypeIdentifier ValueTypeId;

    [FieldOffset(146)]
    public TextMetaData AttributeName;
}

[StructLayout(LayoutKind.Explicit, CharSet = CharSet.Ansi, Pack = 0)]
internal unsafe struct SectionHeaderType
{
    [FieldOffset(0)]
    public fixed byte SectionMagicNumber[14];

    [FieldOffset(14)]
    public KernelVersionT KernelVersion;

    [FieldOffset(22)]
    public RawMemBlk OriginHeaderAddr;

    [FieldOffset(30)]
    public RawMemBlk FaAddr;

    [FieldOffset(38)]
    public RawMemBlk FzAddr;
    
    [FieldOffset(46)]
    public RawMemBlk VaAddr;

    [FieldOffset(54)]
    public RawMemBlk VzAddr;

    [FieldOffset(62)]
    public RawMemBlk ImageBase;
}

public struct SectionDetails
{
    public SizeT SectionSize { get; set; }

    public SizeT SectionHeaderOffset { get; set; }

    public KernelVersionT KernelVersion { get; set; }

    public RawMemBlk OriginHeaderAddr { get; set; }

    public RawMemBlk FaAddr { get; set; }

    public RawMemBlk FzAddr { get; set; }

    public RawMemBlk VaAddr { get; set; }

    public RawMemBlk VzAddr { get; set; }

    public RawMemBlk ImageBase { get; set; }

    public SizeT Offset { get; set; }

    public SizeT TotalCount { get; set; }

    public SizeT ValidCount { get; set; }
}
