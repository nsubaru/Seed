#include "Seed.Core.Threading.Threadables.ThreadableThread.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;
using namespace Seed::Threading;
using namespace Seed::Threading::Threadables;

ThreadableRef Seed::Core::Threading::Threadables::RegisterThreadThreadable(const Thread& thread, volatile EThreadabledStates* state)
{
    auto threadable = new ThreadableThread(thread);
	*state = EThreadabledStates::Executing;
	_ReadWriteBarrier();
	return RegisterIThreadable(MoveRValue(threadable), state);
}

void Seed::Core::Threading::Threadables::UnRegisterThreadThreadable(ThreadableRef&& ref)
{
	UnRegisterIThreadable(MoveRValue(ref));
	_ReadWriteBarrier();
}
