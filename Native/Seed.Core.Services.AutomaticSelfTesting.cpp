#include "Seed.Core.Services.AutomaticSelfTesting.h"

void Seed::Core::Services::AutomatiSelfTestingService::AutomaticTestingTask::Shedule()
{
	This->testRunLock.ExclusiveLock([&]() {
		
		This->testsResults += Testing::TestRegister::RunAllTest(CurrentSettings().KernelServices.AutomaticTestingServiceCfg.IsLogTestProcessing);

		if (This->testsResults.Count > CurrentSettings().KernelServices.AutomaticTestingServiceCfg.SavedTestsCount) {
			This->testsResults.FirstIt.Remove();
		}
	});
}

void Seed::Core::Services::AutomatiSelfTestingService::Dispose()
{
	if (This->testRunnerJob != nullptr) {
		MoveRValue(*This->testRunnerJob).StopSheduling();
		delete This->testRunnerJob;
	}
}

void Seed::Core::Services::AutomatiSelfTestingService::InitAutoTesting()
{
	if (This->testRunnerJob != nullptr)
	{
		Throw(InvalidArgumentException(ExceptionPosition, L"", L"Тестування вже виконується"));
	}
	auto testTask = Threading::SystemThreadPool::CreatePriorityScheduledTask(new AutomaticTestingTask(This), Threading::EPoolThreadsPriority::Low)
		.SetProximity(CurrentSettings().KernelServices.AutomaticTestingServiceCfg.SelfTestingRetryTimeout / 2)
		.SetIntervalLenght(CurrentSettings().KernelServices.AutomaticTestingServiceCfg.SelfTestingRetryTimeout)
		.SetExecutionCount(Math::Max<Int32>())
		.SetName(L"Automatic self testing task")
		.Shedule();

	This->testRunnerJob = new Threading::UnifiedThreadPoolSheduledTask(MoveRValue(testTask));
}

void Seed::Core::Services::AutomatiSelfTestingService::StopTesting()
{
	if (This->testRunnerJob != nullptr) {
		MoveRValue(*This->testRunnerJob).StopSheduling();
		delete This->testRunnerJob;

		This->testRunnerJob = nullptr;
	}
}

Seed::Core::Testing::TestPackResult Seed::Core::Services::AutomatiSelfTestingService::RunTestsNow(Boolean isLogging)
{
	return This->testRunLock.ExclusiveLock([&]() {
		return Testing::TestRegister::RunAllTest(isLogging);
	});
}

