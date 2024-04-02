#include "Seed.Core.IO.IPC.Pipes.Exceptions.InvalidPipeModeException.h"

using namespace Seed::Core;

void Seed::Core::IO::IPC::Pipes::InvalidPipeModeException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString Seed::Core::IO::IPC::Pipes::InvalidPipeModeException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		location.toString(),
		(innerException == nullptr) ? L"."C : (L". Also there is a nested exception: "C + InnerException->toString())
	);
}