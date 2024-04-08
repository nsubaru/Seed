#include "Seed.Core.Exceptions.DivideByZero.h"

using namespace Seed::Core;

void DivideByZero::ThrowMe()
{
	Throw(*this);
}

ExceptionString DivideByZero::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}