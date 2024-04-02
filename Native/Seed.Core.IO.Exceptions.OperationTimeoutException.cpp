#include "Seed.Core.IO.Exceptions.OperationTimeoutException.h"

void Seed::Core::IO::OperationTimeoutException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
