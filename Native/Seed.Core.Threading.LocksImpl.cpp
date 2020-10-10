// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
#pragma warning(disable : 4996)

Seed::Core::Threading::DataFlowControler::DataFlowControler() :
	lock({ nullptr }), producer({ nullptr }), concumer({ nullptr }), isConcumerWork(false), isProducerWork(false) {
	Internals::InitSRWLock(&lock);
	Internals::InitCondVal(&producer);
	Internals::InitCondVal(&concumer);
}


#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

void Seed::Core::Threading::Internals::InitSRWLock(void* PSRWLock) {
	RtlInitializeSRWLock(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

void Seed::Core::Threading::Internals::InitCondVal(void* handle) {
	RtlInitializeConditionVariable((PRTL_CONDITION_VARIABLE) handle);
}

void Seed::Core::Threading::Internals::AcquireLockExclusiveSRWLock(void * PSRWLock)
{
	RtlAcquireSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

void Seed::Core::Threading::Internals::ReleaseLockExclusiveSRWLock(void* PSRWLock) {
	RtlReleaseSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

void Seed::Core::Threading::Internals::AcquireLockSharedSRWLock(void* PSRWLock) {
	RtlAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

void Seed::Core::Threading::Internals::ReleaseLockSharedSRWLock(void* PSRWLock) {
	RtlReleaseSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
	_ReadWriteBarrier();
}

bool Seed::Core::Threading::Internals::TryAcquireLockSharedSRWLock(void* PSRWLock) {
	return RtlTryAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock)) == 1;
}

void Seed::Core::Threading::Internals::WakeOneCondVal(void* condVar) {
	RtlWakeConditionVariable((PRTL_CONDITION_VARIABLE) condVar);
}

void Seed::Core::Threading::Internals::WaitCondVal(void* condVar, void* srwLock) {
	RtlSleepConditionVariableSRW((PRTL_CONDITION_VARIABLE) condVar, (PRTL_SRWLOCK) srwLock, 0, 0);
}

void Seed::Core::Threading::Internals::PulseNtEvent(void* handle) {
	LONG state;
	NtPulseEvent(handle, &state);
}

void Seed::Core::Threading::Internals::SetNtEvent(void* handle) {
	LONG state;
	NtSetEvent(handle, &state);
}

void Seed::Core::Threading::Internals::ResetNtEvent(void* handle) {
	LONG state;
	NtResetEvent(handle, &state);
}

#include "Seed.Core.Exceptions.h"

void Seed::Core::Threading::Internals::__CxxRethrowImpl() {
	ReThrow;
}

#pragma warning(pop)