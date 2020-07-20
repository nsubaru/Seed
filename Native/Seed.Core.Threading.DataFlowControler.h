#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class DataFlowControler {
			private:
				struct ImplT {
					void* _;
				};
			private:
				ImplT lock;
				ImplT producer;
				ImplT concumer;
				volatile bool isConcumerWork;
				volatile bool isProducerWork;
			public:
				DataFlowControler();
				
				template<typename Lambda>
				forceinline decltype(auto) Concume(Lambda && lambda) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);
							isProducerWork = true;

							lambda();

							isProducerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::WakeOneCondVal(&concumer);
						}
						catch (...) {
							isProducerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
					else {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);
							isProducerWork = true;

							if constexpr (TypeTraits::IsLValueReferenceV<decltype(lambda())>)
							{
								decltype(lambda()) ret(lambda());

								isProducerWork = false;
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								Internals::WakeOneCondVal(&concumer);

								return ret;
							}
							else
							{
								decltype(lambda()) ret(MoveRValue(lambda()));

								isProducerWork = false;
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								Internals::WakeOneCondVal(&concumer);

								return ret;
							}
						}
						catch (...) {
							isProducerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}

				template<typename Lambda>
				forceinline decltype(auto) Produce(Lambda && lambda) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);
							isConcumerWork = true;

							lambda();

							isConcumerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::WakeOneCondVal(&producer);
						}
						catch (...) {
							isConcumerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
					else {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);
							isConcumerWork = true;

							if constexpr (TypeTraits::IsLValueReferenceV<decltype(lambda())>)
							{
								decltype(lambda()) ret(lambda());

								isConcumerWork = false;
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								Internals::WakeOneCondVal(&producer);

								return ret;
							}
							else
							{
								decltype(lambda()) ret(MoveRValue(lambda()));

								isConcumerWork = false;
								Internals::ReleaseLockExclusiveSRWLock(&lock);
								Internals::WakeOneCondVal(&producer);

								return ret;
							}
						}
						catch (...) {
							isConcumerWork = false;
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}

				template<typename Lambda>
				forceinline auto SharedLock(Lambda&& lambda) -> decltype(lambda()) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							Internals::AcquireLockSharedSRWLock(&lock);

							lambda();

							Internals::ReleaseLockSharedSRWLock(&lock);
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
					else {
						try
						{
							Internals::AcquireLockSharedSRWLock(&lock);

							if constexpr (TypeTraits::IsLValueReferenceV<decltype(lambda())>)
							{
								decltype(lambda()) ret(lambda());

								Internals::ReleaseLockSharedSRWLock(&lock);

								return ret;
							}
							else
							{
								decltype(lambda()) ret(MoveRValue(lambda()));

								Internals::ReleaseLockSharedSRWLock(&lock);

								return ret;
							}
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
				}

				template<typename Lambda>
				forceinline auto ExclusiveLock(Lambda&& lambda) -> decltype(lambda()) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);

							lambda();

							Internals::ReleaseLockExclusiveSRWLock(&lock);
						}
						catch (...) {
							Internals::ReleaseLockExclusiveSRWLock(&lock);
							Internals::__CxxRethrowImpl();
						}
					}
					else {
						try {
							Internals::AcquireLockExclusiveSRWLock(&lock);

							if constexpr (TypeTraits::IsLValueReferenceV<decltype(lambda())>)
							{
								decltype(lambda()) ret(lambda());

								Internals::ReleaseLockExclusiveSRWLock(&lock);

								return ret;
							}
							else
							{
								decltype(lambda()) ret(MoveRValue(lambda()));

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

				forceinline void WaitForProducer() {
					if (true)
						Internals::WaitCondVal(&producer, &lock);
					else
						Core::Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCTION__, __LINE__);
				}

				forceinline void WaitForConcumer() {
					if (true)
						Internals::WaitCondVal(&concumer, &lock);
					else
						Core::Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCTION__, __LINE__);
				}
			};
		}
	}
}

#pragma warning(pop)