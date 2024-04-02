#include "Seed.Core.Exceptions.EndNodeNotReferenceable.h"

using namespace Seed::Core;

void EndNodeNotReferenceable::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString EndNodeNotReferenceable::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}