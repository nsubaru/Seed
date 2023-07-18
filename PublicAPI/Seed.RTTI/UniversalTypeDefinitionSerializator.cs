using Seed.Serialization;

namespace Seed.RTTI;

public class UniversalTypeDefinitionSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(UniversalTypeIdentifier);

    public string TypeName => "Seed::Core::RTTI::UniversalTypeIdentifier";

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic uuid = new UUID();

        serializator.Deserialize(ref uuid, serializationStream);
        return new UniversalTypeIdentifier() { Id = uuid };
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var uuid = ((UniversalTypeIdentifier)data).Id;

        serializator.Serialize(uuid, serializationStream);
    }
}
