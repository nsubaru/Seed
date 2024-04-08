#include "Seed.Core.Exceptions.StackOverflowException.h"

using namespace Seed::Core;

void StackOverflowException::ThrowMe()
{
	Throw(*this);
}


ExceptionString StackOverflowException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(), L". "
	);
}