#pragma once
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Threading.Thread.h"
#include "Seed.Core.Threading.TaskSheduler.h"
#include "Seed.Core.Threading.ThreadPool.h"
#include "Seed.Core.Threading.UnifiedThreadPool.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				using ThreadableRef = Seed::Core::Containers::ConcurencySensitiveList<Seed::Core::SmartPtr<Seed::Core::Threading::IThreadable>>::Iterator;

				ThreadableRef RegisterThreadable(IThreadable*&& threadable, volatile EThreadabledStates* State);
				void UnRegisterThreadable(ThreadableRef&& ref);
			}
		}

	}
}