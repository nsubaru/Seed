#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Exceptions.h"

#include "Seed.Core.Memory.h"

#include "Seed.Core.Exceptions.ABI.Internal.Tls.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			struct TLS {
				IThreadable* Threadable;
				volatile EThreadabledStates* State;
				//ExceptionInfoT ExceptionInfo;

				TlsExceptionData ExceptionInfo;
			};

			static_assert(sizeof(TEB::TlsSlots) >= sizeof(TLS), "Seed TLS to big");

			InternalApiCall __forceinline TLS* GetTLS() { return reinterpret_cast<TLS*>(NtCurrentTeb()->TlsSlots); }

			InternalApiCall __forceinline void ConfigureTls(IThreadable* Threadable, volatile EThreadabledStates* State) {
				TLS* thisTLS = GetTLS();
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
				thisTLS->State = State;
				thisTLS->Threadable = Threadable;
				thisTLS->ExceptionInfo = TlsExceptionData();
			}

			InternalApiCall __forceinline void DestroyTls() {
				TLS* thisTLS = GetTLS();
				delete thisTLS->Threadable;
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
			}

			InternalApiCall __forceinline void InitMinTls() {
				TLS* thisTLS = GetTLS();
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
				thisTLS->ExceptionInfo = TlsExceptionData();
			}

			InternalApiCall __forceinline void StopMinTls() {
				TLS* thisTLS = GetTLS();
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
			}
		}
	}
}

#pragma warning(pop)