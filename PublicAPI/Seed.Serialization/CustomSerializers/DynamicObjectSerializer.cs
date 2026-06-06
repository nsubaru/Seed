using Microsoft.Extensions.Logging;
using Seed.Core.RTTI;
using Seed.RTTI;

namespace Seed.Serialization.CustomSerializers;

public class DynamicObjectSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(DynamicObject);
    public UniversalTypeIdentifier VoidFullTypeId;

    public string TypeName => "Seed::Core::RTTI::Dynamic";

    public Type[] DependableTypes => new Type[] { typeof(UniversalTypeIdentifier) };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        var fullTypeId = new UniversalTypeIdentifier() { };

        try
        {    
            dynamic dynFullTypeId = fullTypeId;

            serializer.Deserialize(ref dynFullTypeId, serializationStream);

            fullTypeId = dynFullTypeId;

            serializer.Logger?.LogTrace("Deserialize dynamic with UIID: {fullTypeId}", fullTypeId);

            var rttiType = serializer.GetAllRttiTypes().First(x => x.FullTypeId == fullTypeId);

            serializer.Logger?.LogTrace("Deserialize dynamic with Seed type: {0}", rttiType.TypeName);

            if (rttiType.TypeName == "void")
            {
                return new DynamicObject();
            }

            var typeRecord = serializer.GetSupportedTypes().First(x => x.FullTypeId == fullTypeId.Id);

            serializer.Logger?.LogTrace("Deserialize dynamic with C# type: {0}", typeRecord.ClrType.FullName);

            dynamic obj = serializer.CreateObjectByTypeRecord(typeRecord);

            serializer.Deserialize(ref obj, serializationStream);

            return new DynamicObject(obj, fullTypeId.Id);
        }
        catch(InvalidOperationException) 
        {
            var rttiType = serializer.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == fullTypeId);

            if( rttiType != null )
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type: {rttiType.TypeName}");
            }
            else
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type known. Uuid: {fullTypeId.Id}. Kernel version: {fullTypeId.KernelVersion}");
            }
        }
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var dynamicObject = (DynamicObject)data;

        if (VoidFullTypeId.Id == Guid.Empty)
        {
            VoidFullTypeId = serializer.GetAllRttiTypes().First(x => x.TypeName == "void").FullTypeId;
        }

        var fullTypeId = dynamicObject.IsValid 
            ? new UniversalTypeIdentifier() { Id = dynamicObject.FullTypeId }
            : VoidFullTypeId;

        serializer.Logger?.LogTrace("Serialize dynamic with UIID: {fullTypeId}", fullTypeId);

        serializer.Serialize(fullTypeId, serializationStream);

        if (dynamicObject.IsValid)
        {
            serializer.Serialize(dynamicObject.Value!, serializationStream);
        }
    }
}
