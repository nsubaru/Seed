using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis;
using Microsoft.CSharp;
using Seed.CTTI;
using Seed.RTTI;
using System.Reflection;
using System.Text;
using Seed.RTTI.Record;
using static System.Linq.Enumerable;
using Microsoft.CodeAnalysis.Emit;
using Microsoft.CodeAnalysis.Text;
using System.Runtime.Loader;
using System.Diagnostics;
using Microsoft.Extensions.Logging;
using System.Runtime.CompilerServices;
using Seed.RTTI.CustomSerializationSupport;
using LanguageExt;

namespace Seed.Serialization;

public struct SeedSerializerCompilationResult
{
    public string Code { get; set; }

    public List<TypeRecord> SupportedTypes { get; set; }

    public EmitResult Result { get; set; }

    public MemoryStream PeStream { get; set; }

    public MemoryStream PdbStream { get; set; }
}

public static class SeedSerializerCompilationResultExtensions
{
    public static void Deconstruct(this SeedSerializerCompilationResult compilationResult, out string code, out List<TypeRecord> supportedTypes, out EmitResult result, out MemoryStream peStream, out MemoryStream pdbStream)
    {
        code           = compilationResult.Code;
        supportedTypes = compilationResult.SupportedTypes;
        result         = compilationResult.Result;
        peStream       = compilationResult.PeStream;
        pdbStream      = compilationResult.PdbStream;
    }
}

/// <summary>
/// Builds RTTI-to-CLR mappings and owns lazily generated, collectible serializer implementations.
/// </summary>
public partial class SeedSerializerFactory : ISeedSerializerFactory, IDisposable
{
    private readonly string serializationClassString;
    private readonly List<TypeRecord> supportedTypesList = new();
    private readonly List<KeyValuePair<string, string>> functions = new();
    private readonly Dictionary<RttiTypeRecord, ICustomClassSerializer> customTypeMappingSchema;
    private readonly Dictionary<String, List<ITemplateCustomClassSerializerFactory>> customTemplatesSerializerFactory;
    private readonly IEnumerable<RttiTypeRecord> allRttiTypes;
    private readonly RTTISection section;
    private ISeedSerializer? serializer;
    private readonly List<AssemblyLoadContext> assemblyContexts = new();
    private bool disposed;
    private readonly List<(Type Type, SeedSerializableAttribute Attribute)> serializableTypes;
    private readonly ILogger<SeedSerializerFactory>? logger;
    private readonly ILogger<ISeedSerializer>? serializerLogger;
    private readonly IServiceProvider serviceProvider;

    /// <summary>
    /// Builds the serializer on first use and retains it until this factory is disposed.
    /// </summary>
    /// <returns>Serializer owned by this factory; callers must keep the factory alive while using it.</returns>
    public ISeedSerializer GetSeedSerializer()
    {
        ObjectDisposedException.ThrowIf(disposed, this);

        return serializer ??= BuildSeedSerializer();
    }

    public RTTISection RTTISection => section;

    public static string GetFriendlyTypeName(Type type)
    {
        using var p = new CSharpCodeProvider();
        var r = new System.CodeDom.CodeTypeReference(type);

        return p.GetTypeOutput(r);
    }

    public List<TypeRecord> GetSupportedTypes() => supportedTypesList;

