using System.Reflection;
using Seed.RTTI;
using Seed.Serialization;
using Seed.RTTI.Record;
using System.Collections;
using Microsoft.Extensions.Logging;
using System.Runtime.CompilerServices;
using System.Diagnostics.Contracts;

namespace Seed.CTTI;

public class SeedBinarySerializator : ISeedBinarySerializator
{
    private readonly RTTISection rttiSection;
    private readonly ISeedSerializator seedSerializator;
    private readonly Dictionary<RttiTypeRecord, Type> mapingSheme = new();

    private readonly ILogger<SeedBinarySerializator>? logger;

    public SeedBinarySerializator(
        RTTISection rttiSection,
        ILogger<SeedBinarySerializator>? logger = null, 
        ILogger<SeedSerializatorFactory>? seedSerializatorFactory = null,
        ILogger<ISeedSerializator>? seedSerializatorImplLogger = null)
    {
        this.rttiSection = rttiSection;
        this.logger = logger;

        logger?.LogInformation("Create serializator factory");
        SeedSerializatorFactory serializatorFactory = new SeedSerializatorFactory(rttiSection, seedSerializatorFactory, seedSerializatorImplLogger);

        logger?.LogInformation("Retrieve serializator implementation");
        seedSerializator = serializatorFactory.GetSeedSerializator();

        logger?.LogInformation("Load supported types from serializator implementation");
        mapingSheme = seedSerializator
            .GetSupportedTypes()
            .OrderBy(x => x.RttiType.TypeClass)
            .OrderBy(x => x.RttiType.TypeName)
            .ToDictionary(x => x.RttiType, x => x.ClrType);
    }

    private void SerializeImpl<T>(Stream serializationStream, T graph)
    {
        seedSerializator.Serialize(graph!, serializationStream);
    }

    private void SerializeArrayImpl<T>(Stream serializationStream, T graph, RttiTypeRecord record)
    {
        switch (record.TypeClass)
        {
            case ETypes.FixedCollection:
            {
                var mirror = graph as IList;
                if (mirror!.Count > (int)record.ArrayItemsCount)
                {
                    throw new InvalidCastException();
                }
                break;
            }
            case ETypes.FixedArray:
            {
                var mirror = graph as Array;
                if (mirror!.Length != (int)record.ArrayItemsCount)
                {
                    throw new InvalidCastException();
                }
                break;
            }
        }

        SerializeImpl<T>(serializationStream, graph);
    }

    public void Serialize(Stream serializationStream, object graph, RttiTypeRecord? targetType = null)
    {
        if (graph == null)
        {
            throw new NullReferenceException();
        }

        logger?.LogInformation("Start serializing Seed type {0} with clr type {1}", targetType?.TypeName ?? "null", graph.GetType().FullName);

        Type? type = null;
        UniversalTypeIdentifier Id = new UniversalTypeIdentifier();

        if (targetType != null)
        {
            if (!mapingSheme.ContainsKey(targetType))
            {
                logger?.LogError("Serialize failed. Unknown type");
                throw new ArgumentException("Unknown type");
            }
            else if (mapingSheme.ContainsKey(targetType) && mapingSheme[targetType] == graph.GetType())
            {
                Id = targetType.FullTypeId;
                type = mapingSheme[targetType];
            }
        }
        else if (mapingSheme.ContainsValue(graph.GetType()))
        {
            var rec = mapingSheme.Where(x => x.Value == graph.GetType()).First();
            Id = rec.Key.FullTypeId;
            type = rec.Value;
        }
        else
        {
            logger?.LogError("Serialize failed. Unknown type");
            throw new InvalidCastException();
        }

        logger?.LogDebug("Call serializator implementation for serialize full type id");

        seedSerializator.Serialize(Id, serializationStream);

        var record = rttiSection.AllRegisteredTypes.First(x => x.FullTypeId == Id);

        logger?.LogDebug("Call serializator implementation for serialize data");

        try
        {
            if (record.TypeClass == ETypes.FixedArray || record.TypeClass == ETypes.FixedCollection)
            {
                GetType().GetRuntimeMethods().Where(x => x.Name == "SerializeArrayImpl")
                    .First()
                    .MakeGenericMethod(type!)
                    .Invoke(this, new object[] { serializationStream, graph, record });
            }
            else
            {
                GetType().GetRuntimeMethods().Where(x => x.Name == "SerializeImpl")
                    .First()
                    .MakeGenericMethod(type!)
                    .Invoke(this, new object[] { serializationStream, graph });
            }
        }
        catch(Exception ex)
        {
            logger?.LogError("Serialization failed with exception: {0}", ex.ToString());
            throw;
        }

        logger?.LogInformation("Serialization successfully completed");
    }

