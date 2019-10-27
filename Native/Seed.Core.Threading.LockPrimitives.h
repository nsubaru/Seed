#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>Примітив синхронізації який дозволяє одночасне читання і есклюзивний запис</summary>
			class SeedKernelCall SRWLock final
			{
			private:
				struct ImplT {
					void* _;
				};

				ImplT lock;
			private:
				friend void AcquireLockExclusiveSRWLock(void* PSRWLock);
				friend void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				friend void AcquireLockSharedSRWLock(void* PSRWLock);
				friend void ReleaseLockSharedSRWLock(void* PSRWLock);
				friend bool TryAcquireLockSharedSRWLock(void* PSRWLock);
			private:
				friend _declspec(noreturn) void __rethrow();
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
				__forceinline decltype(auto) ExclusiveLock(T& Funct)
				{
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						AcquireLockExclusiveSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
						ReleaseLockExclusiveSRWLock(&lock);
					}
					else
					{
						AcquireLockExclusiveSRWLock(&lock);
						try {
							decltype(Funct()) ret(Funct());
							ReleaseLockExclusiveSRWLock(&lock);
							return ret;
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
				}
				///<summary>Виконує <see cref="Funct"/> з cпільни блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в спільному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T>
				__forceinline decltype(auto) ShareLock(T& Funct)
				{
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						AcquireLockSharedSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							ReleaseLockSharedSRWLock(&lock);
							__rethrow();
						}
						ReleaseLockSharedSRWLock(&lock);
					}
					else
					{
						AcquireLockSharedSRWLock(&lock);
						try {
							decltype(Funct()) ret(Funct());
							ReleaseLockSharedSRWLock(&lock);
							return ret;
						}
						catch (...) {
							ReleaseLockSharedSRWLock(&lock);
							__rethrow();
						}
					}
				}

				template<typename T>
				Boolean TryInShareLock(T& Funct) {
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						if (!TryAcquireLockSharedSRWLock(&lock))
							return false;
						try {
							Funct();
						}
						catch (...) {
							ReleaseLockSharedSRWLock(&lock);
							__rethrow();
						}
						ReleaseLockSharedSRWLock(&lock);
						return true;
					}
					else
					{
						static_assert(false, "Try acquire can use only with void return function\\lambda");
					}
				}
			};
		}
	}
}