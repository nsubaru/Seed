#pragma once
#include "Seed.Core.Logger.StaticLogger.h"

#include "Seed.Core.String.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Logger {
			/// <summary>
			/// Розширений статичний логгер
			/// </summary>
			template<EStaticSubsystems TSubsystem>
			class StaticManagedLogger : public StaticLogger<TSubsystem> {

			public:
				/// <summary>
				/// Записати повідомлення в логи
				/// </summary>
				/// <param name="logLevel">Тип повідомлення</param>
				/// <param name="msg">Повідомлення</param>
				template<typename THeap>
				static void toLog(ELoggerLevel logLevel, const BaseString<THeap>& msg) {
					if (CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[TSubsystem] & logLevel) {
						switch (logLevel)
						{
						case Seed::Core::Logger::ELoggerLevel::Debug:
							toLogWithLevel<ELoggerLevel::Debug>(msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Verbose:
							toLogWithLevel<ELoggerLevel::Verbose>(msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Warning:
							toLogWithLevel<ELoggerLevel::Warning>(msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Error:
							toLogWithLevel<ELoggerLevel::Error>(msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Critical:
							toLogWithLevel<ELoggerLevel::Critical>(msg);
							break;
						default:
							break;
						}
					}
				}

			protected:
				template<ELoggerLevel TLogLevel, typename THeap>
				static void toLogWithLevel(const BaseString<THeap>& msg) {
					constexpr decltype(auto) header = GenerateLogTextHeader<TLogLevel>();

					UChar buffer[MaxLogMessage];
					CStr::Copy((UChar*)header.Raw(), buffer);
					
					if (Msg.Lenght > MaxLogMessage - (header.Count * sizeof(UChar))) {
						auto str = msg.Clone().Resize(((MaxLogMessage - 1) - (header.Count * sizeof(UChar))) - 3);
						str += "...";

						CStr::Copy((UChar*)(const ConstCharStr)str, buffer + (header.Count * sizeof(UChar)));
					}
					else {
						CStr::Copy((UChar*)(const ConstCharStr)msg, buffer + (header.Count * sizeof(UChar)));
					}

					toCoreLog(buffer, (header.Count * sizeof(UChar)) + msg.Size.Bytes);
				}
			};
		}
	}
}

#pragma warning(pop)