#include "Seed.Core.IO.FS.Exceptions.FileNameInvalidException.h"

void Seed::Core::IO::FS::FileNameInvalidException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
