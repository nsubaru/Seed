using Seed.RTTI.Record;

namespace Seed.Serialization;

/// <summary>
/// Reads and writes values in the Seed binary serialization format.
/// </summary>
public interface ISeedBinarySerializer
{
    /// <summary>
    /// Writes the native type identifier and value payload to the stream.
    /// </summary>
    /// <param name="serializationStream">The destination stream for the encoded value.</param>
    /// <param name="graph">The non-null CLR value to encode.</param>
    /// <param name="targetType">The optional native type record for the value. When omitted, the serializer uses the CLR type mapping.</param>
    void Serialize(Stream serializationStream, object graph, RttiTypeRecord? targetType = null);

    /// <summary>
    /// Reads the native type identifier and reconstructs the value from the stream.
    /// </summary>
    /// <param name="serializationStream">The source stream positioned at the encoded value.</param>
    /// <param name="targetType">The optional expected native type record. Its identifier must match the identifier stored in the stream.</param>
    /// <returns>The reconstructed CLR value.</returns>
    object Deserialize(Stream serializationStream, RttiTypeRecord? targetType = null);
}
