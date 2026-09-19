using System.Net.NetworkInformation;

namespace Seed.Serialization.CustomSerializers;

public class PhysicalAddressSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(PhysicalAddress);

    public string TypeName => "Seed::Core::IO::Network::PhysicalAddress";

    public Type[] DependableTypes => new Type[] { typeof(byte[]) };

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        UInt64 size = 0;
        object sizeDynamic = size;
        serializer.Deserialize(ref sizeDynamic, serializationStream);
        size = (UInt64)sizeDynamic;

        var dataBuffer = new byte[(int) size];

        serializationStream.ReadExactly(dataBuffer);

        return new PhysicalAddress(dataBuffer);
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        byte[] dataBuffer = (data as PhysicalAddress)!.GetAddressBytes();

        serializer.Serialize((UInt64) dataBuffer.Length, serializationStream);

        serializationStream.Write(dataBuffer);
    }
}
