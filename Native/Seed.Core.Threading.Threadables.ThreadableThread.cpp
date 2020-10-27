#include "Seed.Core.Threading.Threadables.ThreadableThread.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;
using namespace Seed::Threading;
using namespace Seed::Threading::Threadables;

ThreadableRef Seed::Core::Threading::Threadables::RegisterThreadThreadable(const Thread& thread, volatile EThreadabledStates* state)
{
	HANDLE hThread = nullptr;
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &hThread, GENERIC_ALL, 0, 0);
    auto threadable = new ThreadableThread(thread);
	return RegisterThreadable(MoveRValue(threadable), state);
}

void Seed::Core::Threading::Threadables::UnRegisterThreadThreadable(ThreadableRef&& ref)
{
	UnRegisterThreadable(MoveRValue(ref));
}
