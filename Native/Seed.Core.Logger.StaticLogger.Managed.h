#pragma once
#include "Seed.Core.Logger.StaticLogger.h"

#include "Seed.Core.String.h"
#include "Seed.Core.CoreLog.Managed.h"

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

				/// <summary>
				/// Записати повідомлення в логи
				/// </summary>
				/// <param name="logLevel">Тип повідомлення</param>
				/// <param name="msg">Повідомлення</param>
				template<typename THeap>
				static void toLog(ELoggerLevel logLevel, BaseString<THeap>&& msg) {
					if (CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[TSubsystem] & logLevel)
					{
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

				/// <summary>
				/// Записати повідомлення в логи
				/// </summary>
				/// <param name="logLevel">Тип повідомлення</param>
				/// <param name="msg">Повідомлення</param>
				template<typename TLambda> requires Concepts::IsInvockable<TLambda>
				static void toLog(ELoggerLevel logLevel, TLambda&& msgGen) {
					if (CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[TSubsystem] & logLevel)
					{
						switch (logLevel)
						{
							case Seed::Core::Logger::ELoggerLevel::Debug:
							toLogWithLevel<ELoggerLevel::Debug>(msgGen());
							break;
							case Seed::Core::Logger::ELoggerLevel::Verbose:
							toLogWithLevel<ELoggerLevel::Verbose>(msgGen());
							break;
							case Seed::Core::Logger::ELoggerLevel::Warning:
							toLogWithLevel<ELoggerLevel::Warning>(msgGen());
							break;
							case Seed::Core::Logger::ELoggerLevel::Error:
							toLogWithLevel<ELoggerLevel::Error>(msgGen());
							break;
							case Seed::Core::Logger::ELoggerLevel::Critical:
							toLogWithLevel<ELoggerLevel::Critical>(msgGen());
							break;
							default:
							break;
						}
					}
				}

			protected:
				template<ELoggerLevel TLogLevel>
				constexpr static auto GenerateManagedLogTextHeader() {
					return
						Details::CreateTypeStr(L"[") +
						Logger::GetLogLevelName<TLogLevel>() +
						Details::CreateTypeStr(L" -> ") +
						Logger::GetStaticSubsystemName<TSubsystem>() +
						Details::CreateTypeStr(L"]  \0");
				}

			protected:
				template<ELoggerLevel TLogLevel, typename THeap>
				static void toLogWithLevel(const BaseString<THeap>& msg) {
					constexpr decltype(auto) header = GenerateManagedLogTextHeader<TLogLevel>();
					constexpr auto headerSize = header.Count - 1;

					UChar buffer[StaticLogger<TSubsystem>::MaxLogMessage]{};
					CStr::Copy((UChar*)header.Raw(), buffer);
					
					if (msg.FullLenght > StaticLogger<TSubsystem>::MaxLogMessage - headerSize) {
						auto str = msg.Clone().Resize(((StaticLogger<TSubsystem>::MaxLogMessage - 1) - headerSize) - 3);
						str += "...";

						CStr::Copy((UChar*)(const ConstCharStr)str, buffer + headerSize);
					}
					else {
						CStr::Copy((UChar*)(const ConstCharStr)msg, buffer + headerSize);
					}

					toCoreLog(buffer, (headerSize * sizeof(UChar)) + msg.Size.Bytes);
				}

				template<ELoggerLevel TLogLevel, typename THeap>
				static void toLogWithLevel(BaseString<THeap>&& msg) {
					constexpr decltype(auto) header = GenerateManagedLogTextHeader<TLogLevel>();
					constexpr auto headerSize = header.Count - 1;

					UChar buffer[StaticLogger<TSubsystem>::MaxLogMessage]{};
					CStr::Copy((UChar*) header.Raw(), buffer);

					if (msg.FullLenght > StaticLogger<TSubsystem>::MaxLogMessage - headerSize)
					{
						auto str = msg.Clone().Resize(((StaticLogger<TSubsystem>::MaxLogMessage - 1) - headerSize) - 3);
						str += "...";

						CStr::Copy((UChar*) (const ConstCharStr) str, buffer + headerSize);
					}
					else
					{
						CStr::Copy((UChar*) (const ConstCharStr) msg, buffer + headerSize);
					}

					toCoreLog(buffer, (headerSize * sizeof(UChar)) + msg.Size.Bytes);
				}
			};
		}
	}
}

#pragma warning(pop)