#include "Seed.Core.Exceptions.FullQueueException.h"

using namespace Seed::Core;

void FullQueueException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString FullQueueException::toString() const
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