using Microsoft.Extensions.Logging;
using Seed.RTTI;

namespace Seed.Serialization.CustomSerilizators;

public class DynamicObjectSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(DynamicObject);
    public UniversalTypeIdentifier VoidFullTypeId;

    public string TypeName => "Seed::Core::RTTI::Dynamic";

    public Type[] DependableTypes => new Type[] { typeof(UniversalTypeIdentifier) };

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        var fullTypeId = new UniversalTypeIdentifier() { };

        try
        {    
            dynamic dynFullTypeId = fullTypeId;

            serializator.Deserialize(ref dynFullTypeId, serializationStream);

            fullTypeId = dynFullTypeId;

            serializator.Logger?.LogTrace("Deserialize dynamic with UIID: {fullTypeId}", fullTypeId);

            var rttiType = serializator.GetAllRttiTypes().First(x => x.FullTypeId == fullTypeId);

            serializator.Logger?.LogTrace("Deserialize dynamic with Seed type: {0}", rttiType.TypeName);

            if (rttiType.TypeName == "void")
            {
                return new DynamicObject();
            }

            var typeRecord = serializator.GetSupportedTypes().First(x => x.FullTypeId == fullTypeId.Id);

            serializator.Logger?.LogTrace("Deserialize dynamic with C# type: {0}", typeRecord.ClrType.FullName);

            dynamic obj = serializator.CreateObjectByTypeRecord(typeRecord);

            serializator.Deserialize(ref obj, serializationStream);

            return new DynamicObject(obj, fullTypeId.Id);
        }
        catch(InvalidOperationException) 
        {
            var rttiType = serializator.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == fullTypeId);

            if( rttiType != null )
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type: {rttiType.TypeName}");
            }
            else
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type uknown. Uuid: {fullTypeId.Id}. Kernel version: {fullTypeId.KernelVersion}");
            }
        }
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var dynamicObject = (DynamicObject)data;

        if (VoidFullTypeId.Id == Guid.Empty)
        {
            VoidFullTypeId = serializator.GetAllRttiTypes().First(x => x.TypeName == "void").FullTypeId;
        }

        var fullTypeId = dynamicObject.IsValid 
            ? new RTTI.UniversalTypeIdentifier() { Id = dynamicObject.FullTypeId }
            : VoidFullTypeId;

        serializator.Logger?.LogTrace("Serilize dynamic with UIID: {fullTypeId}", fullTypeId);

        serializator.Serialize(fullTypeId, serializationStream);

        if (dynamicObject.IsValid)
        {
            serializator.Serialize(dynamicObject.Value!, serializationStream);
        }
    }
}
