using System.Data;
using System.Reflection;
using System.Runtime.Serialization;
using Seed.RTTI;
using System.IO;
using System;
using System.Linq;
using System.Collections.Generic;
using Seed.Serialization;
using Seed.RTTI.Record;
using System.Collections;

namespace Seed.CTTI;

public class SeedBinarySerializator
{
    RTTISection rttiSection;
    ISeedSerializator seedSerializator;
    Dictionary<RttiTypeRecord, Type> mapingSheme = new Dictionary<RttiTypeRecord, Type>();

    public SeedBinarySerializator(RTTISection rttiSection)
    {
        this.rttiSection = rttiSection;
        SeedSerializatorFactory serializatorFactory = new SeedSerializatorFactory(rttiSection);

        seedSerializator = serializatorFactory.GetSeedSerializator();

        mapingSheme = seedSerializator.GetSupportedTypes().OrderBy(x => x.RttiType.TypeName).OrderBy(x => x.RttiType.TypeClass).ToDictionary(x => x.RttiType, x => x.ClrType);
    }

    private void SerializeImpl<T>(Stream serializationStream, T graph)
    {
        seedSerializator.Serialize(graph, serializationStream);
    }

    private void SerializeArrayImpl<T>(Stream serializationStream, T graph, RttiTypeRecord record)
    {
        switch (record.TypeClass)
        {
            case ETypes.FixedCollection:
                {
                    var mirror = graph as IList;
                    if (mirror.Count != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
            case ETypes.FixedArray:
                {
                    var mirror = graph as Array;
                    if (mirror.Length != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
        }

        SerializeImpl<T>(serializationStream, graph);
    }

    public void Serialize(Stream serializationStream, object graph, RttiTypeRecord targetType = null)
    {
        Type type = null;
        UniversalTypeIdentificator Id = new UniversalTypeIdentificator();

        if (targetType != null)
        {
            if (!mapingSheme.ContainsKey(targetType))
            {
                throw new ArgumentException();
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
            throw new InvalidCastException();
        }

        seedSerializator.Serialize(Id, serializationStream);

        var record = rttiSection.AllRegisteredTypes.First(x => x.FullTypeId == Id);

        if (record.TypeClass == ETypes.FixedArray || record.TypeClass == ETypes.FixedCollection)
        {
            GetType().GetRuntimeMethods().Where(x => x.Name == "SerializeArrayImpl")
                .First()
                .MakeGenericMethod(type)
                .Invoke(this, new object[] { serializationStream, graph, record });
        }
        else
        {
            GetType().GetRuntimeMethods().Where(x => x.Name == "SerializeImpl")
                .First()
                .MakeGenericMethod(type)
                .Invoke(this, new object[] { serializationStream, graph });
        }
    }

    private T CreateObject<T>(Stream serializationStream)
    {
        Type type = typeof(T);

        object obj = null;

        if (type == typeof(String))
        {
            obj = string.Empty;
        }
        else if (type == typeof(DynamicObject))
        {
            UniversalTypeIdentificator Id = new UniversalTypeIdentificator();
            dynamic ID = Id;

            var streamStartPos = serializationStream.Position;
            seedSerializator.Deserialize(ref ID, serializationStream);
            serializationStream.Position = streamStartPos;

            Id = ID;
            
            if (!mapingSheme.Any(x => x.Key.FullTypeId == Id))
                throw new InvalidCastException();

            Type typePair = mapingSheme.First(x => x.Key.FullTypeId == Id).Value;
            var innerObj = GetType().GetRuntimeMethods().Where(x => x.Name == "CreateObject")
                            .First()
                            .MakeGenericMethod(typePair)
                            .Invoke(this, new object[] { serializationStream });

            obj = new DynamicObject(innerObj);
        }
        else if (type.BaseType == typeof(ValueTuple).BaseType)
        {
            object[] args = new object[type.GenericTypeArguments.Length];
            for (int i = 0; i < type.GenericTypeArguments.Length; i++)
            {
                args[i] = GetType().GetRuntimeMethods().Where(x => x.Name == "CreateObject")
                            .First()
                            .MakeGenericMethod(type.GenericTypeArguments[i])
                            .Invoke(this, new object[] { serializationStream});
            }

            obj = Activator.CreateInstance(type, args);
        }
        else
        { 
            obj = Activator.CreateInstance(typeof(T)); 
        }

        return (T)obj;
    }

    private T DeserializeImpl<T>(Stream serializationStream)
    {
        Type type = typeof(T);

        object beginObj = null;

        beginObj = CreateObject<T>(serializationStream);

        T obj = (T) beginObj;
        dynamic data = obj;
        seedSerializator.Deserialize(ref data, serializationStream);

        return data;
    }

    private T DeserializeArrayImpl<T>(Stream serializationStream, RttiTypeRecord record)
    {
        object beginObj = null;

        switch (record.TypeClass)
        {
            case ETypes.FixedCollection:
                beginObj = CreateObject<T>(serializationStream);
                break;
            case ETypes.FixedArray:
                beginObj = Array.CreateInstance(typeof(T).GetElementType(), (int)record.ArrayItemsCount);
                break;
            default:
                throw new InvalidCastException();
        }

        T obj = (T)beginObj;
        dynamic data = obj;
        seedSerializator.Deserialize(ref data, serializationStream);

        switch (record.TypeClass)
        {
            case ETypes.FixedCollection:
                {
                    var mirror = obj as IList;
                    if (mirror.Count != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
            case ETypes.FixedArray:
                {
                    var mirror = obj as Array;
                    if (mirror.Length != (int)record.ArrayItemsCount)
                    {
                        throw new InvalidCastException();
                    }
                    break;
                }
        }

        return data;
    }

    public object Deserialize(Stream serializationStream, RttiTypeRecord targetType = null)
    {
        if (targetType != null && !mapingSheme.ContainsKey(targetType))
            throw new ArgumentException();

        Type type = null;
        UniversalTypeIdentificator Id = new UniversalTypeIdentificator();
        dynamic ID = Id;

        seedSerializator.Deserialize(ref ID, serializationStream);

        Id = ID;

        if (targetType == null && mapingSheme.Any(x => x.Key.FullTypeId == Id))
        {
            type = mapingSheme.First(x => x.Key.FullTypeId == Id).Value;
        }
        else if(targetType != null && mapingSheme.Any(x => x.Key.FullTypeId == Id) && Id == targetType.FullTypeId)
        {
            type = mapingSheme[targetType];
        }
        else
        {
            throw new InvalidCastException();
        }

        object retObj = null;

        var record = rttiSection.AllRegisteredTypes.First(x => x.FullTypeId == Id);

        if (record.TypeClass == ETypes.FixedArray || record.TypeClass == ETypes.FixedCollection)
        {
            retObj = GetType().GetRuntimeMethods().Where(x => x.Name == "DeserializeArrayImpl")
                .First()
                .MakeGenericMethod(type)
                .Invoke(this, new object[] { serializationStream, record });
        }
        else
        {
            retObj = GetType().GetRuntimeMethods().Where(x => x.Name == "DeserializeImpl")
                .First()
                .MakeGenericMethod(type)
                .Invoke(this, new object[] { serializationStream });
        }

        return retObj;
    }
}
