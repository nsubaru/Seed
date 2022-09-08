using Seed.CTTI.MetaData;
using Seed.CTTI.ClassSerializators;
using Seed.CTTI.ClassSerializators.ComplexTypes;
using Seed.CTTI.ClassSerializators.SimpleTypes;
using System.Data;
using System.Reflection;
using System.Runtime.Serialization;
using Seed.RTTI;
using System.IO;
using System;
using System.Linq;
using System.Collections.Generic;
using Seed.Serialization;

namespace Seed.CTTI;

public class SeedBinnarySerializator : IFormatter
{
    private List<TypeRecord> AllTypesList = new List<TypeRecord>();

    public List<SerializationTypeRecord> SerializationShema { get; private set; } = new List<SerializationTypeRecord>();

    Dictionary<ETypes, ITypeSerializator> TypeClassSerializators { get; set;} = new Dictionary<ETypes, ITypeSerializator>();

    public SerializationBinder Binder { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public StreamingContext Context { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public ISurrogateSelector SurrogateSelector { get => throw new NotImplementedException(); set => throw new NotImplementedException(); }

    public SeedBinnarySerializator(RTTISection rttiSection)
    {
        LoadTypeSerilizator();
        LoadBuiltInClassInformation();
        LoadStringTypeInformation();
        LoadMetaDataTypes();
        LoadDynamicDataTypes();
        LoadRttiTypes(rttiSection);
        LoadFullTypesIds();
    }

    private void LoadFullTypesIds()
    {
        for (int i = 0; i < SerializationShema.Count; i++)
        {
            SerializationShema[i] = new SerializationTypeRecord() {
                FullTypeId = AllTypesList
                    .FirstOrDefault(y => y.TypeName == SerializationShema[i].SeedKernelTypeName).FullTypeId,
                ClrType = SerializationShema[i].ClrType,
                SeedKernelTypeShema = SerializationShema[i].SeedKernelTypeShema,
            };
        }
    }

    private void LoadRttiTypes(RTTISection rttiSection)
    {
        var decoder = new TypeMetaDataDecoder();

        var types = rttiSection.AllRegisteredTypes
            .Where(x => x.IsValid)
            .Select(x => new TypeRecord(){
                TypeName = x.TypeName,
                SerializationSchema = x.SerializationSchema,
                FullTypeId = x.FullTypeId.Id
            })
            .ToList();

        AllTypesList.AddRange(types);

        var serializableTypes = Assembly.GetExecutingAssembly()
            .GetReferencedAssemblies()
            .SelectMany(x => Assembly.Load(x).GetTypes().Where(x => Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) != null))
            .Select(x => new { Type = x, Attribute = (Attribute.GetCustomAttribute(x, typeof(SeedSerializableAttribute)) as SeedSerializableAttribute) })
            .ToList();

        var mappedTypes = AllTypesList
            .Where(x => serializableTypes.Any(y => y.Attribute!.SeedKernelTypeName == x.TypeName))
            .Select(x => new SerializationTypeRecord()
            {
                SeedKernelTypeShema = decoder.DecodeTypeMetaData(x.SerializationSchema),
                ClrType = serializableTypes.First(y => y.Attribute!.SeedKernelTypeName == x.TypeName).Type,
                FullTypeId = x.FullTypeId
            })
            .ToList();

        SerializationShema.AddRange(mappedTypes);
    }

    private void LoadTypeSerilizator()
    {
        TypeClassSerializators.Add(ETypes.Logic, new LogicClassSerializator());
        TypeClassSerializators.Add(ETypes.Numeric, new NumericClassSerializator());
        TypeClassSerializators.Add(ETypes.FloatNumeric, new FloatNumericClassSerializator());
        TypeClassSerializators.Add(ETypes.Character, new CharacterClassSerializator());
        TypeClassSerializators.Add(ETypes.Collection, new CollectionClassSerializator());
        TypeClassSerializators.Add(ETypes.String, new StringClassSerialization());
        TypeClassSerializators.Add(ETypes.Enumeration, new EnumerationClassSerializator());
        TypeClassSerializators.Add(ETypes.FixedCollection, new FixedCollectionClassSerializator());
        TypeClassSerializators.Add(ETypes.Pair, new PairClassSerializator());
        TypeClassSerializators.Add(ETypes.AssociableCollection, new AssociableCollectionClassSerializator());
        TypeClassSerializators.Add(ETypes.FixedArray, new FixedArraySerializator());
        TypeClassSerializators.Add(ETypes.Box, new BoxClassSerializator());
        TypeClassSerializators.Add(ETypes.Tuple, new TupleClassSerializator());
        TypeClassSerializators.Add(ETypes.MetaData, new MetaDataClassSerialization());
        TypeClassSerializators.Add(ETypes.Struct, new StructClassSerializator());
        TypeClassSerializators.Add(ETypes.Object, new StructClassSerializator());
        TypeClassSerializators.Add(ETypes.Dynamic, new DynamicClassSerializator());
    }

    private void LoadStringTypeInformation()
    {
        SerializationShema.Add(
            new SerializationTypeRecord
            {
                ClrType = typeof(string),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Core::String",
                    SizeOfSize = sizeof(UInt64) * 3,
                    StaticSize = 0,
                    AdditionalInfo = "",
                    Type = ETypes.String,
                    NestedTypes = new List<TypeMetaDataRecord>
                    {
                        SerializationShema.Where(record => record.SeedKernelTypeName == "Seed::Core::UChar").First().SeedKernelTypeShema
                    }
                }
            }
        );

        SerializationShema.Add(
            new SerializationTypeRecord
            {
                ClrType = typeof(string),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Core::SensitiveString",
                    SizeOfSize = sizeof(UInt64) * 3,
                    StaticSize = 0,
                    AdditionalInfo = "",
                    Type = ETypes.String,
                    NestedTypes = new List<TypeMetaDataRecord>
                    {
                        SerializationShema.Where(record => record.SeedKernelTypeName == "Seed::Core::UChar").First().SeedKernelTypeShema
                    }
                }
            }
        );
    }

    void LoadMetaDataTypes()
    {
        SerializationShema.Add(
           new SerializationTypeRecord
           {
               ClrType = typeof(TypeMetaDataRecord),
               SeedKernelTypeShema = new TypeMetaDataRecord
               {
                   TypeName = "Seed::Core::Serialization::TypeMetaDataRecord",
                   SizeOfSize = sizeof(UInt64),
                   StaticSize = 0,
                   AdditionalInfo = "",
                   Type = ETypes.MetaData,
                   NestedTypes = new List<TypeMetaDataRecord>
                   {
                        SerializationShema.Where(record => record.SeedKernelTypeName == "Seed::Core::String").First().SeedKernelTypeShema
                   }
               }
           }
       );

       SerializationShema.Add(
           new SerializationTypeRecord
           {
               ClrType = typeof(TypeMetaDataRecord),
               SeedKernelTypeShema = new TypeMetaDataRecord
               {
                   TypeName = "Seed::Core::Serialization::SensitiveTypeMetaDataRecord",
                   SizeOfSize = sizeof(UInt64),
                   StaticSize = 0,
                   AdditionalInfo = "",
                   Type = ETypes.MetaData,
                   NestedTypes = new List<TypeMetaDataRecord>
                   {
                        SerializationShema.Where(record => record.SeedKernelTypeName == "Seed::Core::SensitiveString").First().SeedKernelTypeShema
                   }
               }
           }
       );
    }

    void LoadDynamicDataTypes()
    {
        SerializationShema.Add(
           new SerializationTypeRecord
           {
               ClrType = typeof(DynamicObject),
               SeedKernelTypeShema = new TypeMetaDataRecord
               {
                   TypeName = "Seed::Core::Containers::Dynamic",
                   SizeOfSize = sizeof(UInt64),
                   StaticSize = 0,
                   AdditionalInfo = "",
                   Type = ETypes.Dynamic,
                   NestedTypes = new List<TypeMetaDataRecord>()
               }
           }
       );

        SerializationShema.Add(
            new SerializationTypeRecord
            {
                ClrType = typeof(DynamicObject),
                SeedKernelTypeShema = new TypeMetaDataRecord
                {
                    TypeName = "Seed::Core::RTTI::Dynamic",
                    SizeOfSize = sizeof(UInt64),
                    StaticSize = 0,
                    AdditionalInfo = "",
                    Type = ETypes.Dynamic,
                    NestedTypes = new List<TypeMetaDataRecord>()
                }
            }
        );
    }

    private void LoadBuiltInClassInformation() => SerializationShema
            .AddRange(
                TypeClassSerializators
                    .Where(item => item.Value.GetType().BaseType == typeof(SimpeTypeSerializator))
                    .Select(item => (SimpeTypeSerializator)item.Value)
                    .SelectMany(item => item.GetSupportedTypes())
            );

    private void SerializeHeader(Stream serializationStream, Guid typeId)
    {
        var typeMetaData = SerializationShema.First(serilizator => serilizator.ClrType == typeof(Guid));

        TypeClassSerializators[typeMetaData.SeedKernelTypeClass]
            .Serialize(typeId, serializationStream, typeMetaData.SeedKernelTypeShema, SerializationShema, TypeClassSerializators);
    }

    private Guid DeserializeHeader(Stream serializationStream)
    {
        var typeMetaData = SerializationShema.First(serilizator => serilizator.ClrType == typeof(Guid));
        var obj = new object();
        obj = new Guid();

        TypeClassSerializators[typeMetaData.SeedKernelTypeClass]
            .Deserialize(ref obj, serializationStream, typeMetaData.SeedKernelTypeShema, SerializationShema, TypeClassSerializators);

        return (Guid)obj;
    }

    public Type ResolveGenericTypeByTypeMetaData(TypeMetaDataRecord typeMetaDataRecord)
    {
        var schemas = SerializationShema.Where(schema => schema.SeedKernelTypeName == typeMetaDataRecord.TypeName);

        if (schemas.Any())
            return schemas.First().ClrType;

        return TypeClassSerializators[typeMetaDataRecord.Type].TryResolveTypeByTypeMetaDataRecord(typeMetaDataRecord, TypeClassSerializators);
    }

    private void TryRegisterTypeAndSubTypesByTypeMetaData(TypeMetaDataRecord typeMetaDataRecord)
    {
        var type = ResolveGenericTypeByTypeMetaData(typeMetaDataRecord);

        if (type == null || SerializationShema.Exists(shema => shema.ClrType == type))
        {
            return;
        }

        SerializationShema.Add(new SerializationTypeRecord
        {
            ClrType = type,
            SeedKernelTypeShema = typeMetaDataRecord,
            FullTypeId = TryGetTypeId(typeMetaDataRecord.TypeName)
        });

        foreach (var nestedTypeMetaData in typeMetaDataRecord.NestedTypes)
        {
            if (nestedTypeMetaData.NestedTypes == null || nestedTypeMetaData.NestedTypes.Count == 0)
            {
                continue;
            }

            type = ResolveGenericTypeByTypeMetaData(nestedTypeMetaData);

            if (type == null || SerializationShema.Exists(shema => shema.ClrType == type))
            {
                continue;
            }

            SerializationShema.Add(new SerializationTypeRecord
            {
                ClrType = type,
                SeedKernelTypeShema = nestedTypeMetaData,
                FullTypeId = TryGetTypeId(typeMetaDataRecord.TypeName)
            });

            TryRegisterTypeAndSubTypesByTypeMetaData(nestedTypeMetaData);
        }
    }

    private Guid TryGetTypeId(string SeedKernelName)
    {
        var decoder = new TypeMetaDataDecoder();

        return AllTypesList.Where(x => decoder.DecodeTypeMetaData(x.SerializationSchema).TypeName == SeedKernelName).FirstOrDefault().FullTypeId;
    }

    public void RegisterType(SerializationTypeRecord serializationTypeRecord)
    {
        if (SerializationShema.Exists(shema => shema.ClrType == serializationTypeRecord.ClrType))
        {
            return;
        }

        TryRegisterTypeAndSubTypesByTypeMetaData(serializationTypeRecord.SeedKernelTypeShema);

        if (!SerializationShema.Exists(shema => shema.ClrType == serializationTypeRecord.ClrType))
        {
            serializationTypeRecord.FullTypeId = TryGetTypeId(serializationTypeRecord.SeedKernelTypeName);
            SerializationShema.Add(serializationTypeRecord);
        }
    }

    public object Deserialize(Stream serializationStream)
    {
        Guid typeId = DeserializeHeader(serializationStream);

        if (!SerializationShema.Exists(serilizator => serilizator.FullTypeId == typeId))
        {
            throw new NotSupportedException("Not supported type for deserialization");
        }
        
        var obj = new object();

        var typeMetaData = SerializationShema.First(serilizator => serilizator.FullTypeId == typeId);

        TypeClassSerializators[typeMetaData.SeedKernelTypeClass]
            .Deserialize(ref obj, serializationStream, typeMetaData.SeedKernelTypeShema, SerializationShema, TypeClassSerializators);

        return obj;
        
    }

    public void Serialize(Stream serializationStream, object graph)
    {
        if (!SerializationShema.Exists(serilizator => serilizator.ClrType == graph.GetType()))
        {
            throw new NotSupportedException("Not supported type for serialization");
        }

        var typeMetaData = SerializationShema.First(serilizator => serilizator.ClrType == graph.GetType());

        SerializeHeader(serializationStream, typeMetaData.FullTypeId);

        TypeClassSerializators[typeMetaData.SeedKernelTypeClass]
            .Serialize(graph, serializationStream, typeMetaData.SeedKernelTypeShema, SerializationShema, TypeClassSerializators);
    }

    public void RegisterType(Type type)
    {
        if (SerializationShema.Any(schema => schema.ClrType == type))
            return;

        SeedSerializableAttribute serializableStructAttribute =
            (SeedSerializableAttribute)Attribute.GetCustomAttribute(type, typeof(SeedSerializableAttribute));

        if (serializableStructAttribute == null)
        {
            throw new NotSupportedException($"Type {type.Name} not supported as not containe serializable attribute");
        }

        var SeedTypeName = serializableStructAttribute.SeedKernelTypeName;

        var fields = type
            .GetProperties()
            .Where(prop => prop.GetCustomAttributes().Any(attrb => attrb.GetType() == typeof(SerializableStructFieldAttribute)))
            .ToList();

        if (!fields.Any())
            throw new ArgumentException($"Serializable type {type.Name} must have at least one serializable field");

        var structSchema = new List<StructFieldInformation>();
        var fieldsTypes = new List<Type>();

        foreach (var field in fields)
        {
            if (!(field
                .GetCustomAttributes()
                .Where(attrb => attrb.GetType() == typeof(SerializableStructFieldAttribute))
                .FirstOrDefault() is SerializableStructFieldAttribute fieldInfo))
                continue;

            structSchema.Add(fieldInfo.FieldInformation);
            fieldsTypes.Add(field.PropertyType);
        }

        foreach (var fieldType in fieldsTypes)
        {
            RegisterType(fieldType);
        }

        var fieldTypeRecord = new List<TypeMetaDataRecord>();

        for (int i = 0; i < structSchema.Count; i++)
        {
            if (!SerializationShema.Any(schema => schema.SeedKernelTypeName == structSchema[i].FieldTypeName))
                throw new ArgumentException($"Unknown field {structSchema[i].FieldName} type {structSchema[i].FieldTypeName}");

            var serializationSchema = SerializationShema.First(schema => schema.SeedKernelTypeName == structSchema[i].FieldTypeName);

            fieldTypeRecord.Add(serializationSchema.SeedKernelTypeShema);
        }

        var typeRecord = new SerializationTypeRecord
        {
            ClrType = type,
            SeedKernelTypeShema = new TypeMetaDataRecord
            {
                SizeOfSize = 0,
                StaticSize = 0,
                NestedTypes = fieldTypeRecord,
                Type = serializableStructAttribute.TypeClass,
                TypeName = serializableStructAttribute.SeedKernelTypeName,
                AdditionalInfo = new TypeMetaDataDecoder().EncodeStructFieldSchema(structSchema)
            },
        };

        SerializationShema.Add(typeRecord);
    }
}
