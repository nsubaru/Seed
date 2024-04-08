#include "Seed.Core.DependencyInjection.DIResolveException.h"

using namespace Seed::Core;

void Seed::Core::DI::DIResolveException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::DI::DIResolveException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". Type name: ", typeName, L". ",
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}