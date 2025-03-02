using System.Net.NetworkInformation;

namespace Seed.Serialization.CustomSerializators;

public class PhysicalAddressSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(PhysicalAddress);

    public string TypeName => "Seed::Core::IO::Network::PhysicalAddress";

    public Type[] DependableTypes => new Type[] { typeof(byte[]) };

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        UInt64 size = 0;
        dynamic sizeDynamic = size;
        serializator.Deserialize(ref sizeDynamic, serializationStream);
        size = sizeDynamic;

        var dataBuffer = new byte[(int) size];

        serializationStream.ReadExactly(dataBuffer);

        return new PhysicalAddress(dataBuffer);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        byte[] dataBuffer = (data as PhysicalAddress)!.GetAddressBytes();

        serializator.Serialize((UInt64) dataBuffer.Length, serializationStream);

        serializationStream.Write(dataBuffer);
    }
}
