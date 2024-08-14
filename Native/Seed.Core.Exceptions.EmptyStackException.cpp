#include "Seed.Core.Exceptions.EmptyStackException.h"

using namespace Seed::Core;

void EmptyStackException::ThrowMe()
{
	Throw(*this);
}


ExceptionString EmptyStackException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L" ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}