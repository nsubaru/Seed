#include "Seed.Core.Exceptions.NtException.h"

using namespace Seed::Core;

void NtException::ThrowMe()&&
{
	Throw(MoveRValue(*this));
}

ExceptionString NtException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L". The system call returns the code ",
		BaseString<Memory::ExceptionHeap>::Int32asHex(statusCode.Code), L". Msg: ",
		statusCode.Message, L" ",
		location.toString(),
		(innerException == nullptr) ? L"."C : L". Also there is a nested exception: " + InnerException->toString()
	);
}