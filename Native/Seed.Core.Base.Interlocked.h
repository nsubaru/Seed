#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Synchonize {
			DeprecatedCall("Raw pointer use")
				__forceinline void * InterlockedExchangePointer(void * volatile * Target, void * Val) {
				extern void * implInterlockedExchangePointer(void * volatile * Target, void * Val);
				return implInterlockedExchangePointer(Target, Val);
			}

			__forceinline void MemoryBarrier() {
				extern void implMemoryBarrier();
				_ReadWriteBarrier();
				implMemoryBarrier();
			}
		}
	}
}