#include "Seed.Core.Exceptions.AlreadyExistException.h"

using namespace Seed::Core;

void AlreadyExistException::ThrowMe()
{
	Throw(*this);
}

ExceptionString AlreadyExistException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}