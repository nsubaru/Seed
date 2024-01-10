#pragma once
#include "Seed.Core.Threading.Base.h"

namespace Seed::Core::Threading
{

    /// <summary>
    /// Returns count of currently active threads
    /// </summary>
    /// <returns></returns>
    UInt64 GetActiveThreadsCountPerfromanceCounter();

    /// <summary>
    /// Returns count of created Task object from threading API init
    /// </summary>
    /// <returns>Count of created Task object</returns>
    UInt64 GetTasksCreateCountPerfromanceCounter();

    /// <summary>
    /// Returns count of disposed Task object from threading API init
    /// </summary>
    /// <returns>Count of disposed Task object</returns>
    UInt64 GetTasksDisposedCountPerfromanceCounter();

    /// <summary>
    /// Returns count of completed async IO operations from threading API init
    /// </summary>
    /// <returns>Count of completed async IO operations</returns>
    UInt64 GetCompletedIoOperationsCount();

    /// <summary>
    /// Returns count of completed async wait operations from threading API init
    /// </summary>
    /// <returns>Count of completed async wait operations</returns>
    UInt64 GetCompletedAsyncAwaitOperationsCount();
}