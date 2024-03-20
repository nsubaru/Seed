using System;
using System.Collections.Generic;
using System.Linq;
using System.Net.NetworkInformation;
using System.Text;
using System.Threading.Tasks;

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

        byte[] dataBuffer = new byte[size];
        dynamic dataBufferDynamic = dataBuffer;

        serializator.Deserialize(ref dataBufferDynamic, serializationStream);
        dataBuffer = dataBufferDynamic;

        return new PhysicalAddress(dataBuffer);
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        byte[] dataBuffer = (data as PhysicalAddress)!.GetAddressBytes();

        serializator.Serialize((UInt64) dataBuffer.Length, serializationStream);

        serializator.Serialize(dataBuffer, serializationStream);
    }
}
