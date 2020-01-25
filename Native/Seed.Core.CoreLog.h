#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		SeedKernelCall InternalApiCall Boolean toCoreLog(const Char* Value, SizeT Lenght);

		template<SizeT Lenght>
		__forceinline Boolean toCoreLog(StaticTextRef<Lenght> Msg) {
#pragma warning(push)
#pragma warning(disable: 4996)
			return toCoreLog(Msg, Lenght);
#pragma warning(pop)
		}
	}
}