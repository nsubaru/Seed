#include "Seed.Core.Exceptions.BadException.h"

using namespace Seed::Core;

void BadException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString BadException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}