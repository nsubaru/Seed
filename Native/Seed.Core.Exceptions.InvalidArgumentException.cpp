#include "Seed.Core.Exceptions.InvalidArgumentException.h"

using namespace Seed::Core;

void InvalidArgumentException::ThrowMe()
{
	Throw(*this);
}


ExceptionString InvalidArgumentException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". Invalid ", ArgNameValue,
		L" argument. Additional description: ",
		DescriptionValue, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}