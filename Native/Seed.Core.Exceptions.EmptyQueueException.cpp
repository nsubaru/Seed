#include "Seed.Core.Exceptions.EmptyQueueException.h"

using namespace Seed::Core;

void EmptyQueueException::ThrowMe()
{
	Throw(*this);
}

ExceptionString EmptyQueueException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}