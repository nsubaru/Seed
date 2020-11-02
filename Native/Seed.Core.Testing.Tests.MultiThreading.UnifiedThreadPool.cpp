#include "Seed.Core.Threading.h"

#include "Seed.Core.Testing.Tests.MultiThreading.UnifiedThreadPool.h"

bool Seed::Core::Testing::Test::MultiThreading::UnifiedThreadPool::CreateEmptyUnifiedThreadPool::Excecute()
{
    auto pool = Core::Threading::UnifiedThreadPoolBuilder()
        .SetName(L"Test thread pool")
        .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetMinThreadCount(1)
        .SetMaxThreadCount(8)
        .CreateThreadPool();

    Asserts::AssertEqual(pool.IORequestCount, 0);
    Asserts::AssertEqual(pool.JobsCount, 0);
    Asserts::AssertEqual(pool.ScheduledTaskCount, 0);

    Asserts::AssertEqual(pool.Name, L"Test thread pool");

    Asserts::AssertEqual(pool.JobsPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High);
    Asserts::AssertEqual(pool.ScheduledTasksPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low);
    Asserts::AssertEqual(pool.IORequestPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High);

    return true;
}

bool Seed::Core::Testing::Test::MultiThreading::UnifiedThreadPool::CreateUnifiedThreadPoolWithJobs::Excecute()
{
    constexpr SizeT jobsCount = 1000;

    auto pool = Core::Threading::UnifiedThreadPoolBuilder()
        .SetName(L"Test thread pool")
        .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetMinThreadCount(1)
        .SetMaxThreadCount(8)
        .CreateThreadPool();

    auto jobsResults = Core::Containers::ConcurencyList<bool>();

    auto job1 = pool.PostJob(Core::Delegate([](Core::Containers::ConcurencyList<bool> jobResults) {
        Core::Threading::Sleep(100);
        jobResults += true;
        }, jobsResults));

    auto job2 = pool.PostJob(Core::Delegate([](Core::Containers::ConcurencyList<bool> jobResults) {
        Core::Threading::Sleep(100);
        jobResults += true;
        }, jobsResults));

    auto job3 = pool.PostJob(Core::Delegate([](Core::Containers::ConcurencyList<bool> jobResults) {
        Core::Threading::Sleep(100);
        jobResults += true;
        }, jobsResults));

    job1.Join();
    job2.Join();
    job3.Join();

    Asserts::AssertEqual(job1.IsAlredyExecuted, true);
    Asserts::AssertEqual(job2.IsAlredyExecuted, true);
    Asserts::AssertEqual(job3.IsAlredyExecuted, true);

    Asserts::AssertEqual(jobsResults.Count, 3);

    Asserts::AssertTrue(jobsResults.All(ArrowFunct(x, x == true)));

    Asserts::AssertEqual(pool.JobsCount, 0);

    return true;
}

bool Seed::Core::Testing::Test::MultiThreading::UnifiedThreadPool::CreateUnifiedThreadPoolWithScheduledTasks::Excecute()
{
    class TestSheduleCallback final : public Core::Threading::IUnifiedThreadPoolShedulerCallback {
    private:
        Core::Containers::ConcurencyList<bool> list;
    protected:
        virtual void Dispose() override {}
    public:
        TestSheduleCallback(const Core::Containers::ConcurencyList<bool>& list) : list(list) {}

        virtual void Shedule() override {
            list += Core::Threading::GetThis().GetThreadabledState() == Core::Threading::EThreadabledStates::Executing;
            list += Core::Threading::GetThis().GetThreadableType() == Core::Threading::EThreadableTypes::ScheduledTask;
            list += Core::Threading::GetThis().GetID() != 0;
        }

        virtual ~TestSheduleCallback() { Dispose(); }
    };

    auto pool = Core::Threading::UnifiedThreadPoolBuilder()
        .SetName(L"Test thread pool")
        .SetThreadPoolPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetJobThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetShedulerThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low)
        .SetIOThreadsPriority(Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High)
        .SetMinThreadCount(1)
        .SetMaxThreadCount(8)
        .CreateThreadPool();

    auto jobsResults = Core::Containers::ConcurencyList<bool>();

    auto task = pool.SheduleJob(L"Tests task"S, new TestSheduleCallback(jobsResults))
        .SetExecutionCount(3)
        .SetIntervalLenght(100ms)
        .SetProximity(10ms)
        .SetStartTime(0s)
        .Shedule();

    Core::Threading::Sleep(100ms * 4);

    Asserts::AssertEqual(task.ExecutedCount, 3);
    Asserts::AssertEqual(task.TimerProximity, 10ms);

    Asserts::AssertEqual(jobsResults.Count, 3 * 3);

    Asserts::AssertTrue(jobsResults.All(ArrowFunct(x, x == true)));

    return true;
}
