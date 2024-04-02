#include "Seed.Core.Exceptions.InvalidHandleUsage.h"

using namespace Seed::Core;

void InvalidHandleUsage::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString InvalidHandleUsage::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}