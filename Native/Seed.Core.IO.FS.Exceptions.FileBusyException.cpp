#include "Seed.Core.IO.FS.Exceptions.FileBusyException.h"

void Seed::Core::IO::FS::FileBusyException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
