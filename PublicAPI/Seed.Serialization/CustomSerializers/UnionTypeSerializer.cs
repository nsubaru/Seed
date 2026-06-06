using Seed.Core.Functionality;
using Seed.RTTI.CustomSerializationSupport;
using Seed.RTTI.Record;
using System.Linq;

namespace Seed.Serialization.CustomSerializers;

public abstract class BaseUnionTypeObjectSerializer(Type type, string typeName, IServiceProvider serviceProvider) : TemplateCustomClassSerializerInstantiationBase(type, typeName, serviceProvider) { }

public class UnionTypeObjectSerializer<T>(Type type, string typeName, IServiceProvider serviceProvider) : BaseUnionTypeObjectSerializer(type, typeName, serviceProvider)
{
    public override Type[] DependableTypes => this.ClrType.GenericTypeArguments;

    public override object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        SizeT idx = 0;
        dynamic headDyn = idx;

        serializer.Deserialize(ref headDyn, serializationStream);

        idx = headDyn;
        var type = DependableTypes[idx];

        return this.GetType()
            .GetMethod(nameof(FinishDeserialize))!
            .MakeGenericMethod(type)
            .Invoke(this, [serializationStream, serializer])!;
    }

    public object FinishDeserialize<U>(Stream serializationStream, ISeedSerializer serializer)
    {
        U ret = serializer.CreateObject<U>()!;
        dynamic dataDyn = ret;
        serializer.Deserialize(ref dataDyn, serializationStream);
        ret = dataDyn;

        return Activator.CreateInstance(typeof(T), [ret])!;
    }

    public override void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        SizeT idx = data.TypeId;

        serializer.Serialize(idx, serializationStream);

        var type = DependableTypes[idx];

        this.GetType()
            .GetMethod(nameof(CallSerialize))!
            .MakeGenericMethod(type)
            .Invoke(this, [data, serializationStream, serializer]);
    }

    public void CallSerialize<U>(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        serializer.Serialize(data.GetValue<U>(), serializationStream);
    }
}

public abstract class BaseUnionTypeSerializer : ITemplateCustomClassSerializerFactory
{
    public abstract Type GenericClrType { get; }

    public string TemplateTypeName => "Seed::Core::Functionality::UnionType";

    public TemplateCustomClassSerializerInstantiationBase? BuildSerializerForType(RttiTypeRecord typeRecord, ISeedSerializerFactory seedSerializerFactory, IServiceProvider serviceProvider)
    {
        if ((int) typeRecord.Fields.Count != GenericClrType.GetGenericArguments().Length)
        {
            return null;
        }

        var dependableTypes = typeRecord.Fields
            .Select(x => seedSerializerFactory.RTTISection.AllRegisteredTypes.First(y => y.FullTypeId == x.FullTypeId))
            .Select(seedSerializerFactory.BuildClrType)
            .ToArray();

        if (dependableTypes.Any(x => x == null))
        {
            return null;
        }

        return typeof(UnionTypeObjectSerializer<>)
            .MakeGenericType(GenericClrType.MakeGenericType(dependableTypes!))
            .GetConstructors()
            .First()
            .Invoke([GenericClrType.MakeGenericType(dependableTypes!), typeRecord.TypeName, serviceProvider]) as BaseUnionTypeObjectSerializer;
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