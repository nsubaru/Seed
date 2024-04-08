#include "Seed.Core.Exceptions.IntegerOverflowException.h"

using namespace Seed::Core;

void IntegerOverflowException::ThrowMe()
{
	Throw(*this);
}


ExceptionString IntegerOverflowException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}
