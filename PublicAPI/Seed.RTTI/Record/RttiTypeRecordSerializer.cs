using Microsoft.Extensions.Logging;
using Seed.Core.RTTI;
using Seed.Serialization;

namespace Seed.RTTI.Record;

/// <summary>
/// Serializes RTTI references by their native type identifier, including the zero identifier for an empty record.
/// </summary>
public class RttiTypeRecordSerializer : ICustomClassSerializer
{
    /// <summary>
    /// Managed RTTI record type handled by this serializer.
    /// </summary>
    public Type ClrType => typeof(RttiTypeRecord);

    /// <summary>
    /// Native RTTI reference type handled by this serializer.
    /// </summary>
    public string TypeName => "Seed::Core::RTTI::RttiTypeRecord";

    /// <summary>
    /// Identifier type required to encode RTTI references.
    /// </summary>
    public Type[] DependableTypes => new Type[] { typeof(UniversalTypeIdentifier) };

    /// <summary>
    /// Resolves a native RTTI reference, preserving an empty record when its identifier is zero.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native type identifier.</param>
    /// <param name="serializer">Serializer containing this logical session's RTTI records.</param>
    /// <returns>The matching RTTI record, or an invalid empty record for the zero identifier.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        serializer.Logger?.Log(LogLevel.Trace, "Start deserialization RttiTypeRecord");
        object data = new UniversalTypeIdentifier();
        serializer.Deserialize(ref data, serializationStream);
        var fullTypeId = (UniversalTypeIdentifier)data;
        serializer.Logger?.Log(LogLevel.Trace, "Deserialization UniversalTypeIdentifier. Value {fullTypeId}", fullTypeId);

        if (fullTypeId.Id == Guid.Empty)
        {
            return new RttiTypeRecord();
        }

        var record = serializer.GetAllRttiTypes().FirstOrDefault(x => x.FullTypeId == fullTypeId);

        if (record == null)
        {
            serializer.Logger?.Log(LogLevel.Error, "RttiTypeRecord {FullTypeId} not found during deserialization", fullTypeId);
            throw new InvalidDataException($"RTTI type record {fullTypeId} was not found during deserialization");
        }

        serializer.Logger?.Log(LogLevel.Trace, "Successfully deserialized record with typename {0}", record.TypeName);
        return record;
    }

    /// <summary>
    /// Writes a native type identifier, using zero for an invalid empty record.
    /// </summary>
    /// <param name="data">RTTI record to serialize.</param>
    /// <param name="serializationStream">Stream receiving the native type identifier.</param>
    /// <param name="serializer">Serializer used to write the identifier.</param>
    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        serializer.Logger?.Log(LogLevel.Trace, "Start serializing RttiTypeRecord");
        var obj = data as RttiTypeRecord;
        object fullTypeId = obj!.FullTypeId;
        serializer.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord as UniversalTypeIdentifier");
        serializer.Serialize(fullTypeId, serializationStream);
        serializer.Logger?.Log(LogLevel.Trace, "Serializing RttiTypeRecord completed");
    }
}
