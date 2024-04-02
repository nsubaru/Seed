#include "Seed.Core.Exceptions.StringToLong.h"

using namespace Seed::Core;

void StringToLong::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString StringToLong::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". Requested size ", ExceptionString::toString(requiredSize),
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}