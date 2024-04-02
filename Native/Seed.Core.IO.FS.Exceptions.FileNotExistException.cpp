#include "Seed.Core.IO.FS.Exceptions.FileNotExistException.h"

void Seed::Core::IO::FS::FileNotExistException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
