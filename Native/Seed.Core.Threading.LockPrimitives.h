#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>Примітив синхронізації який дозволяє одночасне читання і есклюзивний запис</summary>
			class SeedKernelCall SRWLock final
			{
			private:
				typedef struct _RTL_SRWLOCK
				{
					void* Ptr;
				} RTL_SRWLOCK, * PRTL_SRWLOCK;

				RTL_SRWLOCK lock;
			public:
				__forceinline SRWLock()
				{
					extern void InitSRWLock(void*);
					InitSRWLock(&lock);
				}
				///<summary>Виконує <see cref="Funct"/> з еклюзивним блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в ексклюзивному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T>
				__forceinline auto ExclusiveLock(T& Funct) -> decltype(Funct())
				{
					extern void AcquireLockExclusiveSRWLock(void*);
					extern void ReleaseLockExclusiveSRWLock(void*);

					if constexpr (StaticAsserts::IsSameT<decltype(Funct()), void>())
					{
						AcquireLockExclusiveSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							throw;
						}
						ReleaseLockExclusiveSRWLock(&lock);
					}
					else
					{
						AcquireLockExclusiveSRWLock(&lock);
						try {
							auto ret(Funct());
							ReleaseLockExclusiveSRWLock(&lock);
							return ret;
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							throw;
						}
					}
				}
				///<summary>Виконує <see cref="Funct"/> з cпільни блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в спільному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T>
				__forceinline auto ShareLock(T& Funct) -> decltype(Funct())
				{
					extern void AcquireLockSharedSRWLock(void*);
					extern void ReleaseLockSharedSRWLock(void*);

					if constexpr (StaticAsserts::IsSameT<decltype(Funct()), void>())
					{
						AcquireLockSharedSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							ReleaseLockSharedSRWLock(&lock);
							throw;
						}
						ReleaseLockSharedSRWLock(&lock);
					}
					else
					{
						AcquireLockSharedSRWLock(&lock);
						try {
							auto ret(Funct());
							ReleaseLockSharedSRWLock(&lock);
							return ret;
						}
						catch (...) {
							ReleaseLockSharedSRWLock(&lock);
							throw;
						}
					}
				}
			};
		}
	}
}