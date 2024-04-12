#include "Seed.Core.IO.Network.Exceptions.InvalidDriverArgumentException.h"

using namespace Seed::Core;

void Seed::Core::IO::Network::InvalidDriverArgumentException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::IO::Network::InvalidDriverArgumentException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}