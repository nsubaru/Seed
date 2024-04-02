#include "Seed.Core.IO.Network.Exceptions.InvalidAddressFamilyException.h"

using namespace Seed::Core;

void Seed::Core::IO::Network::InvalidAddressFamilyException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::IO::Network::InvalidAddressFamilyException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}