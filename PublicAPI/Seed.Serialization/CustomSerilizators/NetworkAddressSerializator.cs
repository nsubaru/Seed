using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization.CustomSerilizators;
public class NetworkAddressSerializator : ICustomClassSerializator
{
    public Type ClrType => typeof(IPEndPoint);

    public string TypeName => "Seed::Core::IO::Network::NetworkAddress";

    public Type[] DependableTypes => Array.Empty<Type>();

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        UInt16 family = 0, port = 0;

        dynamic headDyn = family;
        serializator.Deserialize(ref headDyn, serializationStream);
        family = headDyn;

        headDyn = port;
        serializator.Deserialize(ref headDyn, serializationStream);
        port = headDyn;

        IPEndPoint retObj;

        switch (family)
        {
            case 2:
                uint integerData = 0;
                Span<byte> v4AddrSpan = stackalloc byte[4];
                serializationStream.ReadExactly(v4AddrSpan);

                integerData = BitConverter.ToUInt32(v4AddrSpan);

                retObj = new IPEndPoint(new IPAddress(integerData), port);

                break; 

            case 23:
                dynamic shortDyn;

                // we ignore flowinfo since C# don't support it
                uint flowInfo = 0;
                shortDyn = flowInfo;

                serializator.Deserialize(ref shortDyn, serializationStream);

                uint scopeId = 0;

                Span<byte> addr = stackalloc byte[16];
                serializationStream.ReadExactly(addr);

                shortDyn = scopeId;
                serializator.Deserialize(ref shortDyn, serializationStream);
                scopeId = shortDyn;

                retObj = new IPEndPoint(new IPAddress(addr, scopeId), port);
                break;

            default: throw new ArgumentException(nameof(family));
        }

        return retObj;
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        IPEndPoint address = (IPEndPoint)data;
        dynamic addrHeadDyn = (UInt16) address.AddressFamily;

        serializator.Serialize(addrHeadDyn, serializationStream);

        addrHeadDyn = (UInt16) address.Port;
        serializator.Serialize(addrHeadDyn, serializationStream);

        int outBytes = 0;

        switch (address.AddressFamily)
        {
            case System.Net.Sockets.AddressFamily.InterNetwork:
                byte[] v4Addr = new byte[4];
                Span<byte> v4AddrSpan = new Span<byte>(v4Addr);

                address.Address.TryWriteBytes(v4AddrSpan, out outBytes);
                serializationStream.Write(v4Addr);
                return;

            case System.Net.Sockets.AddressFamily.InterNetworkV6:
                // We should write flowinfo since C++ expect it
                uint flowInfo = 0;
                serializator.Serialize(flowInfo, serializationStream);
                
                Span<byte> v6AddrSpan = stackalloc byte[16];

                address.Address.TryWriteBytes(v6AddrSpan, out outBytes); 
                serializationStream.Write(v6AddrSpan);

                uint scopeId = (uint) address.Address.ScopeId;
                dynamic scopeDyn = scopeId;

                serializator.Serialize(scopeDyn, serializationStream);

                return;

            default : throw new ArgumentException(nameof(address.AddressFamily));
        }
    }
}
