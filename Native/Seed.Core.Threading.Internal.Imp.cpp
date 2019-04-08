#include "Seed.Core.Threading.Internal.h"

InternalApiCall
inline void InitSRWLock(void* PSRWLock)
{
	RtlInitializeSRWLock(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}
InternalApiCall
inline void AcquireLockExclusiveSRWLock(void* PSRWLock)
{
	RtlAcquireSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}
InternalApiCall
inline void ReleaseLockExclusiveSRWLock(void* PSRWLock)
{
	RtlReleaseSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}
InternalApiCall
inline void AcquireLockSharedSRWLock(void* PSRWLock)
{
	RtlAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}
InternalApiCall
inline void ReleaseLockSharedSRWLock(void* PSRWLock)
{
	RtlReleaseSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

void PulseNtEvent(void* handle)
{
	LONG state;
	NtPulseEvent(handle, &state);
}

void SetNtEvent(void* handle)
{
	LONG state;
	NtSetEvent(handle, &state);
}

void ResetNtEvent(void* handle)
{
	LONG state;
	NtResetEvent(handle, &state);
}