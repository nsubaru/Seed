using Seed.Serialization;

namespace Seed.RTTI;

public class RTTISectionSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(RTTISection);
    public string TypeName => "Seed::Core::RTTI::RTTISection";

    public Type[] DependableTypes => new Type[] { typeof(List<UInt8>) };

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic data = new List<UInt8>();

        serializator.Deserialize(ref data, serializationStream);

        return new RTTISection((data as List<UInt8>)!.ToArray());
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic rawData = (data as RTTISection)!.data.ToList();

        serializator.Serialize(rawData, serializationStream);
    }
}
