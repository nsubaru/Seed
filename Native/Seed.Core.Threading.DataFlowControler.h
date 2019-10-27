#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.ConstString.h"

#define CXXTHROWAPI

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
			private:
				friend void AcquireLockExclusiveSRWLock(void* PSRWLock);
				friend void ReleaseLockExclusiveSRWLock(void* PSRWLock);
				friend void AcquireLockSharedSRWLock(void* PSRWLock);
				friend void ReleaseLockSharedSRWLock(void* PSRWLock);
				friend void WakeOneCondVal(void* condVar);
				friend void WaitCondVal(void* condVar, void* srwLock);
			private:
				friend _declspec(noreturn) void __rethrow();
			public:
				DataFlowControler();
				
				template<typename Lambda>
				__forceinline decltype(auto) Concume(Lambda && lambda) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							AcquireLockExclusiveSRWLock(&lock);
							isProducerWork = true;

							lambda();

							isProducerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							WakeOneCondVal(&concumer);
						}
						catch (...) {
							isProducerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
					else {
						try {
							AcquireLockExclusiveSRWLock(&lock);
							isProducerWork = true;

							decltype(lambda()) ret(MoveRValue(lambda()));

							isProducerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							WakeOneCondVal(&concumer);

							return ret;
						}
						catch (...) {
							isProducerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
				}

				template<typename Lambda>
				__forceinline decltype(auto) Produce(Lambda && lambda) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							AcquireLockExclusiveSRWLock(&lock);
							isConcumerWork = true;

							lambda();

							isConcumerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							WakeOneCondVal(&producer);
						}
						catch (...) {
							isConcumerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
					else {
						try {
							AcquireLockExclusiveSRWLock(&lock);
							isConcumerWork = true;

							decltype(lambda()) ret(MoveRValue(lambda()));

							isConcumerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							WakeOneCondVal(&producer);

							return ret;
						}
						catch (...) {
							isConcumerWork = false;
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
				}

				template<typename Lambda>
				__forceinline auto SharedLock(Lambda&& lambda) -> decltype(lambda()) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							AcquireLockSharedSRWLock(&lock);

							lambda();

							ReleaseLockSharedSRWLock(&lock);
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
					else {
						try {
							AcquireLockSharedSRWLock(&lock);

							decltype(lambda()) ret(MoveRValue(lambda()));

							ReleaseLockSharedSRWLock(&lock);

							return ret;
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
				}

				template<typename Lambda>
				__forceinline auto ExclusiveLock(Lambda&& lambda) -> decltype(lambda()) {
					if constexpr (TypeTraits::IsSameV<decltype(lambda()), void>) {
						try {
							AcquireLockExclusiveSRWLock(&lock);

							lambda();

							ReleaseLockExclusiveSRWLock(&lock);

							return ret;
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
					else {
						try {
							AcquireLockExclusiveSRWLock(&lock);

							decltype(lambda()) ret(MoveRValue(lambda()));

							ReleaseLockExclusiveSRWLock(&lock);

							return ret;
						}
						catch (...) {
							ReleaseLockExclusiveSRWLock(&lock);
							__rethrow();
						}
					}
				}

				__forceinline void WaitForProducer() {
					if (true)
						WaitCondVal(&producer, &lock);
					else
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCTION__, __LINE__);
				}

				__forceinline void WaitForConcumer() {
					if (true)
						WaitCondVal(&concumer, &lock);
					else
						Internals::Exceptions::ThrowNullPointException(__FILE__, __FUNCTION__, __LINE__);
				}
			};
		}
	}
}

#undef CXXTHROWAPI