using Seed.RTTI;
using Seed.RTTI.CustomSerializationSupport;
using Seed.RTTI.Record;
using Seed.Serialization;

namespace Seed.Serialization;

/// <summary>
/// Interface for custom class searialization/desearialization.
/// </summary>
public interface ITemplateCustomClassSerializerFactory
{
    /// <summary>
    /// Custom class .Net CLR type
    /// </summary>
    Type GenericClrType { get; }

    /// <summary>
    /// Custom class name in the Seed kernel
    /// </summary>
    string TemplateTypeName { get; }

    TemplateCustomClassSerializerInstantiationBase? BuildSerializerForType(RttiTypeRecord typeRecord, ISeedSerializerFactory seedSerializerFactory, IServiceProvider serviceProvider);
}

public abstract class TemplateCustomClassSerializerInstantiationBase : ICustomClassSerializer
{
    protected IServiceProvider serviceProvider;

    /// <summary>
    /// Custom class .Net CLR type
    /// </summary>
    public Type ClrType { get; }

    /// <summary>
    /// Custom class name in the Seed kernel
    /// </summary>
    public string TypeName { get; }

    /// <summary>
    /// Serializes data into binnary string
    /// </summary>
    /// <param name="data">Data to serialize</param>
    /// <param name="serializationStream">Target stream</param>
    public abstract void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer);

    /// <summary>
    /// Deserializes data from binnary string
    /// </summary>
    /// <param name="serializationStream">Data stream</param>
    /// <returns>Deserialized object</returns>
    public abstract object Deserialize(Stream serializationStream, ISeedSerializer serializer);

    public TemplateCustomClassSerializerInstantiationBase(Type clrType, string typename, IServiceProvider serviceProvider)
    {
        ClrType = clrType;
        TypeName = typename;
        this.serviceProvider = serviceProvider;
    }

    /// <summary>
    /// Types that require for this type
    /// </summary>
    public abstract Type[] DependableTypes { get; }
}