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
				volatile EThreadabledStates SecondState;
				volatile EThreadabledStates* State;
				TlsExceptionData ExceptionInfo;
			} TLSData = { 0 };

			InternalApiCall ForceInline TLS* GetTLS() { return &TLSData; }

			InternalApiCall ForceInline void ConfigureTls(IThreadable* Threadable, volatile EThreadabledStates* State) {
				TLS* thisTLS = GetTLS();
				Memory::MemBlkFill(thisTLS, sizeof(TLS), 0);
				thisTLS->State = State;
				thisTLS->Threadable = Threadable;
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