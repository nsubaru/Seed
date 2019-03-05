#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Threading.RWLock.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			InternalApiCall
			__forceinline void InitSRWLock(RTL_SRWLOCK & SRWLock)
			{
				RtlInitializeSRWLock(&SRWLock);
			}
			InternalApiCall
			__forceinline void AcquireLockExclusiveSRWLock(RTL_SRWLOCK & SRWLock)
			{
				RtlAcquireSRWLockExclusive(&SRWLock);
			}
			InternalApiCall
			__forceinline void ReleaseLockExclusiveSRWLock(RTL_SRWLOCK & SRWLock)
			{
				RtlReleaseSRWLockExclusive(&SRWLock);
			}
			InternalApiCall
			__forceinline void AcquireLockSharedSRWLock(RTL_SRWLOCK & SRWLock)
			{
				RtlAcquireSRWLockShared(&SRWLock);
			}
			InternalApiCall
			__forceinline void ReleaseLockSharedSRWLock(RTL_SRWLOCK & SRWLock)
			{
				RtlReleaseSRWLockShared(&SRWLock);
			}
		}
	}
}