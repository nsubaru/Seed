#include "Seed.Core.RTTI.Exceptions.DynamicCtorCallInvalidArgumentType.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicCtorCallInvalidTargetException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicCtorCallInvalidTargetException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		L"Durring calling ctor for type: ", target.Type.TypeName,
		L"Expected type: ", expectedlType.TypeName, L". ",
		L"Actual type: ", actualType.TypeName, L". ",
		L"On position: ", ExceptionString::toString(position), L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}