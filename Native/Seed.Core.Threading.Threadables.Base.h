#pragma once
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Threading.Thread.h"
#include "Seed.Core.Threading.UnifiedThreadPool.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				ThreadableRef RegisterIThreadable(IThreadable*&& threadable, volatile EThreadabledStates* State);
				void UnRegisterIThreadable(ThreadableRef&& ref);
			}
		}

	}
}