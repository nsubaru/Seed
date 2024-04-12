#include "Seed.Core.Events.EventNotFoundException.h"

void Seed::Core::EventNotFoundException::ThrowMe()
{
	Throw(*this);
}
