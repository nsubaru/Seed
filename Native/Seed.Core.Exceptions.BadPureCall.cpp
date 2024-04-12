#include "Seed.Core.Exceptions.BadPureCall.h"

using namespace Seed::Core;

void BadPureCall::ThrowMe()
{
	Throw(*this);
}

ExceptionString BadPureCall::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}