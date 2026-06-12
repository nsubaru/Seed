using System.Reflection;
using Seed.RTTI;
using Seed.Serialization;
using Seed.RTTI.Record;
using System.Collections;
using System.Collections.Concurrent;
using Microsoft.Extensions.Logging;
using System.Runtime.CompilerServices;
using System.Diagnostics.Contracts;
using Seed.Core.RTTI;
using LanguageExt;
using System.Linq.Expressions;
using Windows.UI.ViewManagement;

namespace Seed.CTTI;

/// <summary>
/// Binnary serializer for Seed types
/// </summary>
public class SeedBinarySerializer : ISeedBinarySerializer
{
    private readonly RTTISection rttiSection;
    private readonly ISeedSerializer seedSerializer;
    private readonly Dictionary<RttiTypeRecord, Type> mappingScheme = new();
    private readonly Dictionary<Type, RttiTypeRecord> reverseMappingSchema = new();

    private readonly ILogger<SeedBinarySerializer>? logger;

    #region Caches for delegates
    private readonly Dictionary<Type, Delegate> serMethodDelegateCache = new Dictionary<Type, Delegate>();
    private readonly Dictionary<Type, Delegate> derMethodDelegateCache = new Dictionary<Type, Delegate>();
    private readonly Dictionary<Type, Delegate> createObjectDelegateCache = new Dictionary<Type, Delegate>();
    #endregion

    #region Additional caches
    private readonly Dictionary<Guid, RttiTypeRecord> rttiTypeLookup;
    private static readonly ConcurrentDictionary<Type, Func<object>> objectFactoryCache = new();
    #endregion

    /// <summary>
    /// Ctor
    /// </summary>
    /// <param name="rttiSection">RTTI section that contain information about all types registered in Seed</param>
    /// <param name="serviceProvider">For ad-hoc service resolution</param>
    /// <param name="logger">Serializer logger</param>
    /// <param name="seedSerializerFactory">Serializer implementation factory logger</param>
    /// <param name="seedSerializerImplLogger">Serializer implementation logger</param>
    public SeedBinarySerializer(
        RTTISection rttiSection, 
        IServiceProvider serviceProvider,
        ILogger<SeedBinarySerializer>? logger = null, 
        ILogger<SeedSerializerFactory>? seedSerializerFactory = null,
        ILogger<ISeedSerializer>? seedSerializerImplLogger = null)
    {
        this.rttiSection = rttiSection;
        this.logger = logger;

        logger?.LogInformation("Create serializer factory");
        var serializerFactory = new SeedSerializerFactory(rttiSection, serviceProvider, seedSerializerFactory, seedSerializerImplLogger);

        logger?.LogInformation("Retrieve serializer implementation");
        seedSerializer = serializerFactory.GetSeedSerializer();

        logger?.LogInformation("Load supported types from serializer implementation");


        mappingScheme = seedSerializer
            .GetSupportedTypes()
            .OrderBy(x => x.RttiType.TypeClass)
            .ThenBy(x => x.RttiType.TypeName)
            .ToDictionary(x => x.RttiType, x => x.ClrType);

        reverseMappingSchema = mappingScheme
            .OrderBy(x => x.Key.TypeClass)
            .ThenBy(x => x.Key.TypeName)
            .GroupBy(x => x.Value)
            .Select(g => g.First())
            .ToDictionary(x => x.Value, x => x.Key);

        rttiTypeLookup = rttiSection.AllRegisteredTypes
            .OrderBy(x => x.TypeClass)
            .ThenBy(x => x.TypeName)
            .GroupBy(x => x.FullTypeId.Id)
            .Select(g => g.First())
            .ToDictionary(x => x.FullTypeId.Id, x => x);
    }

    #region Serialize
    public void Serialize(Stream serializationStream, object graph, RttiTypeRecord? targetType = null)
    {
        if (graph == null)
        {
            throw new NullReferenceException();
        }

        var graphType = graph.GetType();

        logger?.LogInformation("Start serializing Seed type {0} with CLR type {1}", targetType?.TypeName ?? "null", graph.GetType().FullName);

        Type? type = null;
        var fullTypeId = new UniversalTypeIdentifier();

        if (targetType != null)
        {
            if (!mappingScheme.TryGetValue(targetType, out var mappedType))
            {
                logger?.LogError("Serialize failed. Unknown type");

                throw new ArgumentException("Unknown type");
            }
            
            if (mappedType == graphType)
            {
                fullTypeId = targetType.FullTypeId;
                type = mappingScheme[targetType];
            }
            else
            {
                logger?.LogError("Serialize failed. Unknown type");

                throw new InvalidCastException();
            }
        }
        else if (reverseMappingSchema.TryGetValue(graphType, out var mappedType))
        {
            fullTypeId = mappedType.FullTypeId;
            type = graphType;
        }
        else
        {
            logger?.LogError("Serialize failed. Unknown type");

            throw new InvalidCastException();
        }

        logger?.LogDebug("Call serializer implementation for serialize full type id");

        seedSerializer.Serialize(fullTypeId, serializationStream);

        if (!rttiTypeLookup.TryGetValue(fullTypeId.Id, out RttiTypeRecord? record))
        {
            logger?.LogError("Serialize failed. Unknown full type id {fullTypeId}", fullTypeId);

            throw new ArgumentException("Unknown full type ID");
        }

        logger?.LogDebug("Call serializer implementation for serialize data");

        try
        {
            if (record.TypeClass == ETypes.FixedArray || record.TypeClass == ETypes.FixedCollection)
            {
                InvokeSerializeMethod(serializationStream, record, type!, graph, nameof(SerializeArrayImpl));
            }
            else
            {
                InvokeSerializeMethod(serializationStream, record, type!, graph, nameof(SerializeImpl));
            }
        }
        catch(Exception ex)
        {
            logger?.LogError("Serialization failed with exception: {0}", ex.ToString());
            throw;
        }

        logger?.LogInformation("Serialization successfully completed");
    }

