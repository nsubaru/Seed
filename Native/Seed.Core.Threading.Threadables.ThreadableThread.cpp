#include "Seed.Core.Threading.Threadables.ThreadableThread.h"

#include "Seed.Core.Threading.Threadables.Base.Internal.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

using namespace Seed;
using namespace Seed::Threading;
using namespace Seed::Threading::Threadables;

ThreadableRef Seed::Core::Threading::Threadables::RegisterThreadThreadable(const Thread& thread,
	volatile EThreadabledStates* state,
	volatile EThreadabledStates* secondState)
{
    auto threadable = new ThreadableThread(thread);
	*state = EThreadabledStates::Executing;
	_ReadWriteBarrier();
	return RegisterIThreadable(MoveRValue(threadable), state, secondState);
}

void Seed::Core::Threading::Threadables::UnRegisterThreadThreadable(ThreadableRef&& ref)
{
	UnRegisterIThreadable(MoveRValue(ref));
	_ReadWriteBarrier();
}

#pragma warning(pop)