    private T CreateObject<T>(Stream serializationStream)
    {
        Type type = typeof(T);

        object? obj = null;

        if (type == typeof(String))
        {
            obj = string.Empty;
        }
        else if (type == typeof(DynamicObject))
        {
            UniversalTypeIdentifier Id = new();
            dynamic dynId = Id;

            var streamStartPos = serializationStream.Position;
            seedSerializator.Deserialize(ref dynId, serializationStream);
            serializationStream.Position = streamStartPos;

            Id = dynId;

            if (!mapingSheme.Any(x => x.Key.FullTypeId == Id))
            {
                throw new InvalidCastException();
            }

            Type typePair = mapingSheme.First(x => x.Key.FullTypeId == Id).Value;
            var innerObj = GetType().GetRuntimeMethods().Where(x => x.Name == nameof(CreateObject))
                            .First()
                            .MakeGenericMethod(typePair)
                            .Invoke(this, new object[] { serializationStream });

            obj = new DynamicObject(innerObj!, Id.Id);
        }
        else if (type.BaseType == typeof(ValueTuple).BaseType)
        {
            object[] args = new object[type.GenericTypeArguments.Length];

            for (int i = 0; i < type.GenericTypeArguments.Length; i++)
            {
                args[i] = GetType().GetRuntimeMethods().Where(x => x.Name == nameof(CreateObject))
                            .First()
                            .MakeGenericMethod(type.GenericTypeArguments[i])
                            .Invoke(this, new object[] { serializationStream})!;
            }

            obj = Activator.CreateInstance(type, args);
        }
        else
        {
            obj = !typeof(T).GetConstructors().Any(x => x.GetParameters().Length == 0)
                ? (T)RuntimeHelpers.GetUninitializedObject(typeof(T))
                : Activator.CreateInstance(typeof(T));

            foreach (var field in obj!.GetType().GetFields())
            {
                // Skip all field except public non static rw
                if (field.IsStatic || field.IsPrivate && field.IsInitOnly || field.IsLiteral)
                {
                    continue;
                }

                if (field.FieldType.GetConstructors().Any(x => x.GetParameters().Length == 0) ||
                    field.FieldType.GetConstructors().Length == 0)
                {
                    field.SetValue(obj, Activator.CreateInstance(field.FieldType));
                }
            }
        }

        return (T) obj!;
    }

    private T DeserializeImpl<T>(Stream serializationStream)
    {
        object? beginObj = CreateObject<T>(serializationStream);
        T obj = (T) beginObj!;
        dynamic data = obj;
        seedSerializator.Deserialize(ref data, serializationStream);

        return data;
    }

    private T DeserializeCustomImpl<T>(Stream serializationStream)
    {
        T beginObj = CreateObject<T>(serializationStream);

        Contract.Assume(beginObj != null);

        dynamic data = beginObj;
        seedSerializator.Deserialize(ref data, serializationStream);

        return data;
    }

