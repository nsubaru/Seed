#include "Seed.Core.Exceptions.NotFoundException.h"

using namespace Seed::Core;

void NotFoundException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString NotFoundException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}