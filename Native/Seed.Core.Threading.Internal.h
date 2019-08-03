// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

InternalApiCall OptimizeExport(extern void InitSRWLock(void* PSRWLock),
	{
	RtlInitializeSRWLock(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	}
);
InternalApiCall OptimizeExport(extern void AcquireLockExclusiveSRWLock(void* PSRWLock),
	{
	RtlAcquireSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	}
);
InternalApiCall OptimizeExport(extern void ReleaseLockExclusiveSRWLock(void* PSRWLock),
	{
	RtlReleaseSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	}
);
InternalApiCall OptimizeExport(extern void AcquireLockSharedSRWLock(void* PSRWLock),
	{
		RtlAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	}
);
InternalApiCall OptimizeExport(extern void ReleaseLockSharedSRWLock(void* PSRWLock),
	{
		RtlReleaseSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	}
);

InternalApiCall OptimizeExport(extern void PulseNtEvent(void* handle),
	{
		LONG state;
		NtPulseEvent(handle, &state);
	}
);
InternalApiCall OptimizeExport(extern void SetNtEvent(void* handle),
	{
		LONG state;
		NtSetEvent(handle, &state);
	}
);
InternalApiCall OptimizeExport(extern void ResetNtEvent(void* handle),
	{
		LONG state;
		NtResetEvent(handle, &state);
	}
);

InternalApiCall OptimizeExport(extern void InitCondVal(void* handle),
	{
		RtlInitializeConditionVariable((PRTL_CONDITION_VARIABLE)handle);
	}
);

InternalApiCall OptimizeExport(extern void WaitCondVal(void* condVar, void* srwLock),
	{
		RtlSleepConditionVariableSRW((PRTL_CONDITION_VARIABLE)condVar, (PRTL_SRWLOCK)srwLock, NULL, 0);
	}
);

InternalApiCall OptimizeExport(extern void WakeOneCondVal(void* condVar),
	{
		RtlWakeConditionVariable((PRTL_CONDITION_VARIABLE)condVar);
	}
);

InternalApiCall OptimizeExport(extern void WakeAllCondVal(void* condVar),
	{
		RtlWakeAllConditionVariable((PRTL_CONDITION_VARIABLE)condVar);
	}
);


