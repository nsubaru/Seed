#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>Примітив синхронізації який дозволяє одночасне читання і есклюзивний запис</summary>
			class SRWLock
			{
			private:
				typedef struct _RTL_SRWLOCK
				{
					void* Ptr;
				} RTL_SRWLOCK, *PRTL_SRWLOCK;

				RTL_SRWLOCK lock;
			public:
				SRWLock()
				{
					extern void InitSRWLock(void*);
					InitSRWLock(&lock);
				}
				///<summary>Виконує <see cref="Funct"/> з еклюзивним блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в ексклюзивному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T>
				auto ExclusiveLock(T & Funct) -> decltype(Funct())
				{
					extern void AcquireLockExclusiveSRWLock(void*);
					extern void ReleaseLockExclusiveSRWLock(void*);

					if constexpr (StaticAsserts::IsSame<decltype(Funct()), void>())
					{
						AcquireLockExclusiveSRWLock(&lock);
						Funct();
						ReleaseLockExclusiveSRWLock(&lock);
					}
					else
					{
						AcquireLockExclusiveSRWLock(&lock);
						auto && ret = MoveRValue(Scope());
						ReleaseLockExclusiveSRWLock(&lock);
						return MoveRValue(ret);
					}
				}
				///<summary>Виконує <see cref="Funct"/> з cпільни блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в спільному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T>
				auto ShareLock(T & Funct) -> decltype(Funct())
				{
					extern void AcquireLockSharedSRWLock(void*);
					extern void ReleaseLockSharedSRWLock(void*);

					if constexpr (StaticAsserts::IsSame<decltype(Funct()), void>())
					{
						AcquireLockSharedSRWLock(&lock);
						Funct();
						ReleaseLockSharedSRWLock(&lock);
					}
					else
					{
						AcquireLockSharedSRWLock(&lock);
						auto && ret = MoveRValue(Funct());
						ReleaseLockSharedSRWLock(&lock);
						return MoveRValue(ret);
					}
				}
			};
		}
	}
}