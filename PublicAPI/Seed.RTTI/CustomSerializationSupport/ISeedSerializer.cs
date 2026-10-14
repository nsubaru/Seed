using Microsoft.Extensions.Logging;
using Seed.RTTI.Record;

namespace Seed.Serialization;

/// <summary>
/// Seed serializer interface
/// </summary>
public interface ISeedSerializer
{
    /// <summary>
    /// Serializes data into binnary string
    /// </summary>
    /// <param name="data">Data to serialize</param>
    /// <param name="serializationStream">Target stream</param>
    public void Serialize(dynamic data, Stream serializationStream);
    /// <summary>
    /// Deserializes data from binnary string
    /// </summary>
    /// <param name="data">Deserialized data target container</param>
    /// <param name="serializationStream">Data stream</param>
    public void Deserialize(ref dynamic data, Stream serializationStream);
    /// <summary>
    /// Create object instance by type
    /// </summary>
    /// <typeparam name="T">Object type</typeparam>
    /// <returns>New object</returns>
    public T CreateObject<T>();
    /// <summary>
    /// Generate type instance by type record
    /// </summary>
    /// <param name="typeRecord">Type information</param>
    /// <returns>Type instance as object</returns>
    public object CreateObjectByTypeRecord(TypeRecord typeRecord);
    /// <summary>
    /// Returns list of supprted types
    /// </summary>
    /// <returns>List of types</returns>
    public List<TypeRecord> GetSupportedTypes();
    /// <summary>
    /// Returns all known RTTI types
    /// </summary>
    /// <returns>All known RTTI types</returns>
    public IEnumerable<RttiTypeRecord> GetAllRttiTypes();
    /// <summary>
    /// Serializer logger
    /// </summary>
    public ILogger<ISeedSerializer>? Logger
    {
        get;
    }
}
