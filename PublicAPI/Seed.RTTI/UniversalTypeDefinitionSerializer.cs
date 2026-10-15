using Seed.Core.RTTI;
using Seed.Serialization;

namespace Seed.RTTI;

public class UniversalTypeDefinitionSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(UniversalTypeIdentifier);

    public string TypeName => "Seed::Core::RTTI::UniversalTypeIdentifier";

    public Type[] DependableTypes => new Type[]
    {
        typeof(UUID)
    };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        dynamic uuid = new UUID();
        serializer.Deserialize(ref uuid, serializationStream);

        return new UniversalTypeIdentifier() { Id = uuid };
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var uuid = ((UniversalTypeIdentifier)data).Id;
        
        serializer.Serialize(uuid, serializationStream);
    }
}
