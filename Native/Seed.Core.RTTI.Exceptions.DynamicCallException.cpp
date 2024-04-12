#include "Seed.Core.RTTI.Exceptions.DynamicCallException.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCallException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCallException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling: ", target.Name,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}