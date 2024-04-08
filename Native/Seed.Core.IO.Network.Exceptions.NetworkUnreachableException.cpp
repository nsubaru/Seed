#include "Seed.Core.IO.Network.Exceptions.NetworkUnreachableException.h"

using namespace Seed::Core;

void Seed::Core::IO::Network::NetworkUnreachableException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::IO::Network::NetworkUnreachableException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}