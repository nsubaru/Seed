#include "Seed.Core.Events.EventAlreadyExistException.h"

void Seed::Core::EventAlreadyExistException::ThrowMe()
{
	Throw(*this);
}
