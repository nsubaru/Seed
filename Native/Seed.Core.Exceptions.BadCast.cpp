#include "Seed.Core.Exceptions.BadCast.h"

using namespace Seed::Core;

void BadCast::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString BadCast::toString() const
{
	return ExceptionString::Combine(
		exceptionName, L". ",
		location.toString(), L". "
	);
}