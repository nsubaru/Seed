#include "Seed.Core.Threading.Threadables.Base.h"

#include "Seed.Core.Threading.TLS.GlobalTLSBlock.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

extern Seed::Core::Containers::ConcurencySensitiveList<Seed::Core::SmartPtr<Seed::Core::Threading::IThreadable>>* AllActiveThreadable;

Seed::Core::Threading::Threadables::ThreadableRef Seed::Core::Threading::Threadables::RegisterThreadable(IThreadable*&& threadable, volatile EThreadabledStates* State)
{
	Threading::ConfigureTls(threadable, State);
	return AllActiveThreadable->AddLast(threadable->MakeRef());
}

void Seed::Core::Threading::Threadables::UnRegisterThreadable(ThreadableRef&& ref)
{
	AllActiveThreadable->Remove(ref);
	Threading::DestroyTls();
}
