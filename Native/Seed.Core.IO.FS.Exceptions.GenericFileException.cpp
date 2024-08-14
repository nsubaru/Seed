#include "Seed.Core.IO.FS.Exceptions.GenericFileException.h"

using namespace Seed::Core;

void Seed::Core::IO::FS::GenericFileException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::IO::FS::GenericFileException::toString() const
{
	return ExceptionString::Combine(
		L"File exception with message: ", exceptionName, L". ",
		GetStatusCodeMessageIfAvailable(),
		L"Occurs while processing the file: \"", fileName, L"\"",
		location.toString(),
		(innerException == nullptr) ? L"."C : L". Also there is a nested exception: "C, InnerException->toString()
	);
}