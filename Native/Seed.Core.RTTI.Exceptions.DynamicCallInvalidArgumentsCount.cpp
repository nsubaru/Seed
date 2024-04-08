#include "Seed.Core.RTTI.Exceptions.DynamicCallInvalidArgumentsCount.h"
using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCallInvalidArgumentsCountException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCallInvalidArgumentsCountException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling: ", target.Name,
		L"Expected count: ", ExceptionString::toString(target.Arguments.Count - 1), L". ",
		L"Actual count: ", ExceptionString::toString(actualCount), L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}