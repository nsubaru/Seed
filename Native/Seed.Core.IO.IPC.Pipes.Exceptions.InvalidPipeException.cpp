#include "Seed.Core.IO.IPC.Pipes.Exceptions.InvalidPipeException.h"

using namespace Seed::Core;

void Seed::Core::IO::IPC::Pipes::InvalidPipeException::ThrowMe()
{
	Throw(*this);
}

ExceptionString Seed::Core::IO::IPC::Pipes::InvalidPipeException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}