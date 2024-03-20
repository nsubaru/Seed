using Microsoft.Extensions.Logging;
using Seed.Serialization;

namespace Seed.RTTI.Record;

public class RttiTypeRecordSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(RttiTypeRecord);

    public string TypeName => "Seed::Core::RTTI::RttiTypeRecord";

    public Type[] DependableTypes => new Type[] { typeof(UniversalTypeIdentifier) };

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        serializator.Logger?.Log(LogLevel.Trace, "Start deserializing RttiTypeRecord");
        dynamic dynData = new UniversalTypeIdentifier();
        serializator.Deserialize(ref dynData, serializationStream);
        UniversalTypeIdentifier fullTypeId = dynData;
        serializator.Logger?.Log(LogLevel.Trace, "Deserializing UniversalTypeIdentifier. Value {fullTypeId}", fullTypeId);
        var record = serializator.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == dynData);

        if (record == null)
        {
            serializator.Logger?.Log(LogLevel.Error, "RttiTypeRecord not found durring deserializing", fullTypeId);
            throw new NullReferenceException();
        }
        else
        {
            serializator.Logger?.Log(LogLevel.Trace, "Successfully deserialized record with typename {0}", record.TypeName);
            return record;
        }
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        serializator.Logger?.Log(LogLevel.Trace, "Start serializing RttiTypeRecord");
        var obj = data as RttiTypeRecord;
        dynamic dynData = obj!.FullTypeId;
        serializator.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord as UniversalTypeIdentifier");
        serializator.Serialize(dynData, serializationStream);
        serializator.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord completed");
    }
}
