using Seed.Serialization;

namespace Seed.RTTI;

public class RTTISectionSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(RTTISection);
    public string TypeName => Constants.Types.RTTISection;

    public Type[] DependableTypes => new Type[] { typeof(List<UInt8>) };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        dynamic data = new byte[1];

        serializer.Deserialize(ref data, serializationStream);

        return new RTTISection((data as byte[])!);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        dynamic rawData = (data as RTTISection)!.data.ToList();

        serializer.Serialize(rawData, serializationStream);
    }
}
