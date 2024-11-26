using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Network;

[SerializableEnum("Seed::Core::Services::Network::::NetworkInterfaceAddressFlag")]
[Flags]
public enum ENetworkInterfaceAddressFlag : UInt32
{
    Broadcast = 0x00000002, // Broadcast is  supported.
    Loopback = 0x00000004, // This is loopback interface.
    PointToPoint = 0x00000008, // This is point-to-point interface.
    Multicast = 0x00000010  // Multicast is supported.
}
