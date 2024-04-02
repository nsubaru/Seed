#include "Seed.Core.Exceptions.InternalError.h"

using namespace Seed::Core;

void InternalError::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString InternalError::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(), L". "
	);
}