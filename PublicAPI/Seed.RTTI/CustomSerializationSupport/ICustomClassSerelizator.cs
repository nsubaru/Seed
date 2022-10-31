namespace Seed.Serialization;

/// <summary>
/// Interface for custom class serelization/deserelization.
/// </summary>
public interface ICustomClassSerelizator
{
    /// <summary>
    /// Custom class .Net CLR type
    /// </summary>
    Type ClrType { get; }

    /// <summary>
    /// Custom class name in the Seed kernel
    /// </summary>
    string TypeName { get; }

    /// <summary>
    /// Serializes data into binnary string
    /// </summary>
    /// <param name="data">Data to serialize</param>
    /// <param name="serializationStream">Target stream</param>
    void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator);

    /// <summary>
    /// Deserializes data from binnary string
    /// </summary>
    /// <param name="serializationStream">Data stream</param>
    /// <returns>Deserialized object</returns>
    object Deserialize(Stream serializationStream, ISeedSerializator serializator);
}
