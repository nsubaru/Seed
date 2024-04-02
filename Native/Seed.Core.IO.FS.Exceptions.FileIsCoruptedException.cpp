#include "Seed.Core.IO.FS.Exceptions.FileIsCoruptedException.h"

void Seed::Core::IO::FS::FileIsCoruptedException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}
