#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>������� ������������ ���� �������� ��������� ������� � ����������� �����</summary>
			class SeedKernelCall SRWLock final
			{
			private:
				struct ImplT {
					void* _;
				};

				ImplT lock;
			public:
				ForceInline SRWLock()
				{
					Internals::InitSRWLock(&lock);
				}
				///<summary>������ <see cref="Funct"/> � ���������� �����������</summary>
				///<param name="Funct">������� ��� ������ ��� ���������� � ������������� ���������</param>
				///<returns>�� � �� � Funct</returns>
				template<typename T> requires Concepts::IsInvockable<T>
				ForceInline decltype(auto) ExclusiveLock(T&& Funct)
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
							if constexpr (TypeTraits::IsLValueReferenceV<decltype(Funct())>)
							{
								decltype(Funct()) ret(Funct());
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								return ret;
							}
							else
							{
								decltype(Funct()) ret(MoveRValue(Funct()));
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								return ret;
							}
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}
				///<summary>������ <see cref="Funct"/> � c����� �����������</summary>
				///<param name="Funct">������� ��� ������ ��� ���������� � �������� ���������</param>
				///<returns>�� � �� � Funct</returns>
				template<typename T> requires Concepts::IsInvockable<T>
				ForceInline auto ShareLock(T&& Funct) -> decltype(Funct())
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
							if constexpr (TypeTraits::IsLValueReferenceV<decltype(Funct())>)
							{
								decltype(Funct()) ret(Funct());
								Internals::ReleaseLockSharedSRWLock(&lock);
								return ret;
							}
							else
							{
								decltype(Funct()) ret(MoveRValue(Funct()));
								Internals::ReleaseLockSharedSRWLock(&lock);
								return ret;
							}
						}
						catch (...) {
							Internals::ReleaseLockSharedSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}

				template<typename T> requires Concepts::IsInvockableWithResult<T, void>
				ForceInline Boolean TryInShareLock(T&& Funct) {
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