    private void SerializeImpl<T>(Stream serializationStream, T graph, RttiTypeRecord _)
    {
        seedSerializer.Serialize(graph!, serializationStream);
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

        SerializeImpl(serializationStream, graph, record);
    }

    private static object FastCreateInstance(Type type)
    {
        var factory = objectFactoryCache.GetOrAdd(type, static type =>
        {
            var ctor = type.GetConstructor(Type.EmptyTypes);

            if (ctor == null)
            {
                throw new InvalidOperationException($"Type {type} has no default constructor");
            }

            var newExpr = Expression.New(ctor);
            return Expression.Lambda<Func<object>>(newExpr).Compile();
        });

        return factory();
    }

    private static object FastCreateArrayInstance(Type type)
    {
        var factory = objectFactoryCache.GetOrAdd(type, static type =>
        {
            var elementType = type.GetElementType()
                ?? throw new InvalidOperationException($"Type {type} is not an array type");

            var length = Expression.Constant(1);
            var newArrayExpr = Expression.NewArrayBounds(elementType, length);
            return Expression.Lambda<Func<object>>(newArrayExpr).Compile();
        });

        return factory();
    }

    private T CreateObject<T>(Stream serializationStream)
    {
        /* Here out task is create dummy object which will be not null but will not contain any data.
         * We have next cases:
         * - string                     -> just return string.Empty
         * - our DynamicObject          -> we resolve boxed type by full type id and create object of this type
         * - type have empty ctor       -> use FastCreateInstance that create type via expression that cached
         * - type is array              -> use FastCreateArrayInstance that create type via expression that cached
         * - type is value type         -> use RuntimeHelpers.GetUninitializedObject
         * - type don't have empty ctor -> use RuntimeHelpers.GetUninitializedObject
         * - general case               -> use Activator.CreateInstance
         */

        Type type = typeof(T);

        object? obj = null;

        if (type == typeof(String))
        {
            obj = string.Empty;

            return (T) obj!;
        }
        
        if (type == typeof(DynamicObject))
        {
            UniversalTypeIdentifier Id = new();
            dynamic dynId = Id;

            var streamStartPos = serializationStream.Position;
            seedSerializer.Deserialize(ref dynId, serializationStream);

            Id = dynId;

            if (!rttiTypeLookup.TryGetValue(Id.Id, out RttiTypeRecord? record))
            {
                throw new InvalidCastException();
            }

            if (!mappingScheme.TryGetValue(record, out Type? targetType))
            {
                throw new InvalidCastException();
            }

            var innerObj = InvokeCreateObject(serializationStream, targetType);

            obj = new DynamicObject(innerObj!, Id.Id);

            // We need to restore position since CreateObject should create dummy object and we should not read any data from stream
            serializationStream.Position = streamStartPos;

            return (T) obj!;
        }

        if (typeof(T).GetConstructor(Type.EmptyTypes) != null)
        {
            obj = FastCreateInstance(type);
        }
        else if (typeof(T).IsArray)
        {
            obj = FastCreateArrayInstance(type);
        }
        else if (typeof(T).IsValueType || typeof(T).GetConstructor(Type.EmptyTypes) == null)
        {
            obj = RuntimeHelpers.GetUninitializedObject(type);
        }
        else
        {
            obj = Activator.CreateInstance<T>();
        }

        foreach (var field in type.GetFields(BindingFlags.Instance | BindingFlags.Public))
        {
            if (!field.FieldType.IsValueType && field.GetValue(obj) == null)
            {
                field.SetValue(obj, InvokeCreateObject(serializationStream, field.FieldType));
            }
        }

        return (T) obj!;
    }
    #endregion

    #region Deserialize
    public object Deserialize(Stream serializationStream, RttiTypeRecord? targetType = null, Type? targetCsType = null)
    {
        logger?.LogInformation("Start deserialization Seed type {0} with CLR type {1}", targetType?.TypeName ?? "null", targetCsType?.FullName ?? "null");

        if (targetType != null && !mappingScheme.ContainsKey(targetType))
        {
            throw new ArgumentException("Unknown type");
        }

        Type? type = targetCsType;
        var fullTypeId = new UniversalTypeIdentifier();
        dynamic dynFullTypeId = fullTypeId;

