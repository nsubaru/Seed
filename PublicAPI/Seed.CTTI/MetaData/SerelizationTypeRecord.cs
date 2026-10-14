namespace Seed.CTTI.MetaData;

/// <summary>
/// Serialization type record
/// </summary>
public struct SerializationTypeRecord
{
    /// <summary>
    /// Type name in the Seed kernel
    /// </summary>
    public string SeedKernelTypeName
    {
        get => SeedKernelTypeShema.TypeName;
        set => SeedKernelTypeShema.TypeName = value;
    }

    /// <summary>
    /// .Net CLR type
    /// </summary>
    public Type ClrType
    {
        get;
        set;
    }

    /// <summary>
    /// Type class in the Seed kernel
    /// </summary>
    public ETypes SeedKernelTypeClass
    {
        get => SeedKernelTypeShema.Type;
        set => SeedKernelTypeShema.Type = value;
    }

    /// <summary>
    /// Type scheme in the Seed kernel
    /// </summary>
    public TypeMetaDataRecord SeedKernelTypeShema
    {
        get;
        set;
    }

    /// <summary>
    /// Full type Id
    /// </summary>
    public Guid FullTypeId
    {
        get;
        set;
    }
}
