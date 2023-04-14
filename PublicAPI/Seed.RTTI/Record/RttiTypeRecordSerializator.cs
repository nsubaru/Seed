using Seed.Serialization;

namespace Seed.RTTI.Record;

public class RttiTypeRecordSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(RttiTypeRecord);

    public string TypeName => "Seed::Core::RTTI::RttiTypeRecord";

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic dynData = new UniversalTypeIdentificator();
        serializator.Deserialize(ref dynData, serializationStream);
        return serializator.GetAllRttiTypes().First(x => x.FullTypeId == dynData);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var obj = data as RttiTypeRecord;
        dynamic dynData = obj.FullTypeId;
        serializator.Serialize(dynData, serializationStream);
    }
}
