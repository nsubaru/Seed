#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

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
			public:
				forceinline SRWLock()
				{
					Internals::InitSRWLock(&lock);
				}
				///<summary>Виконує <see cref="Funct"/> з еклюзивним блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в ексклюзивному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T> requires Concepts::IsInvockable<T>
				forceinline decltype(auto) ExclusiveLock(T& Funct)
				{
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						Internals::AcquireLockExclusiveSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
						Internals::ReleaseLockExclusiveSRWLock(&lock);
					}
					else
					{
						Internals::AcquireLockExclusiveSRWLock(&lock);
						try {
							decltype(Funct()) ret(Funct());
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							return ret;
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}
				///<summary>Виконує <see cref="Funct"/> з cпільни блокуванням</summary>
				///<param name="Funct">Функція або лямбда яка виконається в спільному блокуванні</param>
				///<returns>Те ж що і Funct</returns>
				template<typename T> requires Concepts::IsInvockable<T>
				forceinline decltype(auto) ShareLock(T&& Funct)
				{
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						Internals::AcquireLockSharedSRWLock(&lock);
						try {
							Funct();
						}
						catch (...) {
							Internals::ReleaseLockSharedSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
						Internals::ReleaseLockSharedSRWLock(&lock);
					}
					else
					{
						Internals::AcquireLockSharedSRWLock(&lock);
						try {
							decltype(Funct()) ret(Funct());
							Internals::ReleaseLockSharedSRWLock(&lock);
							return ret;
						}
						catch (...) {
							Internals::ReleaseLockSharedSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}

				template<typename T> requires Concepts::IsInvockableWithResult<T, void>
				forceinline Boolean TryInShareLock(T&& Funct) {
					if constexpr (TypeTraits::IsSameT<decltype(Funct()), void>())
					{
						if (!Internals::TryAcquireLockSharedSRWLock(&lock))
							return false;
						try {
							Funct();
						}
						catch (...) {
							Internals::ReleaseLockSharedSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
						Internals::ReleaseLockSharedSRWLock(&lock);
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

#pragma warning(pop)