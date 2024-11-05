using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.RTTI.Record;

public unsafe class RttiAttribute : IRttiIterableObject<RttiAttribute>
{
    internal RttiIterator<RttiAttribute>? it;

    internal AttributeRecord* GetRecord() { return (AttributeRecord*)it!.item; }

    /// <summary>
    /// Constructor
    /// </summary>
    public RttiAttribute()
    {
        it = null;
    }

    /// <summary>
    /// Constructor
    /// </summary>
    /// <param name="it">Iterator</param>
    public RttiAttribute(RttiIterator<RttiAttribute> it)
    {
        this.it = it;
    }

    /// <summary>
    /// Create instance from iterator
    /// </summary>
    /// <param name="iterator">Object data</param>
    /// <returns>New instance</returns>
    public RttiAttribute CreateInstance(RttiIterator<RttiAttribute> iterator)
    {
        return new RttiAttribute(iterator);
    }

    /// <summary>
    /// Value type ID
    /// </summary>
    public ID TypeId => GetRecord()->ValueTypeId.TypeId;

    /// <summary>
    /// Value full type ID
    /// </summary>
    public UniversalTypeIdentifier FullTypeId => GetRecord()->ValueTypeId;

    /// <summary>
    /// Value size (in bytes)
    /// </summary>
    public SizeT BinnarySize => GetRecord()->ValueTypeInformation.BinarySize;

    /// <summary>
    /// Value type name
    /// </summary>
    public String TypeName => GetTypeName();

    /// <summary>
    /// Attribute name
    /// </summary>
    public String Name => Marshal.PtrToStringAuto((IntPtr) GetRecord()->AttributeName.Text) ?? string.Empty;

    /// <summary>
    /// Attribute target type
    /// </summary>
    public EAttributeTargetType AttributeTargetType => GetRecord()->AttributeTargetType;

    /// <summary>
    /// Attribute value category
    /// </summary>
    public EAttributeValueCategoriess ValueCategory => GetRecord()->ValueCategory;

    internal String GetTypeName()
    {
        var text = (TextMetaData*)((Byte*)GetRecord() + GetRecord()->OffsetToAttributeValueTypeName);
        return Marshal.PtrToStringAuto((IntPtr)text->Text) ?? string.Empty;
    }

    internal static RttiCollection<RttiAttribute> CreateCollection(Collection* @base, bool isValidCxxTypeId, ulong imageBase)
    {
        return new RttiCollection<RttiAttribute>(@base, imageBase, isValidCxxTypeId);
    }
}
