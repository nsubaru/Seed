using Seed.Core;

namespace Seed.Serialization.CustomSerializers;

public class Utf8StringSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(Utf8String);

    public string TypeName => Constants.Types.Utf8String;

    public Type[] DependableTypes => new[]
    {
        typeof(UInt64),
        typeof(byte[])
    };

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        UInt64 size        = 0;
        object sizeDynamic = size;

        serializer.Deserialize(ref sizeDynamic, serializationStream);

        size = (UInt64)sizeDynamic;

        if (size > int.MaxValue)
        {
            throw new InvalidDataException($"UTF-8 string payload is too large for managed deserialization: {size} bytes.");
        }

        var dataBuffer = new byte[(int)size];
        serializationStream.ReadExactly(dataBuffer);

        return new Utf8String(dataBuffer);
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var utf8       = data is Utf8String value ? value : Utf8String.FromString(data?.ToString());
        var dataBuffer = utf8.ToArray();

        serializer.Serialize((UInt64)dataBuffer.Length, serializationStream);
        serializationStream.Write(dataBuffer);
    }
}
