using Seed.Core.Functionality;
using Seed.RTTI.CustomSerializationSupport;
using Seed.RTTI.Record;
using System.Linq;

namespace Seed.Serialization.CustomSerializers;

public abstract class BaseUnionTypeObjectSerializer(Type type, string typeName, IServiceProvider serviceProvider) : TemplateCustomClassSerializerInstantiationBase(type, typeName, serviceProvider) {}

public class UnionTypeObjectSerializer<T>(Type type, string typeName, IServiceProvider serviceProvider) : BaseUnionTypeObjectSerializer(type, typeName, serviceProvider)
{
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

    public void CallSerialize<U>(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        U value = data.GetValue<U>();
        serializer.Serialize(value, serializationStream);
    }
}

public abstract class BaseUnionTypeSerializer : ITemplateCustomClassSerializerFactory
{
    public abstract Type GenericClrType { get; }

    public string TemplateTypeName => "Seed::Core::Functionality::UnionType";

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

public class UnionTypeSerializerT0 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<>);
}

public class UnionTypeSerializerT1 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,>);
}

public class UnionTypeSerializerT2 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,>);
}

public class UnionTypeSerializerT3 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,,>);
}

public class UnionTypeSerializerT4 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,,,>);
}

public class UnionTypeSerializerT5 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,,,>);
}

public class UnionTypeSerializerT6 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,,,,>);
}

public class UnionTypeSerializerT7 : BaseUnionTypeSerializer
{
    public override Type GenericClrType => typeof(UnionType<,,,,,,>);
}
