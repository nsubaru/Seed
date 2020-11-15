#pragma once
#include "Seed.Core.Logger.Base.h"

#include "Seed.Core.SettingsAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Logger {
			/// <summary>
			/// Статичний логгер
			/// </summary>
			template<EStaticSubsystems TSubsystem>
			class StaticLogger {

			public:
				static constexpr SizeT MaxLogMessage = CurrentSettings::SettingsData::KernelLoggerT::MaxKernelLogMsgSize.Bytes;

			public:
				/// <summary>
				/// Записати повідомлення в логи
				/// </summary>
				/// <param name="logLevel">Тип повідомлення</param>
				/// <param name="Msg">Повідомлення</param>
				template<SizeT Lenght>
				static void toLog(ELoggerLevel logLevel, StaticTextRef<Lenght> Msg) {
					if (CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[TSubsystem] & logLevel) {
						switch (logLevel)
						{
						case Seed::Core::Logger::ELoggerLevel::Debug:
							toLogWithLevel<ELoggerLevel::Debug>(Msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Verbose:
							toLogWithLevel<ELoggerLevel::Verbose>(Msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Warning:
							toLogWithLevel<ELoggerLevel::Warning>(Msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Error:
							toLogWithLevel<ELoggerLevel::Error>(Msg);
							break;
						case Seed::Core::Logger::ELoggerLevel::Critical:
							toLogWithLevel<ELoggerLevel::Critical>(Msg);
							break;
						default:
							break;
						}
					}
				}

			protected:
				template<ELoggerLevel TLogLevel>
				constexpr static auto GenerateLogTextHeader() {
					return
						Details::CreateTypeStr(L"[") +
						Logger::GetLogLevelName<TLogLevel>() +
						Details::CreateTypeStr(L" -> ") +
						Logger::GetStaticSubsystemName<TSubsystem>() +
						Details::CreateTypeStr(L"] ");
				}

			protected:
				template<ELoggerLevel TLogLevel, SizeT Lenght>
				static void toLogWithLevel(StaticTextRef<Lenght> Msg) {
					constexpr decltype(auto) header = GenerateLogTextHeader<TLogLevel>();

					UChar buffer[MaxLogMessage];

					CStr::Copy((UChar*)header.Raw(), buffer);

					if constexpr (Lenght * sizeof(UChar) >= 4095 - (header.Count * sizeof(UChar))) {
						Memory::Copy(
							(UChar*)Msg, 
							buffer + (header.Count * sizeof(UChar)), 
							(MaxLogMessage - 1) - ((header.Count * sizeof(UChar)) + 3));

						buffer[MaxLogMessage - 4] = L'.';
						buffer[MaxLogMessage - 3] = L'.';
						buffer[MaxLogMessage - 2] = L'.';
						buffer[MaxLogMessage - 1] = L'\0';
					}
					else {
						CStr::Copy((UChar*)Msg, buffer + (header.Count * sizeof(UChar)));
					}

					toCoreLog(buffer, (header.Count * sizeof(UChar) + Lenght));
				}
			};
		}
	}
}

#pragma warning(pop)