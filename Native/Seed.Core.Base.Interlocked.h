#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Synchonize {
			///<summary>Атомарний обмін адресів покажчиків</summary>
			///<param name = "Target">Адрес першого покажчика</param>
			///<param name = "Val">Другий покажчик</param>
			///<returns>Вертає перший покажчик до обміну</returns>
			DeprecatedCall("Raw pointer use")
				__forceinline void* InterlockedExchangePointer(void* volatile* Target, void* Val) {
				extern void* implInterlockedExchangePointer(void* volatile* Target, void* Val);
				return implInterlockedExchangePointer(Target, Val);
			}

			///<summary>Бар'єр пам'яті синхронізує доступ до пам'яті для кількох потоків</summary>
			__forceinline void MemoryBarrier() {
				extern void implMemoryBarrier();
				//_ReadWriteBarrier();
				implMemoryBarrier();
			}
		}
	}
}