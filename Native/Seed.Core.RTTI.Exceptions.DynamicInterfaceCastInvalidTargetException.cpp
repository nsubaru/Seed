#include "Seed.Core.RTTI.Exceptions.DynamicInterfaceCastInvalidTargetException.h"

using namespace Seed::Core;

void Seed::Core::RTTI::Exceptions::DynamicInterfaceCastInvalidTargetException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::RTTI::Exceptions::DynamicInterfaceCastInvalidTargetException::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		(castType ? L"Durring casting to: "C
		 : L"Durring casting from: "C), target.TypeName,
		L"Expected type: ", expectedlType.TypeName, L". ",
		L"Actual type: ", actualType.TypeName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}