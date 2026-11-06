using Microsoft.Extensions.Logging;
using Seed.Core.RTTI;
using Seed.RTTI;

namespace Seed.Serialization.CustomSerializers;

/// <summary>
/// Maps native dynamic values to DynamicObject instances for binary serialization.
/// </summary>
public class DynamicObjectSerializer : ICustomClassSerializer
{
    /// <summary>
    /// The CLR type used to represent native dynamic values.
    /// </summary>
    public Type ClrType => typeof(DynamicObject);

    /// <summary>
    /// Cached native void type identifier used to encode an invalid dynamic value. An empty identifier triggers lookup.
    /// </summary>
    public UniversalTypeIdentifier VoidFullTypeId;

    /// <summary>
    /// The native RTTI name used to select the serializer for native dynamic values.
    /// </summary>
    public string TypeName => "Seed::Core::RTTI::Dynamic";

    /// <summary>
    /// The identifier type used to read and write the wrapped native type.
    /// </summary>
    public Type[] DependableTypes => new Type[]
    {
        typeof(UniversalTypeIdentifier)
    };

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        var fullTypeId = new UniversalTypeIdentifier() { };

        try
        {
            object dynFullTypeId = fullTypeId;
            serializer.Deserialize(ref dynFullTypeId, serializationStream);

            fullTypeId = (UniversalTypeIdentifier)dynFullTypeId;
            serializer.Logger?.LogTrace("Deserialize dynamic with UIID: {fullTypeId}", fullTypeId);

            var rttiType = serializer.GetAllRttiTypes().First(x => x.FullTypeId == fullTypeId);
            serializer.Logger?.LogTrace("Deserialize dynamic with Seed type: {0}", rttiType.TypeName);

            if (rttiType.TypeName == "void")
            {
                return new DynamicObject();
            }

            var typeRecord = serializer.GetSupportedTypes().First(x => x.FullTypeId == fullTypeId.Id);
            serializer.Logger?.LogTrace("Deserialize dynamic with C# type: {0}", typeRecord.ClrType.FullName);

            object obj = serializer.CreateObjectByTypeRecord(typeRecord);
            serializer.Deserialize(ref obj, serializationStream);

            return new DynamicObject(obj, fullTypeId.Id);
        }
        catch (InvalidOperationException)
        {
            var rttiType = serializer.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == fullTypeId);

            if (rttiType != null)
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type: {rttiType.TypeName}");
            }
            else
            {
                throw new ApplicationException($"Deserialize unserializable type. Seed type known. Uuid: {fullTypeId.Id}. Kernel version: {fullTypeId.KernelVersion}");
            }
        }
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
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
