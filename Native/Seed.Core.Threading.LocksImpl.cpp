// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Threading.LockPrimitives.h"

#pragma warning(push)
#pragma warning(disable : 4996)

//Imports internal functions
extern void InitSRWLock(void* PSRWLock);
static void AcquireLockExclusiveSRWLock(void* PSRWLock);
static void ReleaseLockExclusiveSRWLock(void* PSRWLock);
static void AcquireLockSharedSRWLock(void* PSRWLock);
static void ReleaseLockSharedSRWLock(void* PSRWLock);

extern void PulseNtEvent(void* handle);
extern void SetNtEvent(void* handle);
extern void ResetNtEvent(void* handle);

static void InitCondVal(void* handle);

static void WaitCondVal(void* condVar, void* srwLock);
static void WakeOneCondVal(void* condVar);
static void WakeAllCondVal(void* condVar);
static bool TryAcquireLockSharedSRWLock(void* PSRWLock);


//HACK: Thinck how remove it. It use for rethrow when Seed exception API not allowed
void _declspec(noreturn) Seed::Core::Threading::__rethrow() {
	throw; //-V667
}

Seed::Core::Threading::DataFlowControler::DataFlowControler() :
	lock({ nullptr }), producer({ nullptr }), concumer({ nullptr }), isConcumerWork(false), isProducerWork(false) {
	::InitSRWLock(&lock);
	::InitCondVal(&producer);
	::InitCondVal(&concumer);
}

void Seed::Core::Threading::AcquireLockExclusiveSRWLock(void* PSRWLock) {
	::AcquireLockExclusiveSRWLock(PSRWLock);
}

void Seed::Core::Threading::ReleaseLockExclusiveSRWLock(void* PSRWLock) {
	::ReleaseLockExclusiveSRWLock(PSRWLock);
}

void Seed::Core::Threading::AcquireLockSharedSRWLock(void* PSRWLock) {
	::AcquireLockSharedSRWLock(PSRWLock);
}

void Seed::Core::Threading::ReleaseLockSharedSRWLock(void* PSRWLock) {
	::ReleaseLockSharedSRWLock(PSRWLock);
}

bool Seed::Core::Threading::TryAcquireLockSharedSRWLock(void* PSRWLock) {
	return ::TryAcquireLockSharedSRWLock(PSRWLock);
}

void Seed::Core::Threading::WakeOneCondVal(void* condVar) {
	::WakeOneCondVal(condVar);
}

void Seed::Core::Threading::WaitCondVal(void* condVar, void* srwLock) {
	::WaitCondVal(condVar, srwLock);
}

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"


extern forceinline void InitSRWLock(void* PSRWLock) {
	RtlInitializeSRWLock(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

static forceinline void AcquireLockExclusiveSRWLock(void* PSRWLock) {
	RtlAcquireSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

static forceinline void ReleaseLockExclusiveSRWLock(void* PSRWLock) {
	RtlReleaseSRWLockExclusive(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

static forceinline void AcquireLockSharedSRWLock(void* PSRWLock) {
	RtlAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

static forceinline void ReleaseLockSharedSRWLock(void* PSRWLock) {
	RtlReleaseSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock));
}

extern forceinline void PulseNtEvent(void* handle) {
	LONG state;
	NtPulseEvent(handle, &state);
}

extern forceinline void SetNtEvent(void* handle) {
	LONG state;
	NtSetEvent(handle, &state);
}

extern forceinline void ResetNtEvent(void* handle) {
	LONG state;
	NtResetEvent(handle, &state);
}

static forceinline void InitCondVal(void* handle) {
	RtlInitializeConditionVariable((PRTL_CONDITION_VARIABLE) handle);
}

static forceinline void WaitCondVal(void* condVar, void* srwLock) {
	RtlSleepConditionVariableSRW((PRTL_CONDITION_VARIABLE) condVar, (PRTL_SRWLOCK) srwLock, NULL, 0);
}

static forceinline void WakeOneCondVal(void* condVar) {
	RtlWakeConditionVariable((PRTL_CONDITION_VARIABLE) condVar);
}

static forceinline void WakeAllCondVal(void* condVar) {
	RtlWakeAllConditionVariable((PRTL_CONDITION_VARIABLE) condVar);
}

static forceinline bool TryAcquireLockSharedSRWLock(void* PSRWLock) {
	return RtlTryAcquireSRWLockShared(reinterpret_cast<PRTL_SRWLOCK>(PSRWLock)) == TRUE;
}

#pragma warning(pop)