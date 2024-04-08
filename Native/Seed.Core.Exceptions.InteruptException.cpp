#include "Seed.Core.Exceptions.InteruptException.h"

using namespace Seed::Core;

void InteruptException::ThrowMe()
{
	Throw(*this);
}

ExceptionString InteruptException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}