#include "Seed.Core.IO.Exceptions.OperationCancelledException.h"

void Seed::Core::IO::OperationCancelledException::ThrowMe()
{
	Throw(*this);
}