#include "Seed.Core.RTTI.Exceptions.DynamicCtorCallException.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCtorCallException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCtorCallException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling ctor for: ", target.Type.TypeName,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}