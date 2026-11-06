using System.Net.NetworkInformation;

namespace Seed.Serialization.CustomSerializers;

/// <summary>
/// Maps native physical network addresses to PhysicalAddress instances for binary serialization.
/// </summary>
public class PhysicalAddressSerializer : ICustomClassSerializer
{
    /// <summary>
    /// The CLR type used to represent native physical network addresses.
    /// </summary>
    public Type ClrType => typeof(PhysicalAddress);

    /// <summary>
    /// The native RTTI name used to select the serializer for native physical network addresses.
    /// </summary>
    public string TypeName => "Seed::Core::IO::Network::PhysicalAddress";

    /// <summary>
    /// The byte-array type used for the address bytes.
    /// </summary>
    public Type[] DependableTypes => new Type[]
    {
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
        UInt64 size = 0;
        object sizeDynamic = size;
        serializer.Deserialize(ref sizeDynamic, serializationStream);

        size = (UInt64)sizeDynamic;
        var dataBuffer = new byte[(int)size];
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

        serializer.Serialize((UInt64)dataBuffer.Length, serializationStream);
        serializationStream.Write(dataBuffer);
    }
}
