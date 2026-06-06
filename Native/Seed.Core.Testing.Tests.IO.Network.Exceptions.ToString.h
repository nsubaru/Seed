#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.IO.h"

namespace Seed::Core::Testing::Test::IO::Network::Exceptions::ToString
{
	SeedTestDef(AddressAlreadyBusyExceptionToString);
	SeedTestDef(ConnectionAlreadyCloseExceptionToString);
	SeedTestDef(ConnectionClosedBadExceptionToString);
	SeedTestDef(InvalidAddressExceptionToString);
	SeedTestDef(InvalidAddressFamilyExceptionToString);
	SeedTestDef(InvalidDriverArgumentExceptionToString);
	SeedTestDef(InvalidNetworkFamilyExceptionToString);
	SeedTestDef(InvalidStateExceptionToString);
	SeedTestDef(IOTimeoutExceptionToString);
	SeedTestDef(NetworkUnreachableExceptionToString);
	SeedTestDef(SocketExceptionToString);
}