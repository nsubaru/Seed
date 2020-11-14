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

			inline __declspec(thread) struct TLS {
				IThreadable* Threadable = 0;
				volatile EThreadabledStates* volatile SecondState;
				volatile EThreadabledStates* volatile State;
				TlsExceptionData ExceptionInfo;
			} TLSData = { 0 };

			InternalApiCall ForceInline TLS* GetTLS() { return &TLSData; }

			InternalApiCall ForceInline void ConfigureTls(
				IThreadable* threadable, 
				volatile EThreadabledStates* volatile state,
				volatile EThreadabledStates* volatile secondState
			) {
				TLS* thisTLS = GetTLS();
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
				thisTLS->State = state;
				thisTLS->SecondState = secondState;
				thisTLS->Threadable = threadable;
				thisTLS->ExceptionInfo = TlsExceptionData();
			}

			InternalApiCall ForceInline void DestroyTls() {
				TLS* thisTLS = GetTLS();
				delete thisTLS->Threadable;
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
			}
		}
	}
}

#pragma warning(pop)