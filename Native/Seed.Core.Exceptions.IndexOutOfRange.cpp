#include "Seed.Core.Exceptions.IndexOutOfRange.h"

using namespace Seed::Core;

void IndexOutOfRange::ThrowMe()
{
	Throw(*this);
}

ExceptionString IndexOutOfRange::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". During the try to access the element by index ", ExceptionString::toString(RequiredIndex),
		L" if maximal allowed index ", ExceptionString::toString(MaxAllowedIndex),
		L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}