#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			//TODO: Implement it
			class GuardLock {
			private:
				struct LockT {
					void* _;
				};
			private:
				LockT lock, nonLocked, locked;
				volatile bool IsLocked;
				volatile bool IsUnlocked;
			public:
				GuardLock();
				
				template<typename Lambda>
				auto Locked(Lambda l) -> decltype(Lambda()()) {

				}

				template<typename Lambda>
				auto Unlocked(Lambda l) -> decltype(Lambda()()) {

				}
			};
		}
	}
}