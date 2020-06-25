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

void Seed::Core::Testing::LoadTest(Seed::Core::String&& name, Seed::Core::Testing::ITest*&& instance)
{
	auto& tests = TestsStorage();
	tests += {MoveRValue(name), MoveRValue(instance)};
}

inline List<TestRecord> & Seed::Core::Testing::TestRegister::GetAllTests()
{
	return TestsStorage();
}

TestPackResult Seed::Core::Testing::TestRegister::RunAllTest() {
	Containers::List<TestResult> results;
	toCoreLog(L"Запуск тестів...");
	DateTime startTime = DateTime::GetCurrentSystemTimeData();

	GetAllTests().ForEach([&](const TestRecord& record) {
		DateTime testDurration = DateTime::GetCurrentSystemTimeData();
		try
		{
			if (record.Value->Excecute())
			{
				toCoreLog(L"Тест \"" + record.Key + L"\" успішно пройдено");
				testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
				results += {record.Key, TestResult::EStatus::Success, "", testDurration};
			}
			else
			{
				toCoreLog(L"Тест \"" + record.Key + L"\" не пройдено");
				testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
				results += {record.Key, TestResult::EStatus::Failure, L"Вернув false", testDurration};
			}
		}
		catch (Asserts::AssertException & ex)
		{
			toCoreLog(L"Спрацював ассерт в тесті \"" + record.Key + L"\": " + ex.AssertMessage);
			testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
			results += {record.Key, TestResult::EStatus::Failure, ex.toString(), testDurration};
		}
		catch (Exception & ex) {
			toCoreLog(L"При обробці тесту " + record.Key + L" виникла помилка ", ex);
			testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
			results += {record.Key, TestResult::EStatus::Exception, ex.toString(), testDurration};
		}
		catch (...) {
			toCoreLog(L"При обробці тесту " + record.Key + L" виникла непередбачувана помилка");
			testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
			results += {record.Key, TestResult::EStatus::Exception, L"Uknown", testDurration};
		}
		});

	DateTime endTime = DateTime::GetCurrentSystemTimeData();
	return TestPackResult(String(L"Тести ядра"), results, startTime, endTime);
}
