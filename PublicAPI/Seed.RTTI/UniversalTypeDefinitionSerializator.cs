using Seed.Serialization;

namespace Seed.RTTI;

public class UniversalTypeDefinitionSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(UniversalTypeIdentificator);

    public string TypeName => "Seed::Core::RTTI::UniversalTypeIdentificator";

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic uuid = new UUID();

        serializator.Deserialize(ref uuid, serializationStream);
        return new UniversalTypeIdentificator() { Id = uuid };
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var uuid = ((UniversalTypeIdentificator)data).Id;

        serializator.Serialize(uuid, serializationStream);
    }
}
