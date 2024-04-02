#include "Seed.Core.DependencyInjection.DIRegisterServiceException.h"

using namespace Seed::Core;

void Seed::Core::DI::DIRegisterServiceException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::DI::DIRegisterServiceException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". Service name: ", typeName, L". ",
		location.toString(), L". "
	);
}