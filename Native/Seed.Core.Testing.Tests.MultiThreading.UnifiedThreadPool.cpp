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
    Asserts::AssertEqual(pool.SheduledTaskCount, 0);

    Asserts::AssertEqual(pool.Name, L"Test thread pool");

    Asserts::AssertEqual(pool.JobsPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High);
    Asserts::AssertEqual(pool.SheduledTasksPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::Low);
    Asserts::AssertEqual(pool.IORequestPriority, Core::Threading::UnifiedThreadPool::EPoolThreadsPriority::High);

    return true;
}
