// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Test.h"
#include "Seed.Core.Testing.Register.h"
#include "Seed.Core.Logger.StaticLogger.Managed.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Containers;
using namespace Seed::Core::Testing;

inline auto& TestsStorage() 
{
	using namespace Seed;
	using namespace Seed::Core::Containers;
	static auto tests = List<Seed::Core::Testing::TestRecord>();

	return tests;
}

namespace Seed
{
	inline namespace Core
	{
		namespace Testing
		{
			SizeT LoadTest(RTTI::RTTITypeRecord&& typeRecord, ITest&(*testObjGenerator)())
			{
				auto& tests = TestsStorage();
				auto testAddress = tests.Count;
				tests += {MoveRValue(typeRecord), testObjGenerator};
				return testAddress;
			}

			inline List<TestRecord>& TestRegister::GetAllTests()
			{
				return TestsStorage();
			}

			Tuple<String, SizeT> InitMsgBufferIfRequiredForLogging(Boolean isManual, const List<TestRecord>& records) {
				if (isManual && (
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::ManualTestingAPI] & Logger::ELoggerLevel::Verbose ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::ManualTestingAPI] & Logger::ELoggerLevel::Error ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::ManualTestingAPI] & Logger::ELoggerLevel::Critical ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::ManualTestingAPI] & Logger::ELoggerLevel::Warning))
				{
					SizeT maxTestNameSz = records.Max(ArrowFunct(record, record.Key.TypeName.Lenght));

					maxTestNameSz += 10;

					String msg = String(Memory::DataSizeT(maxTestNameSz) + 100Kb);

					return { msg, maxTestNameSz };
				}
				else if(
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::AutomaticTestingAPI] & Logger::ELoggerLevel::Verbose ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::AutomaticTestingAPI] & Logger::ELoggerLevel::Error ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::AutomaticTestingAPI] & Logger::ELoggerLevel::Critical ||
					CurrentSettings().KernelLoggerConfiguration.StaticLoggerLevel[Logger::EStaticSubsystems::AutomaticTestingAPI] & Logger::ELoggerLevel::Warning)
				{
					SizeT maxTestNameSz = records.Max(ArrowFunct(record, record.Key.TypeName.Lenght));

					maxTestNameSz += 10;

					String msg = String(Memory::DataSizeT(maxTestNameSz) + 100Kb);

					return { msg, maxTestNameSz };
				}

				return { String(), SizeT() };
			}

			void LoggingSuccessTestResult(
				const TestRecord& record, 
				Time::TimeSpan testDurration, 
				Boolean isManual, 
				String& msg, 
				SizeT maxTestNameSz
			) {
				if (isManual)
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Warning,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration.Ticks);
							msg += L" tiks";
							msg += L", тест успішно пройдено";
							return msg;
						});

					msg.Resize(0);
				}
				else
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Warning,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration.Ticks);
							msg += L" tiks";
							msg += L", тест успішно пройдено";
							return msg;
						});

					msg.Resize(0);
				}
			}

			void LoggingFailedTestResult(
				const TestRecord& record,
				Time::TimeSpan testDurration,
				Boolean isManual,
				String& msg,
				SizeT maxTestNameSz
			) {
				if (isManual)
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Verbose,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration.Ticks);
							msg += L" tiks";
							msg += L", тест пройдено";
							return msg;
						});

					msg.Resize(0);
				}
				else
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Verbose,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration.Ticks);
							msg += L" tiks";
							msg += L", тест пройдено";
							return msg;
						});

					msg.Resize(0);
				}
			}

			void LoggingTestAssertResult(
				const TestRecord& record,
				Time::TimeSpan testDurration,
				Boolean isManual,
				String& msg,
				SizeT maxTestNameSz,
				Asserts::AssertException& ex
			) {
				if (isManual)
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Warning,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, спрацював ассерт: ";
							msg += ex.AssertMessage;
							return msg;
						});

					msg.Resize(0);
				}
				else
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Warning,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, спрацював ассерт: ";
							msg += ex.AssertMessage;
							return msg;
						});

					msg.Resize(0);
				}
			}

			void LoggingTestExceptionResult(
				const TestRecord& record,
				Time::TimeSpan testDurration,
				Boolean isManual,
				String& msg,
				SizeT maxTestNameSz,
				Exception& ex
			) {
				if (isManual)
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Error,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло виключення: ";
							msg += ex.toString();
							return msg;
						});

					msg.Resize(0);
				}
				else
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Error,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло виключення: ";
							msg += ex.toString();
							return msg;
						});

					msg.Resize(0);
				}
			}

			void LoggingTestUknownExceptionResult(
				const TestRecord& record,
				Time::TimeSpan testDurration,
				Boolean isManual,
				String& msg,
				SizeT maxTestNameSz
			) {
				if (isManual)
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Critical,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло невідоме виключення";
							return msg;
						});

					msg.Resize(0);
				}
				else
				{
					Logger::StaticManagedLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Critical,
						[&] () {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло невідоме виключення";
							return msg;
						});

					msg.Resize(0);
				}
			}

			TestPackResult TestRegister::RunAllTest(Boolean isManual) {
				Containers::List<TestResult> results;

				if (isManual)
				{
					Logger::StaticLogger<Logger::EStaticSubsystems::ManualTestingAPI>::toLog(
						Logger::ELoggerLevel::Verbose,
						L"Запуск тестів...");
				}
				else
				{
					Logger::StaticLogger<Logger::EStaticSubsystems::AutomaticTestingAPI>::toLog(
						Logger::ELoggerLevel::Verbose,
						L"Запуск тестів...");
				}

				Time::DateTime startTime = Time::DateTime::GetCurrentSystemTimeData();

				auto [msg, maxTestNameSz] = InitMsgBufferIfRequiredForLogging(isManual, GetAllTests());

				GetAllTests().ForEach([&](TestRecord& record) InlineLambda {
					Time::TimeSpan testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks;
					
					try
					{
						auto result = record.Value().Excecute();
						testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

						if (result)
						{
							LoggingSuccessTestResult(record, testDurration, isManual, msg, maxTestNameSz);
							results += {record.Key.TypeName, TestResult::EStatus::Success, "", testDurration};
						}
						else
						{
							LoggingFailedTestResult(record, testDurration, isManual, msg, maxTestNameSz);
							results += {record.Key.TypeName, TestResult::EStatus::Failure, L"Вернув false", testDurration};
						}
					}
					catch (Asserts::AssertException & ex)
					{
						LoggingTestAssertResult(record, testDurration, isManual, msg, maxTestNameSz, ex);
						results += {record.Key.TypeName, TestResult::EStatus::Failure, ex.toString(), testDurration};
					}
					catch (Exception & ex) {
						LoggingTestExceptionResult(record, testDurration, isManual, msg, maxTestNameSz, ex);
						results += {record.Key.TypeName, TestResult::EStatus::Exception, ex.toString(), testDurration};
					}
					catch (...) {
						LoggingTestUknownExceptionResult(record, testDurration, isManual, msg, maxTestNameSz);
						results += {record.Key.TypeName, TestResult::EStatus::Exception, L"Uknown", testDurration};
					}
					});

				auto endTime = Time::DateTime::GetCurrentSystemTimeData();
				return TestPackResult(String(L"Тести ядра"), results, { startTime, endTime });
			}
		}
	}
}