// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Test.h"
#include "Seed.Core.Testing.Register.h"

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

			TestPackResult TestRegister::RunAllTest(Boolean isLogging) {
				Containers::List<TestResult> results;

				if(isLogging) toCoreLog(L"Запуск тестів...");

				Time::DateTime startTime = Time::DateTime::GetCurrentSystemTimeData();

				auto maxTestNameSz = GetAllTests().Max(ArrowFunct(record, record.Key.TypeName.Lenght));

				maxTestNameSz += 10;

				String msg = (isLogging) ? String(maxTestNameSz + 100Kb) : String();

				GetAllTests().ForEach([&](TestRecord& record) InlineLambda {
					Time::TimeSpan testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks;
					
					try
					{
						if (record.Value().Excecute())
						{
							testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

							if (isLogging) {
								msg += L"Тест \"";
								msg += record.Key.TypeName;
								msg += L"\"";
								msg.ResizeToFixed(maxTestNameSz);
								msg += L" тривалість: ";
								msg += String::toString(testDurration.Ticks);
								msg += L" tiks";
								msg += L", тест пройдено";
								toCoreLog(msg);
							}

							results += {record.Key.TypeName, TestResult::EStatus::Success, "", testDurration};
						}
						else
						{
							testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

							if (isLogging) {
								msg += L"Тест \"";
								msg += record.Key.TypeName;
								msg += L"\"";
								msg.ResizeToFixed(maxTestNameSz);
								msg += L" тривалість: ";
								msg += String::toString(testDurration.Ticks);
								msg += L" tiks";
								msg += L", тест скасовано";
								toCoreLog(msg);
							}

							results += {record.Key.TypeName, TestResult::EStatus::Failure, L"Вернув false", testDurration};
						}

						if (isLogging) msg.Resize(0);
					}
					catch (Asserts::AssertException & ex)
					{
						testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

						if (isLogging) {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, спрацював ассерт: ";
							msg += ex.AssertMessage;

							toCoreLog(msg);
							msg.Resize(0);
						}

						results += {record.Key.TypeName, TestResult::EStatus::Failure, ex.toString(), testDurration};
					}
					catch (Exception & ex) {
						testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

						if (isLogging) {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло виключення: ";
							msg += ex.toString();

							toCoreLog(msg);
							msg.Resize(0);
						}

						results += {record.Key.TypeName, TestResult::EStatus::Exception, ex.toString(), testDurration};
					}
					catch (...) {
						testDurration = Time::DateTime::GetCurrentSystemTimeData().Ticks - testDurration.Ticks;

						if (isLogging) {
							msg += L"Тест \"";
							msg += record.Key.TypeName;
							msg += L"\"";
							msg.ResizeToFixed(maxTestNameSz);
							msg += L" тривалість: ";
							msg += String::toString(testDurration);
							msg += L" tiks";
							msg += L", тест не пройдено, виникло невідоме виключення";

							toCoreLog(msg);
							msg.Resize(0);
						}
						results += {record.Key.TypeName, TestResult::EStatus::Exception, L"Uknown", testDurration};

						
					}
					});

				auto endTime = Time::DateTime::GetCurrentSystemTimeData();
				return TestPackResult(String(L"Тести ядра"), results, { startTime, endTime });
			}
		}
	}
}