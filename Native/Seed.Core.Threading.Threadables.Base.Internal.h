#pragma once
#include "Seed.Core.Threading.Threadables.Base.h"

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