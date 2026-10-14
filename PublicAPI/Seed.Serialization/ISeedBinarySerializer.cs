using Seed.RTTI.Record;

namespace Seed.Serialization;

public interface ISeedBinarySerializer
{
    void Serialize(Stream serializationStream, object graph, RttiTypeRecord? targetType = null);
    object Deserialize(Stream serializationStream, RttiTypeRecord? targetType = null);
}
