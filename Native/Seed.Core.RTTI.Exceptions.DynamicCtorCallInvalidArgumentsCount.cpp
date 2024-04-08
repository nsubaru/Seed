#include "Seed.Core.RTTI.Exceptions.DynamicCtorCallInvalidArgumentsCount.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCtorCallInvalidArgumentsCountException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCtorCallInvalidArgumentsCountException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling ctor for type: ", target.Type.TypeName,
		L"Expected count: ", ExceptionString::toString(target.Arguments.Count), L". ",
		L"Actual count: ", ExceptionString::toString(actualCount), L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}