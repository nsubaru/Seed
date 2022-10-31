using Seed.RTTI.Record;

namespace Seed.Serialization;

public interface ISeedBinarySerializator
{
    void Serialize(Stream serializationStream, object graph, RttiTypeRecord? targetType = null);
    object Deserialize(Stream serializationStream, RttiTypeRecord? targetType = null);
}
