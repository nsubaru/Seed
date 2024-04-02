#include "Seed.Core.Exceptions.FullStackException.h"

using namespace Seed::Core;

void FullStackException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString FullStackException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". Maximal allowed size - ",
		ExceptionString::toString(maxSize),
		L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}