    /// <summary>
    /// Discovers serializer mappings and prepares source without loading an executable implementation.
    /// </summary>
    /// <param name="section">Native RTTI that must remain valid for the factory's lifetime.</param>
    /// <param name="serviceProvider">Services available to custom serializers.</param>
    /// <param name="logger">Optional logger for mapping and compilation diagnostics.</param>
    /// <param name="serializerLogger">Optional logger used by generated serializers.</param>
    public SeedSerializerFactory(RTTISection section, IServiceProvider serviceProvider, ILogger<SeedSerializerFactory>? logger = null, ILogger<ISeedSerializer>? serializerLogger = null)
    {
        serializationClassString = @"using System;
using System.IO;
using System.Collections.Generic;
using Seed;
using Seed.CTTI;
using Seed.CTTI.MetaData;
using Seed.RTTI;
using Seed.Core;
using Seed.Serialization;
using Seed.RTTI.Record;
using System.Text;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;
using System.Linq;
using System.Collections.Concurrent;
using System.Security;
using System.Reflection;
using Microsoft.Extensions.Logging;
using System.Linq.Expressions;

namespace Seed.Serializer;

struct DynamicSerializationInfo
{
    public Guid FullTypeId { get; set; }
    public Type ClrType { get; set; }
    public MethodInfo SerializeImpl { get; set; }
    public MethodInfo DeserializeImpl { get; set; }
}

public class SeedSerializer : ISeedSerializer {

    private List<TypeRecord> supportedTypesList = new List<TypeRecord>();

    private Dictionary<Guid, DynamicSerializationInfo> typesDict = new Dictionary<Guid, DynamicSerializationInfo>();   
    private Dictionary<Type, DynamicSerializationInfo> reverseTypesDict = new Dictionary<Type, DynamicSerializationInfo>();
    private readonly Dictionary<Type, (Action<object, Stream> Serialize, Func<object, Stream, object> Deserialize)> valueHandlers = new();

    private readonly Dictionary<ulong, ICustomClassSerializer> customTypeMappingSchema;
    private readonly IEnumerable<RttiTypeRecord> allKnowTypes;
    private readonly ILogger<ISeedSerializer>? logger;

    private readonly Dictionary<Guid, RttiTypeRecord> rttiTypeLookup;
    private readonly Dictionary<Type, Delegate> createObjectDelegateCache = new Dictionary<Type, Delegate>();

    private static readonly ConcurrentDictionary<Type, Func<object>> objectFactoryCache = new();

    public SeedSerializer(
        IEnumerable<RttiTypeRecord> allKnowTypes, 
        List<TypeRecord> supportedTypesList, 
        Dictionary<RttiTypeRecord, ICustomClassSerializer> customTypeMappingSchema = null,
        ILogger<ISeedSerializer>? logger = null)
    {
        this.supportedTypesList.AddRange(supportedTypesList); 
        this.customTypeMappingSchema = customTypeMappingSchema.ToDictionary(x => x.Key.FullTypeId.TypeId, x => x.Value);
        this.logger = logger;

        logger?.LogInformation(""Start SeedSerializer init"");

        this.typesDict = this.supportedTypesList
            .Where(x =>
                this.GetType().GetMethods().FirstOrDefault(y => y.Name == ""SerializeImpl"" && y.GetParameters().First().ParameterType == x.ClrType) != null &&
                this.GetType().GetMethods().FirstOrDefault(y => y.Name == ""DeserializeImpl"" && y.ReturnType == x.ClrType) != null)
            .DistinctBy(x => x.FullTypeId)
            .ToDictionary(x => x.FullTypeId, x => new DynamicSerializationInfo()
            {
                FullTypeId = x.FullTypeId,
                ClrType = x.ClrType,
                SerializeImpl = this.GetType().GetMethods().First(y => y.Name == ""SerializeImpl"" && y.GetParameters().First().ParameterType == x.ClrType),
                DeserializeImpl = this.GetType().GetMethods().First(y => y.Name == ""DeserializeImpl"" && y.ReturnType == x.ClrType)
            });

        this.reverseTypesDict = this.typesDict.DistinctBy(x => x.Value.ClrType).ToDictionary(x => x.Value.ClrType, x => x.Value);

        // __SERIALIZER_VALUE_HANDLERS__

        this.allKnowTypes = allKnowTypes;

        rttiTypeLookup = allKnowTypes.DistinctBy(x => x.FullTypeId.Id).ToDictionary(x => x.FullTypeId.Id, x => x);
    }

    public void Serialize(dynamic data, Stream serializationStream) {
        object value = data;
        if (value != null && valueHandlers.TryGetValue(value.GetType(), out var handler)) {
            handler.Serialize(value, serializationStream);
        } else {
            SerializeImpl(value, serializationStream);
        }
    }

    public void Deserialize(ref dynamic data, Stream serializationStream) { 
        object value = data;
        data = value != null && valueHandlers.TryGetValue(value.GetType(), out var handler)
            ? handler.Deserialize(value, serializationStream)
            : DeserializeImpl(value, serializationStream);
    }

    public void SerializeImpl(object data, Stream stream) {
        var typeName = data == null ? ""<null>"" : data.GetType().FullName;
        throw new InvalidOperationException($""Serialize unsupported CLR type {typeName}. Check managed DTO or custom serializer registration for the corresponding Seed type."");
    }

    public object DeserializeImpl(object data, Stream stream) {
        var typeName = data == null ? ""<null>"" : data.GetType().FullName;
        throw new InvalidOperationException($""Deserialize unsupported CLR type {typeName}. Check managed DTO or custom serializer registration for the corresponding Seed type."");
    }

    public List<TypeRecord> GetSupportedTypes() {
        return supportedTypesList;
    }

    public IEnumerable<RttiTypeRecord> GetAllRttiTypes() {
        return allKnowTypes;
    }

    public ILogger<ISeedSerializer>? Logger => logger;

    private static object FastCreateInstance(Type type)
    {
        var factory = objectFactoryCache.GetOrAdd(type, static type =>
        {
            Expression body;

            if (type.IsValueType)
            {
                // Creates default(T) for structs, including structs without explicit parameterless ctor.
                body = Expression.Default(type);
            }
            else
            {
                var ctor = type.GetConstructor(Type.EmptyTypes);
                if (ctor == null)
                {
                    throw new InvalidOperationException($""Type {type} has no default constructor"");
                }

                body = Expression.New(ctor);
            }

            // Required for value types: boxes T into object.
            body = Expression.Convert(body, typeof(object));

            return Expression.Lambda<Func<object>>(body).Compile();
        });

        return factory();
    }

    private static object FastCreateArrayInstance(Type type)
    {
        var factory = objectFactoryCache.GetOrAdd(type, static type =>
        {
            var elementType = type.GetElementType()
                ?? throw new InvalidOperationException($""Type {type} is not an array type"");

            var length = Expression.Constant(1);
            var newArrayExpr = Expression.NewArrayBounds(elementType, length);
            return Expression.Lambda<Func<object>>(newArrayExpr).Compile();
        });

        return factory();
    }

    public T CreateObject<T>()
    {
        /* This function copied from SeedSerializerFactory.cs with removing DynamicObject case as it processed via own handler
         * Here out task is create dummy object which will be not null but will not contain any data.
         * We have next cases:
         * - string                     -> just return string.Empty
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

            return (T) obj;
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
                field.SetValue(obj, InvokeCreateObject(field.FieldType));
            }
        }

        return (T) obj!;
    }

    private object InvokeCreateObject(Type type)
    {
        if (!createObjectDelegateCache.TryGetValue(type, out var methodDelegate))
        {
            var methodInfo = GetType().GetRuntimeMethods().First(x => x.Name == nameof(CreateObject)).MakeGenericMethod(type);

            var delegateType = typeof(Func<>)!.MakeGenericType(new Type[] { type });

            methodDelegate = methodInfo.CreateDelegate(delegateType, this);
            createObjectDelegateCache[type] = methodDelegate;
        }

        return methodDelegate.DynamicInvoke()!;
    }

    public object CreateObjectByTypeRecord(TypeRecord typeRecord) {
        return InvokeCreateObject(typeRecord.ClrType);
    }
";
        this.section = section;
        this.serviceProvider = serviceProvider;
        this.logger = logger;
        this.serializerLogger = serializerLogger;
        this.allRttiTypes = section.AllRegisteredTypes.Where(x => x.IsValid);

        logger?.LogInformation("Load custom serializers");
        var customSerializers = AppDomain.CurrentDomain.GetAssemblies().Where(x => !x.IsDynamic && !string.IsNullOrEmpty(x.Location)).SelectMany(x => x.ExportedTypes.Where(x => x.GetConstructors().Any(y =>
        {
            try
            {
                return y.GetParameters().Length == 0;
            }
            catch
            {
                return false;
            }
        })).Where(x => x.GetInterfaces().Contains(typeof(ICustomClassSerializer)) && x.IsClass && !x.IsAbstract))
           .Select(x => (ICustomClassSerializer)x.GetConstructors().First().Invoke(Array.Empty<object>()))
           .ToList();

        this.customTypeMappingSchema = customSerializers
            .Select(x => (Record: section.AllRegisteredTypes.FirstOrDefault(y => y.TypeName == x.TypeName), Serializer: x))
            .Where(x => x.Record != null)
            .ToDictionary(x => x.Record!, x => x.Serializer);

        var customSupportedTypes = customTypeMappingSchema.Select(x => new TypeRecord
        {
            TypeName = x.Key.TypeName,
            ClrType = x.Value.ClrType,
            Type = x.Key.TypeClass,
            FullTypeId = x.Key.FullTypeId.Id,
            RttiType = x.Key
        }).ToList();

        logger?.LogInformation("Load custom templates serializers");

        var customTemplatesSerializerFactoriesList = AppDomain.CurrentDomain.GetAssemblies().Where(x => !x.IsDynamic && !string.IsNullOrEmpty(x.Location)).SelectMany(x => x.ExportedTypes.Where(x => x.GetConstructors().Any(y =>
        {
            try
            {
                return y.GetParameters().Length == 0;
            }
            catch
            {
                return false;
            }
        })).Where(x => x.GetInterfaces().Contains(typeof(ITemplateCustomClassSerializerFactory)) && x.IsClass))
           .Select(x => (ITemplateCustomClassSerializerFactory)x.GetConstructors().First().Invoke(Array.Empty<object>()))
           .Select(x => new KeyValuePair<String, ITemplateCustomClassSerializerFactory>(x.TemplateTypeName, x));

        customTemplatesSerializerFactory = customTemplatesSerializerFactoriesList
            .Select(x => x.Key)
            .Distinct()
            .ToDictionary(x => x, x => customTemplatesSerializerFactoriesList.Where(y => y.Key == x).Select(y => y.Value).ToList());

        logger?.LogInformation("Load all serializable types");

        serializableTypes = AppDomain.CurrentDomain
            .GetAssemblies()
            .SelectMany(x => x.GetTypes().Where(x => x.GetCustomAttribute<SeedSerializableAttribute>() != null)).Where(x => !this.customTypeMappingSchema.Any(y => y.Value.ClrType == x))
            .Select(x => (Type: x, Attribute: x.GetCustomAttribute<SeedSerializableAttribute>()!))
            .ToList();
        
        logger?.LogInformation("Found {0} types", supportedTypesList.Count);
        logger?.LogInformation("Add custom serializable types");

        foreach (var item in customSupportedTypes)
        {
            if (supportedTypesList.Any(x => x.RttiType.TypeName == item.RttiType.TypeName))
            {
                continue;
            }

            supportedTypesList.Add(item);

            typeof(SeedSerializerFactory)!.GetMethod("AddType")!.MakeGenericMethod(item.ClrType)!.Invoke(this, new object[]
            {
                item
            });
        }

        logger?.LogInformation("Add all serializable types");

        foreach (var item in FilterAndOrderTypes(section.AllRegisteredTypes))
        {
            var ClrType = this.BuildClrType(item);
            if (ClrType == null)
            {
                continue;
            }

            var typeRecord = new TypeRecord
            {
                TypeName           = !customTemplatesSerializerFactory.ContainsKey(item.TemplateTypeName) && item.TypeModificators.HasFlag(TypeModificator.TemplateType) && item.TemplateTypeName != Constants.Types.ObjectProperties 
                    ? item.TemplateTypeName 
                    : item.TypeName,
                Type               = item.TypeClass,
                FullTypeId         = item.FullTypeId.Id,
                ClrType            = ClrType,
                RttiType           = item,
                IsObjectProperties = item.TemplateTypeName == Constants.Types.ObjectProperties
            };

            if (supportedTypesList.Any(x => x.RttiType.TypeName == typeRecord.RttiType.TypeName))
            {
                continue;
            }

            supportedTypesList.Add(typeRecord);

            typeof(SeedSerializerFactory).GetMethod(nameof(AddType))!.MakeGenericMethod(ClrType).Invoke(this, new object[]
            {
                typeRecord
            });
        }
    }

    private List<RttiTypeRecord> FilterAndOrderTypes(List<RttiTypeRecord> records)
    {
        logger?.LogInformation("Start filtering and ordering types...");
        var result = new List<RttiTypeRecord>();
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Logic));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Numeric && x.TemplateTypeName != Constants.Types.EnumFlag));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Enumeration));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Numeric && x.TemplateTypeName == Constants.Types.EnumFlag));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Character));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.FloatNumeric));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.String));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.MetaData));
        result.AddRange(records.Where(x => x.TypeClass == ETypes.Dynamic));

        result.ForEach(x => records.RemoveAll(y => y.FullTypeId == x.FullTypeId));

        records.RemoveAll(x => x.TypeClass == ETypes.Undefined);
        records.RemoveAll(x => x.TypeClass == ETypes.Function);
        records.RemoveAll(x => x.TypeClass == ETypes.Pointer);
        records.RemoveAll(x => x.TypeClass == ETypes.Reference);
        records.RemoveAll(x => x.TypeClass == ETypes.Abstract);
        records.RemoveAll(x => x.TypeClass == ETypes.Union);
        records.RemoveAll(x => x.TypeClass == ETypes.Object);
        records.RemoveAll(x => supportedTypesList.Any(y => x.FullTypeId.Id == y.FullTypeId));

        var oldCount = records.Count;

        do
        {
            oldCount = records.Count;

            var newRecords = records
                .Where(x => x.Fields
                    .All(y => result
                        .Any(z => z.FullTypeId == y.FullTypeId) || supportedTypesList
                        .Any(z => z.FullTypeId == y.FullTypeId.Id)) && x.Properties
                    .All(y => result
                        .Any(x => x.FullTypeId == y.FullTypeId) || supportedTypesList
                        .Any(z => z.FullTypeId == y.FullTypeId.Id)))
                .ToList();

            newRecords.ForEach(x => records.RemoveAll(y => y.FullTypeId == x.FullTypeId));

            result.AddRange(newRecords);
        }
        while (oldCount != records.Count);
        logger?.LogInformation("Excluded {0} types durring sorting and filtering", records.Count);

        return result;
    }

    #region TypeRegion
    private void AddTypeAsSupported(TypeRecord typeRecord)
    {
        supportedTypesList.Add(typeRecord);
    }

    public Type? BuildClrType(RttiTypeRecord record)
    {
        logger?.LogDebug("Build clr type for {0}", record.TypeName);

        if (customTypeMappingSchema.ContainsKey(record))
        {
            return customTypeMappingSchema[record].ClrType;
        }

        if (customTemplatesSerializerFactory.ContainsKey(record.TemplateTypeName))
        {
            var factories = customTemplatesSerializerFactory[record.TemplateTypeName];

            if (factories.Count == 0)
            {
                return null;
            }

            ICustomClassSerializer? serializer = null;

            foreach (var factory in factories)
            {
                serializer = factory.BuildSerializerForType(record, this, serviceProvider);

                if (serializer != null)
                {
                    break;
                }
            }

            if (serializer == null)
            {
                return null;
            }

            customTypeMappingSchema.Add(record, serializer);

            return serializer.ClrType;
        }

        if (!record.TypeModificators.HasFlag(TypeModificator.Serializable))
        {
            return null;
        }

        switch (record.TypeClass)
        {
            case ETypes.FloatNumeric:         return TryGetAsPrimitive(record);
            case ETypes.Character:            return TryGetAsPrimitive(record);
            case ETypes.Numeric:              return TryGetAsPrimitive(record);
            case ETypes.Logic:                return TryGetAsPrimitive(record);
            case ETypes.Collection:           return TryGetAsLinqSeedType(record);
            case ETypes.FixedCollection:      return TryGetAsLinqSeedType(record);
            case ETypes.AssociableCollection: return TryGetAsLinqSeedType(record);
            case ETypes.FixedArray:           return TryGetAsLinqSeedType(record) ?? TryBuildFixedArrayType(record, section);
            case ETypes.Pair:                 return TryGetAsLinqSeedType(record);
            case ETypes.MetaData:             return GetMetaDataType(record);
            case ETypes.Enumeration:          return GetEnumUnderlyingType(record, section);
            case ETypes.String:               return TryGetAsPrimitive(record);
            case ETypes.Box:                  return GetBoxType(record);
            case ETypes.Tuple:                return GetTupleType(record);
            case ETypes.Struct:               return GetStructType(record);
            case ETypes.Dynamic:              return typeof(DynamicObject);
            case ETypes.Custom:               return GetCustomSerializerType(record);
            case ETypes.Pointer:              return null;
            case ETypes.Reference:            return null;
            case ETypes.Object:               return null;
            case ETypes.Abstract:             return null;
            case ETypes.Function:             return null;
            case ETypes.Union:                return null;
            case ETypes.Undefined:            return null;
            default:                          return null;
        }
    }

    RTTI.Record.RttiTypeRecord? GetRecordByField(RTTI.RTTISection section, RTTI.Record.RttiField field)
    {
        return section.AllRegisteredTypes.Where(record => record.IsValid && record.FullTypeId == field.FullTypeId).FirstOrDefault();
    }

    Type? GetTypeWithOffset(int offset, RTTI.Record.RttiTypeRecord record)
    {
        var innerTypeRecord = GetRecordByField(section, record.Fields.Skip(offset).First());

        if (innerTypeRecord == null)
        {
            return null;
        }

        if (Debugger.IsAttached)
        {
            if (record.TypeName == Constants.Types.EmptyAttributeValueT)
            {
                Debugger.Break();
            }
        }

        var innerType = BuildClrType(innerTypeRecord);

        if (innerType == null)
        {
            return null;
        }

        return innerType;
    }

    #endregion
    #region Code generator
    #region Method headers
    private void AddSerHead<T>(ref StringBuilder code)
    {
        code.AppendLine($"public void SerializeImpl({GetFriendlyTypeName(typeof(T))} data, Stream stream) {{");
        code.AppendLine($"        if (logger?.IsEnabled(LogLevel.Trace) == true) {{ logger?.LogTrace(\"Serialize type {GetFriendlyTypeName(typeof(T))}\"); }}");
    }

    private void AddDeserHead<T>(ref StringBuilder code)
    {
        code.AppendLine($"public {GetFriendlyTypeName(typeof(T))} DeserializeImpl({GetFriendlyTypeName(typeof(T))} data, Stream stream) {{");
        code.AppendLine($"        if (logger?.IsEnabled(LogLevel.Trace) == true) {{ logger?.LogTrace(\"Deserialize type {GetFriendlyTypeName(typeof(T))}\"); }}");
    }

    #endregion
    bool CheckForExcludedTypes(ETypes type)
    {
        return type switch
        {
            ETypes.Pointer or ETypes.Reference or ETypes.Object or ETypes.Abstract or ETypes.Function or ETypes.Union or ETypes.Undefined => true,

            ETypes.Box or ETypes.FloatNumeric or ETypes.Character or ETypes.Numeric or ETypes.Logic or ETypes.Collection or ETypes.FixedCollection or ETypes.AssociableCollection 
                       or ETypes.FixedArray or ETypes.Pair or ETypes.MetaData or ETypes.Enumeration or ETypes.String or ETypes.Tuple or ETypes.Struct or ETypes.Dynamic or ETypes.Custom => false,

            _ => true,
        };
    }

    private void AddSerializer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        AddSerHead<T>(ref code);

        if (typeRecord.IsObjectProperties)
        {
            AddFieldsObjectPropertiesSer<T>(ref code, typeRecord);
        }
        else if (customTypeMappingSchema.ContainsKey(typeRecord.RttiType))
        {
            AddCustomTypeSer<T>(ref code, typeRecord);
        }
        else
        {
            switch (typeRecord.Type)
            {
                case ETypes.FloatNumeric:
                case ETypes.Character:
                case ETypes.Numeric:
                case ETypes.Logic:
                    AddSimpleTypeSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Box:
                    AddBoxSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Enumeration:
                    AddEnumSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Collection:
                    AddCollectionSer<T>(ref code, typeRecord);
                    break;

                case ETypes.String:
                    AddStringSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Struct:
                    AddStructSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Tuple:
                    AddTupleSer<T>(ref code, typeRecord);
                    break;

                case ETypes.FixedCollection:
                    AddFixedCollectionSer<T>(ref code, typeRecord);
                    break;

                case ETypes.AssociableCollection:
                    AddAssociableCollectionSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Pair:
                    AddPairSer<T>(ref code, typeRecord);
                    break;

                case ETypes.FixedArray:
                    AddFixedArraySer<T>(ref code, typeRecord);
                    break;

                case ETypes.Dynamic:
                    AddDynamicSer<T>(ref code, typeRecord);
                    break;

                case ETypes.MetaData:
                    AddMetaDataSer<T>(ref code, typeRecord);
                    break;

                case ETypes.Custom:
                    AddCustomTypeSer<T>(ref code, typeRecord);
                    break;

                //Excluded types:
                case ETypes.Pointer:
                case ETypes.Reference:
                case ETypes.Object:
                case ETypes.Abstract:
                case ETypes.Function:
                case ETypes.Union:
                case ETypes.Undefined:
                default:
                    break;
            }
        }

        code.AppendLine("}");
    }

    private void AddDeserializer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        AddDeserHead<T>(ref code);

        if (typeRecord.IsObjectProperties)
        {
            AddFieldsObjectPropertiesDer<T>(ref code, typeRecord);
        }
        else if (customTypeMappingSchema.ContainsKey(typeRecord.RttiType))
        {
            AddCustomTypeDer<T>(ref code, typeRecord);
        }
        else
        {
            switch (typeRecord.Type)
            {
                case ETypes.FloatNumeric:
                case ETypes.Character:
                case ETypes.Numeric:
                case ETypes.Logic:
                    AddSimpleTypeDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Box:
                    AddBoxDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Enumeration:
                    AddEnumDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Collection:
                    AddCollectionDer<T>(ref code, typeRecord);
                    break;

                case ETypes.String:
                    AddStringDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Struct:
                    AddStructDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Tuple:
                    AddTupleDer<T>(ref code, typeRecord);
                    break;

                case ETypes.FixedCollection:
                    AddFixedCollectionDer<T>(ref code, typeRecord);
                    break;

                case ETypes.AssociableCollection:
                    AddAssociableCollectionDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Pair:
                    AddPairDer<T>(ref code, typeRecord);
                    break;

                case ETypes.FixedArray:
                    AddFixedArrayDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Dynamic:
                    AddDynamicDer<T>(ref code, typeRecord);
                    break;

                case ETypes.MetaData:
                    AddMetaDataDer<T>(ref code, typeRecord);
                    break;

                case ETypes.Custom:
                    AddCustomTypeDer<T>(ref code, typeRecord);
                    break;

                //Excluded types:
                case ETypes.Pointer:
                case ETypes.Reference:
                case ETypes.Object:
                case ETypes.Abstract:
                case ETypes.Function:
                case ETypes.Union:
                case ETypes.Undefined:
                default:
                    break;
            }
        }

        code.AppendLine("return data; \n}");
    }

    #endregion
    public void AddType<T>(TypeRecord typeRecord)
    {
        if (CheckForExcludedTypes(typeRecord.Type))
        {
            return;
        }

        logger?.LogInformation("Add {0} type for Seed type {1}", typeRecord.ClrType.FullName, typeRecord.RttiType.TypeName);

        StringBuilder serFunctionCode = new StringBuilder();
        StringBuilder derFunctionCode = new StringBuilder();

        AddTypeAsSupported(typeRecord);

        AddSerializer<T>(ref serFunctionCode, typeRecord);
        AddDeserializer<T>(ref derFunctionCode, typeRecord);

        functions.Add(new KeyValuePair<string, string>("ser " + GetFriendlyTypeName(typeRecord.ClrType), serFunctionCode.ToString()));
        functions.Add(new KeyValuePair<string, string>("der " + GetFriendlyTypeName(typeRecord.ClrType), derFunctionCode.ToString()));
    }

    /// <summary>
    /// Produces serializer source with instance-owned typed dispatch delegates.
    /// </summary>
    /// <returns>C# source for this factory's supported native-to-CLR mappings.</returns>
    public string GetSerializerClassCode()
    {
        var functionsCode = functions.DistinctBy(x => x.Key).Select(x => x.Value).Aggregate(string.Concat);
        var handlers = new StringBuilder();

        foreach (var type in supportedTypesList.Select(record => record.ClrType).Distinct())
        {
            var name = GetFriendlyTypeName(type);

            handlers.AppendLine($"valueHandlers[typeof({name})] = ((value, stream) => SerializeImpl(({name})value, stream), (value, stream) => DeserializeImpl(({name})value, stream));");
        }

        // C# dynamic overload binding caches generated declaring types outside their load context.
        // Compile direct, instance-owned delegates so using a serializer does not prevent unloading.
        return serializationClassString.Replace("// __SERIALIZER_VALUE_HANDLERS__", handlers.ToString()) + functionsCode + "}\n";
    }

    /// <summary>
    /// Compiles serializer source without loading the emitted assembly into the process.
    /// </summary>
    /// <returns>Source and diagnostics plus PE/PDB streams that the caller must dispose.</returns>
    public SeedSerializerCompilationResult CompileSerializer()
    {
        ObjectDisposedException.ThrowIf(disposed, this);
        logger?.LogInformation("Start codegen for serializer");

        var assemblyName = Path.GetRandomFileName();
        var symbolsName = Path.ChangeExtension(assemblyName, "pdb");
        var sourceCodePath = "SeedSerializer.cs";

        logger?.LogInformation("Format code of serializer");
        var codeText = CSharpSyntaxTree.ParseText(this.GetSerializerClassCode())
            .GetRoot()
            .NormalizeWhitespace().SyntaxTree
            .GetText()
            .ToString();

        using var stream = new MemoryStream();
        using var writer = new StreamWriter(stream);

        writer.Write(codeText);
        writer.Flush();
        stream.Position = 0;

        logger?.LogInformation("Load code of serializer");
        var code = SourceText.From(stream, Encoding.Default, canBeEmbedded: true);

        logger?.LogInformation("Search all assemblies for serializer");

        var assemblyPath = Path.GetDirectoryName(typeof(object).Assembly.Location)!;

        var assemblies = new List<Assembly>
        {
            typeof(System.Object).Assembly,
            typeof(System.ValueTuple).Assembly,
            typeof(System.Tuple).Assembly,
            typeof(Stream).Assembly,
            typeof(ValueType).Assembly,
            typeof(ISeedSerializer).Assembly,
            typeof(DynamicObject).Assembly,
            typeof(TypeRecord).Assembly,
            typeof(RTTISection).Assembly,
            typeof(ETypes).Assembly,
            typeof(ILogger<>).Assembly,
            typeof(RuntimeHelpers).Assembly,
            typeof(Optional<>).Assembly,
            Assembly.LoadFile(Path.Combine(assemblyPath, "mscorlib.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "netstandard.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Core.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Runtime.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.LINQ.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Collections.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "Microsoft.CSharp.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Runtime.InteropServices.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Linq.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Linq.Expressions.dll")),
            Assembly.LoadFile(Path.Combine(assemblyPath, "System.Collections.Concurrent.dll"))
        };

        assemblies.AddRange(customTypeMappingSchema.Select(x => x.Value.ClrType.Assembly).ToList().Concat(assemblies).DistinctBy(x => x.FullName).ToList());

        var supportedTypes = supportedTypesList.DistinctBy(x => x.FullTypeId).ToList();

        assemblies.AddRange(supportedTypes.Select(x => x.ClrType.Assembly).Concat(assemblies).ToList().DistinctBy(x => x.FullName).ToList());

        MetadataReference[] references = assemblies.DistinctBy(x => x.FullName).Select(x => MetadataReference.CreateFromFile(x.Location)).ToArray();
        
        logger?.LogInformation("Compile code of serializer");

        var comp = CSharpCompilation.Create(
            assemblyName: assemblyName,
            syntaxTrees: new[] { CSharpSyntaxTree.ParseText(code, path: sourceCodePath) },
            references: references,
            options: new CSharpCompilationOptions(OutputKind.DynamicallyLinkedLibrary, allowUnsafe: true)
#if DEBUG
                .WithOptimizationLevel(OptimizationLevel.Debug)
#endif
        );

        logger?.LogInformation("Create streams for serializer dll and pdb");

        var ms            = new MemoryStream();
        var pdbMs         = new MemoryStream();
        var emitOptions   = new EmitOptions(debugInformationFormat: DebugInformationFormat.PortablePdb, pdbFilePath: symbolsName);
        var embeddedTexts = new List<EmbeddedText> { EmbeddedText.FromSource(sourceCodePath, code) };

        logger?.LogInformation("Save compilation result to dll and pdb");
        EmitResult result = comp.Emit(peStream: ms, pdbStream: pdbMs, embeddedTexts: embeddedTexts, options: emitOptions);

        return new SeedSerializerCompilationResult
        {
            Code           = codeText,
            SupportedTypes = supportedTypes,
            Result         = result,
            PeStream       = ms,
            PdbStream      = pdbMs
        };
    }

    /// <summary>
    /// Compiles and loads a new serializer in a collectible context owned by this factory.
    /// </summary>
    /// <returns>A serializer valid until the factory is disposed; use GetSeedSerializer to reuse one instance.</returns>
    public ISeedSerializer BuildSeedSerializer()
    {
        ObjectDisposedException.ThrowIf(disposed, this);

        var (code, supportedTypes, result, ms, pdbMs) = CompileSerializer();

        using var peStream = ms;
        using var pdbStream = pdbMs;

        if (!result.Success)
        {
            logger?.LogError("Compilation failed");

            var failures = result.Diagnostics.Where(diagnostic => diagnostic.IsWarningAsError || diagnostic.Severity == DiagnosticSeverity.Error);
            var errorMessage = new StringBuilder("Compilation failed");

            foreach (Diagnostic diagnostic in failures)
            {
                var diagnosticMessage = $"{diagnostic.Location}:{diagnostic.Id} - {diagnostic.GetMessage()}";

                logger?.LogError($"Compilation errors: {diagnosticMessage}");
                Debug.WriteLine(diagnosticMessage);

                errorMessage.AppendLine();
                errorMessage.Append(diagnosticMessage);
            }

            throw new Exception(errorMessage.ToString());
        }
        else
        {
            logger?.LogInformation("Compilation success");

            ms.Seek(0, SeekOrigin.Begin);
            pdbMs.Seek(0, SeekOrigin.Begin);
            logger?.LogInformation("Load serializer type");

            var loadContext = new AssemblyLoadContext("Seed serializer", isCollectible: true);

            try
            {
                var a = loadContext.LoadFromStream(ms, pdbMs);
                var serializerType = a.GetExportedTypes().First(x => x.GetInterfaces().Contains(typeof(ISeedSerializer)));

                logger?.LogDebug("Build instance ctor args");

                object?[] args = new object?[]
                {
                    allRttiTypes,
                    supportedTypes,
                    customTypeMappingSchema,
                    serializerLogger
                };

                Type[] argsTypes = new Type[]
                {
                    allRttiTypes.GetType(),
                    supportedTypes.GetType(),
                    customTypeMappingSchema.GetType(),
                    serializerLogger?.GetType() ?? typeof(ILogger<ISeedSerializer>)
                };

                logger?.LogInformation("Create instance of serializer");
                var serializer = serializerType.GetConstructor(argsTypes)!.Invoke(args) as ISeedSerializer;
                assemblyContexts.Add(loadContext);

                return serializer!;
            }
            catch
            {
                loadContext.Unload();
                throw;
            }
        }
    }

    /// <summary>
    /// Releases generated serializers and requests unloading after their last external reference is released.
    /// </summary>
    public void Dispose()
    {
        if (disposed)
        {
            return;
        }

        disposed = true;
        serializer = null;
        
        foreach (var context in assemblyContexts)
        {
            context.Unload();
        }

        assemblyContexts.Clear();
        customTypeMappingSchema.Clear();
        customTemplatesSerializerFactory.Clear();
        supportedTypesList.Clear();
        serializableTypes.Clear();
        functions.Clear();
    }
}
