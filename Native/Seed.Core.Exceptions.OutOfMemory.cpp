#include "Seed.Core.Exceptions.OutOfMemory.h"

using namespace Seed::Core;

void OutOfMemory::ThrowMe()
{
	Throw(*this);
}

ExceptionString OutOfMemory::toString() const
{
	return ExceptionString::Combine(
		exceptionName,
		L" in ", HeapName, L" heap",
		L" during the allocation ", ExceptionString::toString(TryAllocate),
		L" bytes, at the moment of exception occurring was allocated ",
		ExceptionString::toString(AllocatedBefore), L" bytes in ",
		ExceptionString::toString(MemBlksCountBefore),
		(MemBlksCountBefore == 1 ? L" memory block. "C : L"memory blocks. "C),
		location.toString(),
		(innerException == nullptr) ? L"." : (L". Also there is a nested exception: " + InnerException->toString())
	);
}