using Seed.Core.Functionality;
using Seed.RTTI.CustomSerializationSupport;
using Seed.RTTI.Record;
using System.Linq;

namespace Seed.Serialization.CustomSerializers;

/// <summary>
/// Provides the common mapping and service context for serializers of closed union types.
/// </summary>
/// <param name="type">The closed CLR union type handled by this serializer.</param>
/// <param name="typeName">The native RTTI name of the union being serialized.</param>
/// <param name="serviceProvider">The service provider available to the serializer.</param>
public abstract class BaseUnionTypeObjectSerializer(Type type, string typeName, IServiceProvider serviceProvider) : TemplateCustomClassSerializerInstantiationBase(type, typeName, serviceProvider)
{
}

/// <summary>
/// Serializes a closed union using its active alternative index and payload.
/// </summary>
/// <typeparam name="T">The closed managed union type reconstructed from the payload.</typeparam>
/// <param name="type">The closed CLR union type handled by this serializer.</param>
/// <param name="typeName">The native RTTI name of the union being serialized.</param>
/// <param name="serviceProvider">The service provider available to the serializer.</param>
public class UnionTypeObjectSerializer<T>(Type type, string typeName, IServiceProvider serviceProvider) : BaseUnionTypeObjectSerializer(type, typeName, serviceProvider)
{
    /// <summary>
    /// The closed union’s alternative types in discriminator order.
    /// </summary>
    public override Type[] DependableTypes => this.ClrType.GenericTypeArguments;

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public override object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        SizeT idx = 0;
        object headDyn = idx;
        serializer.Deserialize(ref headDyn, serializationStream);

        idx = (SizeT)headDyn;
        var type = DependableTypes[idx];

        return this.GetType().GetMethod(nameof(FinishDeserialize))!.MakeGenericMethod(type).Invoke(this, [serializationStream, serializer])!;
    }

    /// <summary>
    /// Decodes the selected payload and constructs the enclosing union.
    /// </summary>
    /// <typeparam name="U">The CLR type of the alternative selected by the encoded discriminator.</typeparam>
    /// <param name="serializationStream">The stream positioned at the selected alternative payload.</param>
    /// <param name="serializer">The session serializer used to decode the alternative.</param>
    /// <returns>A closed union instance containing the decoded alternative.</returns>
    public object FinishDeserialize<U>(Stream serializationStream, ISeedSerializer serializer)
    {
        U ret = serializer.CreateObject<U>()!;
        object dataDyn = ret;
        serializer.Deserialize(ref dataDyn, serializationStream);

        ret = (U)dataDyn;

        return Activator.CreateInstance(typeof(T), [ret])!;
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    public override void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        SizeT idx = data.TypeId;
        serializer.Serialize(idx, serializationStream);

        var type = DependableTypes[idx];
        this.GetType().GetMethod(nameof(CallSerialize))!.MakeGenericMethod(type).Invoke(this, [data, serializationStream, serializer]);
    }

    /// <summary>
    /// Writes the active union payload after the discriminator has been written.
    /// </summary>
    /// <typeparam name="U">The CLR type of the active alternative.</typeparam>
    /// <param name="data">The managed union holding the value to write.</param>
    /// <param name="serializationStream">The destination for the active alternative payload.</param>
    /// <param name="serializer">The session serializer used to encode the alternative.</param>
    public void CallSerialize<U>(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        U value = data.GetValue<U>();
        serializer.Serialize(value, serializationStream);
    }
}

/// <summary>
/// Builds serializers for native unions that match a supported managed union arity.
/// </summary>
public abstract class BaseUnionTypeSerializer : ITemplateCustomClassSerializerFactory
{
    /// <summary>
    /// The open CLR generic type used for the supported union arity.
    /// </summary>
    public abstract Type GenericClrType
    {
        get;
    }

    /// <summary>
    /// The native RTTI template name used to select the union factory.
    /// </summary>
    public string TemplateTypeName => "Seed::Core::Functionality::UnionType";

    /// <summary>
    /// Resolves the native alternatives and constructs a serializer for their closed CLR union.
    /// </summary>
    /// <param name="typeRecord">The native union RTTI record and its ordered alternatives.</param>
    /// <param name="seedSerializerFactory">The factory used to resolve the managed types of the alternatives.</param>
    /// <param name="serviceProvider">The service provider passed to the constructed serializer.</param>
    /// <returns>A serializer for the mapped union, or null if the arity or a CLR mapping is unsupported.</returns>
    public TemplateCustomClassSerializerInstantiationBase? BuildSerializerForType(RttiTypeRecord typeRecord, ISeedSerializerFactory seedSerializerFactory, IServiceProvider serviceProvider)
    {
        if ((int)typeRecord.Fields.Count != GenericClrType.GetGenericArguments().Length)
        {
            return null;
        }

        var dependableTypes = typeRecord.Fields.Select(x => seedSerializerFactory.RTTISection.AllRegisteredTypes.First(y => y.FullTypeId == x.FullTypeId)).Select(seedSerializerFactory.BuildClrType).ToArray();

        if (dependableTypes.Any(x => x == null))
        {
            return null;
        }

        return typeof(UnionTypeObjectSerializer<>).MakeGenericType(GenericClrType.MakeGenericType(dependableTypes!)).GetConstructors().First().Invoke([GenericClrType.MakeGenericType(dependableTypes!), typeRecord.TypeName, serviceProvider]) as BaseUnionTypeObjectSerializer;
    }
}

/// <summary>
/// Creates serializers for the open managed union with 1 alternative.
/// </summary>
public class UnionTypeSerializerT0 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 1 alternative.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<>);
}

/// <summary>
/// Creates serializers for the open managed union with 2 alternatives.
/// </summary>
public class UnionTypeSerializerT1 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 2 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,>);
}

/// <summary>
/// Creates serializers for the open managed union with 3 alternatives.
/// </summary>
public class UnionTypeSerializerT2 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 3 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,>);
}

/// <summary>
/// Creates serializers for the open managed union with 4 alternatives.
/// </summary>
public class UnionTypeSerializerT3 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 4 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,,>);
}

/// <summary>
/// Creates serializers for the open managed union with 5 alternatives.
/// </summary>
public class UnionTypeSerializerT4 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 5 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,,,>);
}

/// <summary>
/// Creates serializers for the open managed union with 5 alternatives.
/// </summary>
public class UnionTypeSerializerT5 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 5 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,,,>);
}

/// <summary>
/// Creates serializers for the open managed union with 6 alternatives.
/// </summary>
public class UnionTypeSerializerT6 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 6 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,,,,>);
}

/// <summary>
/// Creates serializers for the open managed union with 7 alternatives.
/// </summary>
public class UnionTypeSerializerT7 : BaseUnionTypeSerializer
{
    /// <summary>
    /// The open managed union type with 7 alternatives.
    /// </summary>
    public override Type GenericClrType => typeof(UnionType<,,,,,,>);
}
