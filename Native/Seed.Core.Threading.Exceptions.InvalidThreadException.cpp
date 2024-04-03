#include "Seed.Core.Threading.Exceptions.InvalidThreadException.h"

using namespace Seed::Core;

void Seed::Core::Threading::InvalidThreadException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::Threading::InvalidThreadException::toString() const
{
	return ExceptionString::Combine(
		this->exceptionName,
		L". Thread not fount with ID ", ExceptionString::toString(tid), L". ",
		Location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}