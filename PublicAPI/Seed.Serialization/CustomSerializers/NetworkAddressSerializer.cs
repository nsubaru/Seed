using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;

namespace Seed.Serialization.CustomSerializers;

public class NetworkAddressSerializer : ICustomClassSerializer
{
    public Type ClrType => typeof(IPEndPoint);

    public string TypeName => "Seed::Core::IO::Network::NetworkAddress";

    public Type[] DependableTypes => Array.Empty<Type>();

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        UInt16 family = 0, port = 0;
        object headDyn = family;

        serializer.Deserialize(ref headDyn, serializationStream);
        family = (UInt16)headDyn;
        headDyn = port;

        serializer.Deserialize(ref headDyn, serializationStream);
        port = (UInt16)headDyn;
        IPEndPoint retObj;

        switch (family)
        {
            case 2:
                uint integerData = 0;
                Span<byte> v4AddrSpan = stackalloc byte[4];

                serializationStream.ReadExactly(v4AddrSpan);

                integerData = BitConverter.ToUInt32(v4AddrSpan);
                retObj      = new IPEndPoint(new IPAddress(integerData), port);

                break;

            case 23:
                object shortDyn;
                // we ignore flowinfo since C# don't support it
                uint flowInfo = 0;
                shortDyn = flowInfo;
                serializer.Deserialize(ref shortDyn, serializationStream);

                uint scopeId = 0;
                Span<byte> addr = stackalloc byte[16];
                serializationStream.ReadExactly(addr);

                shortDyn = scopeId;
                serializer.Deserialize(ref shortDyn, serializationStream);

                scopeId = (uint)shortDyn;
                retObj  = new IPEndPoint(new IPAddress(addr, scopeId), port);

                break;

            default:
                throw new ArgumentException(nameof(family));
        }

        return retObj;
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        IPEndPoint address = (IPEndPoint)data;
        object addrHeadDyn = (UInt16)address.AddressFamily;

        serializer.Serialize(addrHeadDyn, serializationStream);
        addrHeadDyn = (UInt16)address.Port;

        serializer.Serialize(addrHeadDyn, serializationStream);
        int outBytes = 0;

        switch (address.AddressFamily)
        {
            case System.Net.Sockets.AddressFamily.InterNetwork:
                byte[] v4Addr        = new byte[4];
                Span<byte> v4AddrSpan = new Span<byte>(v4Addr);

                address.Address.TryWriteBytes(v4AddrSpan, out outBytes);
                serializationStream.Write(v4Addr);

                return;
            case System.Net.Sockets.AddressFamily.InterNetworkV6:
                // We should write flowinfo since C++ expect it
                uint flowInfo = 0;
                serializer.Serialize(flowInfo, serializationStream);

                Span<byte> v6AddrSpan = stackalloc byte[16];
                address.Address.TryWriteBytes(v6AddrSpan, out outBytes);
                serializationStream.Write(v6AddrSpan);

                uint scopeId    = (uint)address.Address.ScopeId;
                object scopeDyn = scopeId;
                serializer.Serialize(scopeDyn, serializationStream);

                return;
                
            default:
                throw new ArgumentException(nameof(address.AddressFamily));
        }
    }
}
