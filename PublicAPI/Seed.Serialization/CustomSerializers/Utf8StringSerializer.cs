using Seed.Core;

namespace Seed.Serialization.CustomSerializers;

public class Utf8StringSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(Utf8String);

    public string TypeName => Constants.Types.Utf8String;

    public Type[] DependableTypes => new[] { typeof(UInt64), typeof(byte[]) };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        UInt64 size = 0;
        dynamic sizeDynamic = size;
        serializer.Deserialize(ref sizeDynamic, serializationStream);
        size = sizeDynamic;

        if (size > int.MaxValue)
            throw new InvalidDataException($"UTF-8 string payload is too large for managed deserialization: {size} bytes.");

        var dataBuffer = new byte[(int)size];
        serializationStream.ReadExactly(dataBuffer);

        return new Utf8String(dataBuffer);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var utf8 = data is Utf8String value ? value : Utf8String.FromString(data?.ToString());
        var dataBuffer = utf8.ToArray();

        serializer.Serialize((UInt64)dataBuffer.Length, serializationStream);
        serializationStream.Write(dataBuffer);
    }
}
