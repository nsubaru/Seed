#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.BootupServicesManager.h"

#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			/// <summary>
			/// Сервіс логера ядра, надає можливість запису в лог і перегляду записів поточного запуску
			/// </summary>
			class CoreLogger final: IBootUpService<CoreLogger> {
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto CoreLoggerServiceDefinition = ObjectDefinition(
						Services::CoreLogger,
						NullCtorDescription(Services::CoreLogger),
						ObjectMethodsDefs(
							MethodDescription(Services::CoreLogger, ReadLog),
							MethodDescription(Services::CoreLogger, ToLog)
						),
						ObjectPropsDefs()
					);

					return GenObjectRTTIRecord(CoreLoggerServiceDefinition);
				}
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