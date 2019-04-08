#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Threading.SRWLock.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

extern void InitSRWLock(void* PSRWLock);
extern void AcquireLockExclusiveSRWLock(void* PSRWLock);
extern void ReleaseLockExclusiveSRWLock(void* PSRWLock);
extern void AcquireLockSharedSRWLock(void* PSRWLock);
extern void ReleaseLockSharedSRWLock(void* PSRWLock);

extern void PulseNtEvent(void* handle);
extern void SetNtEvent(void* handle);
extern void ResetNtEvent(void* handle);