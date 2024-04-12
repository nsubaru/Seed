#include "Seed.Core.IO.Network.Exceptions.SoketException.h"

using namespace Seed::Core;

void Seed::Core::IO::Network::SoketException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::IO::Network::SoketException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}