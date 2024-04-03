#include "Seed.Core.RTTI.Exceptions.InvalidRttiTypeException.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::InvalidRttiTypeException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::RTTI::Exceptions::InvalidRttiTypeException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Actual type: ", actual.TypeName,
		L"Expected type: ", expected.TypeName,
		L". ", location.toString(),
		(innerException == nullptr) ? L"."C : (L". There is also a nested exception: " + InnerException->toString())
	);
}