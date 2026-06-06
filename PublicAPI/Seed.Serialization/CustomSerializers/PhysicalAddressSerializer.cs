using System.Net.NetworkInformation;

namespace Seed.Serialization.CustomSerializers;

public class PhysicalAddressSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(PhysicalAddress);

    public string TypeName => "Seed::Core::IO::Network::PhysicalAddress";

    public Type[] DependableTypes => new Type[] { typeof(byte[]) };

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        UInt64 size = 0;
        dynamic sizeDynamic = size;
        serializer.Deserialize(ref sizeDynamic, serializationStream);
        size = sizeDynamic;

        var dataBuffer = new byte[(int) size];

        serializationStream.ReadExactly(dataBuffer);

        return new PhysicalAddress(dataBuffer);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        byte[] dataBuffer = (data as PhysicalAddress)!.GetAddressBytes();

        serializer.Serialize((UInt64) dataBuffer.Length, serializationStream);

        serializationStream.Write(dataBuffer);
    }
}
