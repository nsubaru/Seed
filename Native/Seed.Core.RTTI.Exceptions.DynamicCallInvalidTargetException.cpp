#include "Seed.Core.RTTI.Exceptions.DynamicCallInvalidTargetException.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCallInvalidTargetException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCallInvalidTargetException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling: ", target.Name,
		L"Expected type: ", expectedlType.TypeName, L". ",
		L"Actual type: ", actualType.TypeName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}