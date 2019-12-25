#pragma once
#include "Seed.Core.Services.Base.h"

#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			/// <summary>
			/// Сервіс логера ядра, надає можливість запису в лог і проглядання записів
			/// </summary>
			class SeedKernelCall CoreLogger {
			public:
				CoreLogger() {}

				/// <summary>
				/// Запис в лог ядра повідомлення
				/// </summary>
				/// <param name="msg"></param>
				/// <returns></returns>
				bool ToLog(const String& msg) {
					return toCoreLog(msg);
				}

				/// <summary>
				/// Зчитування всіх повідомленнь з логера
				/// </summary>
				/// <returns></returns>
				Containers::List<String> ReadLog();
			};
		}
	}
}