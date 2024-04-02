#include "Seed.Core.IO.FS.Exceptions.PathNotFoundException.h"

void Seed::Core::IO::FS::PathNotFoundException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
