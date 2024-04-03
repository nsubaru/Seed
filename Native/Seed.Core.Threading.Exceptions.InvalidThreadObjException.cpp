#include "Seed.Core.Threading.Exceptions.InvalidThreadObjException.h"
using namespace Seed::Core;

void Seed::Core::Threading::InvalidThreadObjException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::Threading::InvalidThreadObjException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		Location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}