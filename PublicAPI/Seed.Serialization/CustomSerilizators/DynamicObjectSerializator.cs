namespace Seed.Serialization.CustomSerilizators;

public class DynamicObjectSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(DynamicObject);

    public string TypeName => "Seed::Core::RTTI::Dynamic";

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        var fullTypeId = new RTTI.UniversalTypeIdentificator() { };
        dynamic dynFullTypeId = fullTypeId;

        serializator.Deserialize(ref dynFullTypeId, serializationStream);

        var typeRecord = serializator.GetSupportedTypes().First(x => x.FullTypeId == dynFullTypeId.Id);
        dynamic obj = serializator.CreateObjectByTypeRecord(typeRecord);

        serializator.Deserialize(ref obj, serializationStream);

        return new DynamicObject(obj, dynFullTypeId.Id);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var dynamicObject = (DynamicObject)data;
        var fullTypeId = new RTTI.UniversalTypeIdentificator() { Id = dynamicObject.FullTypeId };

        serializator.Serialize(fullTypeId, serializationStream);

        if (dynamicObject.IsValid)
        {
            serializator.Serialize(dynamicObject.Value!, serializationStream);
        }
    }
}
