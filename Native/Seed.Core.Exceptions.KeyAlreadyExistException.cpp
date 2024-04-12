#include "Seed.Core.Exceptions.KeyAlreadyExistException.h"

void Seed::Core::KeyAlreadyExistException::ThrowMe()
{
	Throw(*this);
}
