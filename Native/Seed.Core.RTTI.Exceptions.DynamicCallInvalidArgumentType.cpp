#include "Seed.Core.RTTI.Exceptions.DynamicCallInvalidArgumentType.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCallInvalidArgumentTypeException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCallInvalidArgumentTypeException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling: ", target.Name,
		L"Expected type: ", expectedlType.TypeName, L". ",
		L"Actual type: ", actualType.TypeName, L". ",
		L"At position: ", ExceptionString::toString(index), L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}