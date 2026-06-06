using Microsoft.Extensions.Logging;
using Seed.Core.RTTI;
using Seed.Serialization;

namespace Seed.RTTI.Record;

public class RttiTypeRecordSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(RttiTypeRecord);

    public string TypeName => "Seed::Core::RTTI::RttiTypeRecord";

    public Type[] DependableTypes => new Type[] { typeof(UniversalTypeIdentifier) };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        serializer.Logger?.Log(LogLevel.Trace, "Start deserialization RttiTypeRecord");
        dynamic dynData = new UniversalTypeIdentifier();
        serializer.Deserialize(ref dynData, serializationStream);
        UniversalTypeIdentifier fullTypeId = dynData;
        serializer.Logger?.Log(LogLevel.Trace, "Deserialization UniversalTypeIdentifier. Value {fullTypeId}", fullTypeId);
        var record = serializer.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == dynData);

        if (record == null)
        {
            serializer.Logger?.Log(LogLevel.Error, "RttiTypeRecord not found during deserialization", fullTypeId);
            throw new NullReferenceException();
        }
        else
        {
            serializer.Logger?.Log(LogLevel.Trace, "Successfully deserialized record with typename {0}", record.TypeName);
            return record;
        }
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        serializer.Logger?.Log(LogLevel.Trace, "Start serializing RttiTypeRecord");
        var obj = data as RttiTypeRecord;
        dynamic dynData = obj!.FullTypeId;
        serializer.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord as UniversalTypeIdentifier");
        serializer.Serialize(dynData, serializationStream);
        serializer.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord completed");
    }
}
