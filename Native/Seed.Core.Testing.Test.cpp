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

			TestPackResult TestRegister::RunAllTest() {
				Containers::List<TestResult> results;
				toCoreLog(L"Запуск тестів...");
				DateTime startTime = DateTime::GetCurrentSystemTimeData();

				GetAllTests().ForEach([&](TestRecord& record) InlineLambda {
					DateTime testDurration = DateTime::GetCurrentSystemTimeData();
					try
					{
						if (record.Value().Excecute())
						{
							toCoreLog(L"Тест \"" + record.Key.TypeName + L"\" успішно пройдено");
							testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
							results += {record.Key.TypeName, TestResult::EStatus::Success, "", testDurration};
						}
						else
						{
							toCoreLog(L"Тест \"" + record.Key.TypeName + L"\" не пройдено");
							testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
							results += {record.Key.TypeName, TestResult::EStatus::Failure, L"Вернув false", testDurration};
						}
					}
					catch (Asserts::AssertException & ex)
					{
						toCoreLog(L"Спрацював ассерт в тесті \"" + record.Key.TypeName + L"\": " + ex.AssertMessage);
						testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
						results += {record.Key.TypeName, TestResult::EStatus::Failure, ex.toString(), testDurration};
					}
					catch (Exception & ex) {
						toCoreLog(L"При обробці тесту " + record.Key.TypeName + L" виникла помилка ", ex);
						testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
						results += {record.Key.TypeName, TestResult::EStatus::Exception, ex.toString(), testDurration};
					}
					catch (...) {
						toCoreLog(L"При обробці тесту " + record.Key.TypeName + L" виникла непередбачувана помилка");
						testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
						results += {record.Key.TypeName, TestResult::EStatus::Exception, L"Uknown", testDurration};
					}
					});

				DateTime endTime = DateTime::GetCurrentSystemTimeData();
				return TestPackResult(String(L"Тести ядра"), results, startTime, endTime);
			}
		}
	}
}