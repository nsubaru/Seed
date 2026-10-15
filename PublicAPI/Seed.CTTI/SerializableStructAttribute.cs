using Seed.CTTI.MetaData;

namespace Seed.CTTI;

[AttributeUsage(AttributeTargets.All)]
public class SeedSerializableAttribute : Attribute
{
    public string SeedKernelTypeName { get; private set; }

    public ETypes TypeClass { get; private set; }

    public SeedSerializableAttribute(string seedKernelTypeName, ETypes typeClass)
    {
        SeedKernelTypeName = seedKernelTypeName;
        TypeClass          = typeClass;
    }
}

[AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
public class SerializableStructAttribute : SeedSerializableAttribute
{
    public SerializableStructAttribute(string seedKernelTypeName) : base(seedKernelTypeName, ETypes.Struct)
    {
    }
}

[AttributeUsage(AttributeTargets.Class)]
public class SerializableObjectPropetiesAttribute : SeedSerializableAttribute
{
    public String TargetObjectTypeName { get; private set; }

    public SerializableObjectPropetiesAttribute(string seedKernelTypeName) : base($"Seed::Core::RTTI::ObjectProperties<{seedKernelTypeName}>", ETypes.Struct)
    {
        TargetObjectTypeName = seedKernelTypeName;
    }
}

[AttributeUsage(AttributeTargets.Enum)]
public class SerializableEnumAttribute : SeedSerializableAttribute
{
    public SerializableEnumAttribute(string seedKernelTypeName) : base(seedKernelTypeName, ETypes.Enumeration) {}
}

[AttributeUsage(AttributeTargets.Class | AttributeTargets.Struct)]
public class SerializableObjectAttribute : SeedSerializableAttribute
{
    public SerializableObjectAttribute(string seedKernelTypeName) : base(seedKernelTypeName, ETypes.Struct) {}
}

[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
public class SerializableStructFieldManualAttribute : Attribute
{
    public StructFieldInformation FieldInformation { get; private set; }

    public SerializableStructFieldManualAttribute(string fieldName, string fieldTypeName)
    {
        FieldInformation = new StructFieldInformation
        {
            FieldName     = fieldName,
            FieldTypeName = fieldTypeName
        };
    }
}

[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field)]
public class SerializableStructFieldAttribute : Attribute
{
    public SerializableStructFieldAttribute() {}
}
