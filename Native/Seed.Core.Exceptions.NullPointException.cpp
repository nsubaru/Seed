#include "Seed.Core.Exceptions.NullPointException.h"

using namespace Seed::Core;

void NullPointException::ThrowMe()
{
	Throw(*this);
}

ExceptionString NullPointException::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L" for the address ", ExceptionString::UInt64asHex(GetMemoryAddress()),
		L" when trying ",
		((GetAccessType() == Types::Read) ? L"read. "C :
		 (GetAccessType() == Types::Write) ? L"write. "C
		 : L"execution. "C),
		location.toString(),
		(innerException == nullptr) ? L". "C : (L". Also there is a nested exception: " + InnerException->toString())
	);
}