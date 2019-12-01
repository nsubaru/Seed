// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.DataFlowControler.h"
#include "Seed.Core.Threading.LockPrimitives.h"

#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
#pragma warning(disable : 4996)

//HACK: Thinck how remove it. It use for rethrow when Seed exception API not allowed
void _declspec(noreturn) Seed::Core::Threading::__rethrow()
{
	throw; //-V667
}

Seed::Core::Threading::DataFlowControler::DataFlowControler() :
	lock({nullptr}), producer({nullptr}), concumer({nullptr}), isConcumerWork(false), isProducerWork(false)
{
	::InitSRWLock(&lock);
	::InitCondVal(&producer);
	::InitCondVal(&concumer);
}

void Seed::Core::Threading::AcquireLockExclusiveSRWLock(void* PSRWLock)
{
	::AcquireLockExclusiveSRWLock(PSRWLock);
}

void Seed::Core::Threading::ReleaseLockExclusiveSRWLock(void* PSRWLock)
{
	::ReleaseLockExclusiveSRWLock(PSRWLock);
}

void Seed::Core::Threading::AcquireLockSharedSRWLock(void* PSRWLock)
{
	::AcquireLockSharedSRWLock(PSRWLock);
}

void Seed::Core::Threading::ReleaseLockSharedSRWLock(void* PSRWLock)
{
	::ReleaseLockSharedSRWLock(PSRWLock);
}

bool Seed::Core::Threading::TryAcquireLockSharedSRWLock(void* PSRWLock)
{
	return ::TryAcquireLockSharedSRWLock(PSRWLock);
}

void Seed::Core::Threading::WakeOneCondVal(void* condVar)
{
	::WakeOneCondVal(condVar);
}

void Seed::Core::Threading::WaitCondVal(void* condVar, void* srwLock)
{
	::WaitCondVal(condVar, srwLock);
}

#pragma warning(pop)