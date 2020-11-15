#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Memory {
#pragma region Miscenelious memory function
			///<summary>Копіювання ділянки пам'яті</summary>
			///<param name='Source'>Ділянка пам'яті яка копіюється</param>
			///<param name='Destination'>Ділянка пам'яті куда копіюється</param>
			///<param name='Size'>Кількість байт як потрібно скопіювати</param>
			SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void Copy(
				const void* Source,
				void* Destination,
				SizeT Size);

			///<summary>Заповнює блок пам'яті певним значенням</summary>
			///<param name='MemBlk'>Блок пам'яті</param>
			///<param name='Size'>Розмір в байтах ділянки пам'яті</param>
			///<param name='Filler'>Значення яким заповнить ділянку пам'яті</param>
			SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void MemBlkFill(
				void* MemBlk,
				SizeT Size,
				const Byte Filler);
#pragma endregion
		}
	}
}