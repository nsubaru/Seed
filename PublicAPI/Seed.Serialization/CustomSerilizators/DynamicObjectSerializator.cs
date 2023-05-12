using Microsoft.Extensions.Logging;
using Seed.RTTI;

namespace Seed.Serialization.CustomSerilizators;

public class DynamicObjectSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(DynamicObject);
    public UniversalTypeIdentificator VoidFullTypeId;

    public string TypeName => "Seed::Core::RTTI::Dynamic";

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        var fullTypeId = new RTTI.UniversalTypeIdentificator() { };
        dynamic dynFullTypeId = fullTypeId;

        serializator.Deserialize(ref dynFullTypeId, serializationStream);

        fullTypeId = dynFullTypeId;

        serializator.Logger?.LogTrace("Deserilize dynamic with UIID: {fullTypeId}", fullTypeId);

        var rttiType = serializator.GetAllRttiTypes().First(x => x.FullTypeId == fullTypeId);

        serializator.Logger?.LogTrace("Deserilize dynamic with Seed type: {0}", rttiType.TypeName);

        if (rttiType.TypeName == "void")
        {
            return new DynamicObject();
        }

        var typeRecord = serializator.GetSupportedTypes().First(x => x.FullTypeId == fullTypeId.Id);

        serializator.Logger?.LogTrace("Deserilize dynamic with C# type: {0}", typeRecord.ClrType.FullName);

        dynamic obj = serializator.CreateObjectByTypeRecord(typeRecord);

        serializator.Deserialize(ref obj, serializationStream);

        return new DynamicObject(obj, fullTypeId.Id);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var dynamicObject = (DynamicObject)data;

        if (VoidFullTypeId.Id == Guid.Empty)
        {
            VoidFullTypeId = serializator.GetAllRttiTypes().First(x => x.TypeName == "void").FullTypeId;
        }

        var fullTypeId = dynamicObject.IsValid 
            ? new RTTI.UniversalTypeIdentificator() { Id = dynamicObject.FullTypeId }
            : VoidFullTypeId;

        serializator.Logger?.LogTrace("Serilize dynamic with UIID: {fullTypeId}", fullTypeId);

        serializator.Serialize(fullTypeId, serializationStream);

        if (dynamicObject.IsValid)
        {
            serializator.Serialize(dynamicObject.Value!, serializationStream);
        }
    }
}
