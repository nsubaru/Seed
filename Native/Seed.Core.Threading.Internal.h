#pragma once
#include "Seed.Core.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{
			namespace Internals
			{
				InternalApiCall SeedKernelCall void InitSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall void InitCondVal(void* handle);

				InternalApiCall SeedKernelCall void AcquireLockExclusiveSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall void ReleaseLockExclusiveSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall void AcquireLockSharedSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall void ReleaseLockSharedSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall bool TryAcquireLockSharedSRWLock(void* PSRWLock);

				InternalApiCall SeedKernelCall void WakeOneCondVal(void* condVar);

				InternalApiCall SeedKernelCall void WaitCondVal(void* condVar, void* srwLock);

				InternalApiCall SeedKernelCall void PulseNtEvent(void* handle);
				InternalApiCall SeedKernelCall void SetNtEvent(void* handle);

				InternalApiCall SeedKernelCall void ResetNtEvent(void* handle);

				InternalApiCall SeedKernelCall __declspec(noreturn) void __CxxRethrowImpl();
			}
		}
	}
}