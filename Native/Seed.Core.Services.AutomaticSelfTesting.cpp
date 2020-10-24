#include "Seed.Core.Services.AutomaticSelfTesting.h"

void Seed::Core::Services::AutomatiSelfTestingService::AutomaticTestingTask::Execute(Time::DateTime startTimeUtc) volatile
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
	if (This->sheduler != nullptr && This->testRunnerJob != nullptr) {
		This->testRunnerJob->Interupt().Join((30s).Ticks);

		delete This->testRunnerJob;
		delete This->sheduler;
	}
}

void Seed::Core::Services::AutomatiSelfTestingService::InitAutoTesting()
{
	if (This->sheduler != nullptr || This->testRunnerJob != nullptr)
	{
		Throw(InvalidArgumentException(ExceptionPosition, L"", L"Тестування вже виконується"));
	}
	auto testSheduler = Threading::TaskSheduler(L"Automatic self tetsing thread pool", 1, 1);
	auto testTask = testSheduler
		.NewTask(new AutomaticTestingTask(This))
		.SetTimerProximity(Time::DateTime(Time::MinutesTicksCount))
		.SetLoopDuration(Time::TimeSpan(CurrentSettings().KernelServices.AutomaticTestingServiceCfg.SelfTestingRetryTimeout))
		.SetLoopsCount(Math::Max<Int32>())
		.SetName(L"Automatic self testing task")
		.Shedule();

	This->sheduler = new Threading::TaskSheduler(MoveRValue(testSheduler));
	This->testRunnerJob = new Threading::SheduledJob(MoveRValue(testTask));
}

void Seed::Core::Services::AutomatiSelfTestingService::StopTesting()
{
	if (This->sheduler != nullptr && This->testRunnerJob != nullptr) {
		This->testRunnerJob->Interupt().Join((30s).Ticks);
		delete This->testRunnerJob;
		delete This->sheduler;

		This->testRunnerJob = nullptr;
		This->sheduler = nullptr;
	}
}

Seed::Core::Testing::TestPackResult Seed::Core::Services::AutomatiSelfTestingService::RunTestsNow(Boolean isLogging)
{
	return This->testRunLock.ExclusiveLock([&]() {
		return Testing::TestRegister::RunAllTest(isLogging);
	});
}

