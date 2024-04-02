#include "Seed.Core.Exceptions.InvalidFormatException.h"

using namespace Seed::Core;

void InvalidFormatException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString InvalidFormatException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}