#include "Seed.Core.Threading.Exceptions.DeadlockExpectedException.h"

using namespace Seed::Core;

void Seed::Core::Threading::DeadlockExpectedException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::Threading::DeadlockExpectedException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		Location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}