        seedSerializer.Deserialize(ref dynFullTypeId, serializationStream);
        fullTypeId = dynFullTypeId;

        logger?.LogInformation("Deserialize type full type id: {0}", fullTypeId.Id.ToString());

        if (!rttiTypeLookup.TryGetValue(fullTypeId.Id, out var actualTargetType))
        {
            logger?.LogError("Deserialize failed. Unknown type");

            throw new InvalidCastException();
        }

        if (mappingScheme.TryGetValue(actualTargetType, out var csType))
        {
            if (targetType != null && targetType.FullTypeId != fullTypeId)
            {
                logger?.LogError("Deserialize failed. Type mismatch");

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

        if (!rttiTypeLookup.TryGetValue(fullTypeId.Id, out RttiTypeRecord? record)) 
        {
            logger?.LogError("Deserialize failed. Unknown full type id {fullTypeId}", fullTypeId);

            throw new ArgumentException("Unknown full type ID");
        }

        logger?.LogDebug("Call deserializer implementation");

        try
        {
            switch (record.TypeClass)
            {
                case ETypes.FixedArray:
                case ETypes.FixedCollection:
                    retObj = InvokeDeserializeMethod(serializationStream, record, type, nameof(DeserializeArrayImpl));
                    break;

                case ETypes.Custom:
                    retObj = InvokeDeserializeMethod(serializationStream, record, type, nameof(DeserializeCustomImpl));
                    break;

                default:
                    retObj = InvokeDeserializeMethod(serializationStream, record, type, nameof(DeserializeImpl));
                    break;
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

    private T DeserializeImpl<T>(Stream serializationStream, RttiTypeRecord _)
    {
        object? beginObj = CreateObject<T>(serializationStream);
        T obj = (T)beginObj!;
        dynamic data = obj;
        seedSerializer.Deserialize(ref data, serializationStream);

        return data;
    }

    private T DeserializeCustomImpl<T>(Stream serializationStream, RttiTypeRecord _)
    {
        T beginObj = CreateObject<T>(serializationStream);

        Contract.Assume(beginObj != null);

        dynamic data = beginObj;
        seedSerializer.Deserialize(ref data, serializationStream);

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

        T obj = (T)beginObj!;
        dynamic data = obj;

        seedSerializer.Deserialize(ref data, serializationStream);

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
    #endregion

    #region Cached invokes
    private object InvokeDeserializeMethod(Stream serializationStream, RttiTypeRecord rttiType, Type type, string methodName)
    {
        if (!derMethodDelegateCache.TryGetValue(type, out var methodDelegate))
        {
            var methodInfo = GetType().GetRuntimeMethods().First(x => x.Name == methodName).MakeGenericMethod(type);

            var delegateType = typeof(Func<,,>)!.MakeGenericType(new Type[] { typeof(Stream), typeof(RttiTypeRecord), type });

            methodDelegate = methodInfo.CreateDelegate(delegateType, this);
            derMethodDelegateCache[type] = methodDelegate;
        }

        return methodDelegate.DynamicInvoke(serializationStream, rttiType)!;
    }

    private void InvokeSerializeMethod(Stream serializationStream, RttiTypeRecord rttiType, Type type, object graph, string methodName)
    {
        if (!serMethodDelegateCache.TryGetValue(type, out var methodDelegate))
        {
            var methodInfo = GetType().GetRuntimeMethods().First(x => x.Name == methodName).MakeGenericMethod(type);

            var delegateType = typeof(Action<,,>)!.MakeGenericType(new Type[] { typeof(Stream), type, typeof(RttiTypeRecord) });

            methodDelegate = methodInfo.CreateDelegate(delegateType, this);
            serMethodDelegateCache[type] = methodDelegate;
        }

        methodDelegate.DynamicInvoke(serializationStream, graph, rttiType);
    }

    private object InvokeCreateObject(Stream serializationStream, Type type)
    {
        if (!createObjectDelegateCache.TryGetValue(type, out var methodDelegate))
        {
            var methodInfo = GetType().GetRuntimeMethods().First(x => x.Name == nameof(CreateObject)).MakeGenericMethod(type);

            var delegateType = typeof(Func<,>)!.MakeGenericType(new Type[] { typeof(Stream), type });

            methodDelegate = methodInfo.CreateDelegate(delegateType, this);
            createObjectDelegateCache[type] = methodDelegate;
        }

        return methodDelegate.DynamicInvoke(serializationStream)!;
    }
    #endregion

    #region Interface methods
    object ISeedBinarySerializer.Deserialize(Stream serializationStream, RttiTypeRecord? targetType)
    {
        return Deserialize(serializationStream, targetType, null);
    }

    public T Deserialize<T>(Stream serializationStream, RttiTypeRecord? targetType = null)
    {
        return (T) Deserialize(serializationStream, targetType, typeof(T));
    }

    public IEnumerable<TypeRecord> SerializableTypes => seedSerializer.GetSupportedTypes();

    public TypeRecord GetByClrType(Type type) => SerializableTypes.First(x => x.ClrType == type);
    #endregion
}
