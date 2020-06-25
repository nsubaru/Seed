#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		SeedKernelCall InternalApiCall Boolean toCoreLog(const Char* Value, SizeT Lenght);

		/// <summary>
		/// Запис в логи статичного повідомлення
		/// </summary>
		template<SizeT Lenght>
		forceinline Boolean toCoreLog(StaticTextRef<Lenght> Msg) {
#pragma warning(push)
#pragma warning(disable: 4996)
			return toCoreLog(Msg, Lenght);
#pragma warning(pop)
		}
	}
}