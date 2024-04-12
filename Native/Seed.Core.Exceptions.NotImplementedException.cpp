#include "Seed.Core.Exceptions.NotImplementedException.h"

using namespace Seed::Core;

void NotImplementedException::ThrowMe()
{
	Throw(*this);
}

ExceptionString NotImplementedException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}