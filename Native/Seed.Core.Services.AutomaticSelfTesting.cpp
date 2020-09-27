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
	This->testRunnerJob->Interupt().Join();

	delete This->testRunnerJob;
	delete This->sheduler;
}

Seed::Core::Services::AutomatiSelfTestingService::AutomatiSelfTestingService() : This(new Data())
{
	auto testSheduler = Threading::TaskSheduler(L"Automatic self tetsing thread pool", 1, 1);
	auto testTask = testSheduler
		.NewTask(new AutomaticTestingTask(This))
		.SetLoopDuration(Time::TimeSpan(CurrentSettings().KernelServices.AutomaticTestingServiceCfg.SelfTestingRetryTimeout))
		.SetLoopsCount(Math::Max<Int32>())
		.SetName(L"Automatic self testing task")
		.Shedule();

	This->sheduler = new Threading::TaskSheduler(MoveRValue(testSheduler));
	This->testRunnerJob = new Threading::SheduledJob(MoveRValue(testTask));
}

Seed::Core::Testing::TestPackResult Seed::Core::Services::AutomatiSelfTestingService::RunTestsNow(Boolean isLogging)
{
	return This->testRunLock.ExclusiveLock([&]() {
		return Testing::TestRegister::RunAllTest(isLogging);
	});
}

