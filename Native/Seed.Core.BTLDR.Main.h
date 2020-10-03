#pragma once
#include "Seed.Core.BTLDR.Public.h"

Seed::Core::Btlrd::BtlrdInPlaceUpdateData* Main(void*, void(*)(void*), bool isFirstboot);

namespace Seed {
	inline namespace Core {
		namespace Btlrd {
			__declspec(noreturn, noinline) void BtldEntryPoint(Handle ntdll, void* LdrGetProcedureAddress);
		}
	}
}