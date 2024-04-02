#include "Seed.Core.IO.IPC.Pipes.Exceptions.PipeBrokenException.h"

using namespace Seed::Core;

void Seed::Core::IO::IPC::Pipes::PipeBrokenException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::IO::IPC::Pipes::PipeBrokenException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}