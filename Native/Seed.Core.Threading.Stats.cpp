#include "Seed.Core.Threading.h"

namespace Seed::Core::Threading
{
#pragma region Values
    ForceInline static Atomic<UInt64>& ActiveThreadsCountPerfromanceCounter()
    {
        static Atomic<UInt64> data = { 0 };
        return data;
    }

    ForceInline static Atomic<UInt64>& TasksCreateCountPerfromanceCounter()
    {
        static Atomic<UInt64> data = { 0 };
        return data;
    }

    ForceInline static Atomic<UInt64>& TasksDisposedCountPerfromanceCounter()
    {
        static Atomic<UInt64> data = { 0 };
        return data;
    }

    ForceInline static Atomic<UInt64>& CompletedIoOperationsCount()
    {
        static Atomic<UInt64> data = { 0 };
        return data;
    }

    ForceInline static Atomic<UInt64>& CompletedAsyncAwaitOperationsCount()
    {
        static Atomic<UInt64> data = { 0 };
        return data;
    }
#pragma endregion

#pragma region Values
    void ResetThreadingStats()
    {
        ActiveThreadsCountPerfromanceCounter() = 0;
        TasksCreateCountPerfromanceCounter() = 0;
        TasksDisposedCountPerfromanceCounter() = 0;
        CompletedIoOperationsCount() = 0;
        CompletedAsyncAwaitOperationsCount() = 0;
    }
#pragma endregion

#pragma region Update counters
    void IncreaseActiveThreadPerfomanceCounter()
    {
        ++ActiveThreadsCountPerfromanceCounter();
    }

    void DecreaseActiveThreadPerfomanceCounter()
    {
        --ActiveThreadsCountPerfromanceCounter();
    }

    void IBaseTask::IncrementTasksCreateCountPerfromanceCounter()
    {
        ++TasksCreateCountPerfromanceCounter();
    }

    void IBaseTask::IncrementTasksDisposedCountPerfromanceCounter()
    {
        ++TasksDisposedCountPerfromanceCounter();
    }

    void IncrementCompletedIoOperationsCount()
    {
        ++CompletedIoOperationsCount();
    }

    void IncrementCompletedAsyncAwaitOperationsCount()
    {
        ++CompletedAsyncAwaitOperationsCount();
    }
#pragma endregion

#pragma region Read
    UInt64 GetActiveThreadsCountPerfromanceCounter()
    {
        return ActiveThreadsCountPerfromanceCounter().Read();
    }

    UInt64 GetTasksCreateCountPerfromanceCounter()
    {
        return TasksCreateCountPerfromanceCounter().Read();
    }

    UInt64 GetTasksDisposedCountPerfromanceCounter()
    {
        return TasksDisposedCountPerfromanceCounter().Read();
    }

    UInt64 GetCompletedIoOperationsCount()
    {
        return CompletedIoOperationsCount().Read();
    }

    UInt64 GetCompletedAsyncAwaitOperationsCount()
    {
        return CompletedAsyncAwaitOperationsCount().Read();
    }
#pragma endregion
}