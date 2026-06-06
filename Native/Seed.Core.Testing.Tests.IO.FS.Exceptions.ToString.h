#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.IO.h"

namespace Seed::Core::Testing::Test::IO::FS::Exceptions::ToString
{
	SeedTestDef(CantReadMoreDataThanInExistInFileExceptionToString);
	SeedTestDef(CantWriteDataAfterEofExceptionToString);
	SeedTestDef(DirectoryIsNotFileExceptionToString);
	SeedTestDef(FileBusyExceptionToString);
	SeedTestDef(FileExistExceptionToString);
	SeedTestDef(FileIsCorruptedExceptionToString);
	SeedTestDef(FileModeNoSupportedByOperationExceptionToString);
	SeedTestDef(FileNameInvalidExceptionToString);
	SeedTestDef(FileNotExistExceptionToString);
	SeedTestDef(FsIoExceptionToString);
	SeedTestDef(GenericFileExceptionToString);
	SeedTestDef(PathNotFoundExceptionToString);
}