    private T DeserializeArrayImpl<T>(Stream serializationStream, RttiTypeRecord record)
    {
        object? beginObj = record.TypeClass switch
        {
            ETypes.FixedCollection => CreateObject<T>(serializationStream),
            ETypes.FixedArray => Array.CreateInstance(typeof(T).GetElementType()!, (int)record.ArrayItemsCount),
            _ => throw new InvalidCastException(),
        };

        T obj = (T) beginObj!;
        dynamic data = obj;

        seedSerializator.Deserialize(ref data, serializationStream);

        switch (record.TypeClass)
        {
            case ETypes.FixedCollection:
                {
                    var mirror = obj as IList;
                    if (mirror!.Count != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
            case ETypes.FixedArray:
                {
                    var mirror = obj as Array;
                    if (mirror!.Length != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
        }

        return data;
    }

    public object Deserialize(Stream serializationStream, RttiTypeRecord? targetType = null, Type? targetCsType = null)
    {
        logger?.LogInformation("Start deserializing Seed type {0} with clr type {1}", targetType?.TypeName ?? "null", targetCsType?.FullName ?? "null");

        if (targetType != null && !mapingSheme.ContainsKey(targetType))
        {
            throw new ArgumentException("Unknown type");
        }

        Type? type = targetCsType;
        var Id = new UniversalTypeIdentifier();
        dynamic ID = Id;

        seedSerializator.Deserialize(ref ID, serializationStream);
        Id = ID;

        logger?.LogInformation("Deserialize type full type id: {0}", Id.Id.ToString());

        if (targetType == null && mapingSheme.Any(x => x.Key.FullTypeId == Id))
        {
            var csType = mapingSheme.First(x => x.Key.FullTypeId == Id).Value;

            if (type != null && type != csType)
            {
                logger?.LogError("Deserialize failed. Unknown type");
                throw new InvalidCastException();
            }

            type = csType;
        }
        else if(targetType != null && mapingSheme.Any(x => x.Key.FullTypeId == Id) && Id == targetType.FullTypeId)
        {
            var csType = mapingSheme[targetType];

            if (type != null && type != csType)
            {
                logger?.LogError("Deserialize failed. Unknown type");
                throw new InvalidCastException();
            }

            type = csType;
        }
        else
        {
            logger?.LogError("Deserialize failed. Unknown type");
            throw new InvalidCastException();
        }

        object? retObj = null;

        logger?.LogDebug("Call serializator implementation");

        var record = rttiSection.AllRegisteredTypes.First(x => x.FullTypeId == Id);
        try
        {
            if (record.TypeClass == ETypes.FixedArray || record.TypeClass == ETypes.FixedCollection)
            {
                retObj = GetType().GetRuntimeMethods().Where(x => x.Name == nameof(DeserializeArrayImpl))
                    .First()
                    .MakeGenericMethod(type)
                    .Invoke(this, new object[] { serializationStream, record });
            }
            else if (record.TypeClass == ETypes.Custom)
            {
                retObj = GetType().GetRuntimeMethods().Where(x => x.Name == nameof(DeserializeCustomImpl))
                    .First()
                    .MakeGenericMethod(type)
                    .Invoke(this, new object[] { serializationStream });
            }
            else
            {
                retObj = GetType().GetRuntimeMethods().Where(x => x.Name == nameof(DeserializeImpl))
                    .First()
                    .MakeGenericMethod(type)
                    .Invoke(this, new object[] { serializationStream });
            }
        }
        catch (Exception ex)
        {
            logger?.LogError($"Deserialization failed. Error: {ex}");
            throw;
        }

        logger?.LogInformation("Deserialization completed successfully");

        return retObj!;
    }

    object ISeedBinarySerializator.Deserialize(Stream serializationStream, RttiTypeRecord? targetType)
    {
        return Deserialize(serializationStream, targetType, null);
    }

    public T Deserialize<T>(Stream serializationStream, RttiTypeRecord? targetType = null)
    {
        return (T) Deserialize(serializationStream, targetType, typeof(T));
    }

    public IEnumerable<TypeRecord> SerializableTypes => seedSerializator.GetSupportedTypes();

    public TypeRecord GetByClrType(Type type) => SerializableTypes.First(x => x.ClrType == type);
}
