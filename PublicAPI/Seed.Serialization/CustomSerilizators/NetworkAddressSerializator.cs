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
                dynamic v4addrDyn = integerData;
                serializator.Deserialize(ref v4addrDyn, serializationStream);
                integerData = v4addrDyn;

                retObj = new IPEndPoint(new IPAddress(integerData), port);
                break; 

            case 23:
                uint flowInfo = 0, scopeId = 0;
                dynamic shortDyn = flowInfo;
                serializator.Deserialize(ref shortDyn, serializationStream);
                flowInfo = shortDyn;

                byte[] addr = new byte[16];
                dynamic v6addrDyn = addr;
                serializator.Deserialize (ref v6addrDyn, serializationStream);
                addr = v6addrDyn;

                shortDyn = scopeId;
                serializator.Deserialize(ref shortDyn, serializationStream);
                scopeId = shortDyn;

                retObj = new IPEndPoint(new IPAddress(addr, scopeId), port);
                break;
            default: throw new ArgumentException();
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
                
                dynamic v4addrDyn = v4Addr;
                serializator.Serialize(v4addrDyn, serializationStream);
                return;

            case System.Net.Sockets.AddressFamily.InterNetworkV6:
                serializator.Serialize(0, serializationStream);
                
                byte[] v6Addr = new byte[16];
                Span<byte> v6AddrSpan = new Span<byte>(v6Addr);

                address.Address.TryWriteBytes(v6AddrSpan, out outBytes);

                dynamic v6addrDyn = v6Addr;
                serializator.Serialize(v6addrDyn, serializationStream);

                uint scopeId = (uint) address.Address.ScopeId;
                dynamic scopeDyn = scopeId;
                serializator.Serialize(scopeDyn, serializationStream);
                return;

            default : throw new ArgumentException();
        }
    }
}
