#include "Seed.Core.IO.Exceptions.Base.h"

using namespace Seed::Core;

void Seed::Core::IO::IoException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::IO::IoException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". ",
		location.toString(),
		L" ",
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}