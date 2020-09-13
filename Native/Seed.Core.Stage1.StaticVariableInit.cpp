// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.CoreLog.Managed.h"
#include "Seed.Core.Threading.TLS.GlobalTLSBlock.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

static RTL_SRWLOCK g_tss_srw;
static RTL_CONDITION_VARIABLE g_tss_cv;

static DWORD const xp_timeout = 100; // ms
static int const uninitialized = 0;
static int const being_initialized = -1;
static int const epoch_start = INT_MIN;

extern void* InternalHeapFree(void* MemBlk);
extern void* InternalHeapAllocate(Seed::SizeT Size);

extern __declspec(noreturn) void EmergencyShutdown(
    Seed::Core::ConstAString File,
    Seed::Core::ConstAString Function,
    Seed::Core::UInt64 Line,
    Seed::Core::ConstString Reason //-V813
);

class InternalHeap final: public Seed::Memory::BasicHeap {
public:
    virtual void* Allocate(Seed::SizeT Size) final {
        return InternalHeapAllocate(Size);
    }

    virtual void* AllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq) final {
        Throw(Seed::NotImplementedException(ExceptionPosition));
    }

    virtual void* Free(void* MemBlk) final {
        return InternalHeapFree(MemBlk);
    }

    virtual void* AllignedFree(void* MemBlk, Seed::SizeT AllocationReq) final {
        Throw(Seed::NotImplementedException(ExceptionPosition));
    }


    virtual Seed::SizeT CurrentMemBlkCount() final {
        return Seed::Core::Memory::CurrentInternalHeapAllocatedMemBlks();
    }

    virtual Seed::SizeT CurrentAllocatedMemory() final {
        return Seed::Core::Memory::CurrentInternalHeapAllocatedMemory();
    }

    virtual Seed::SizeT CurrentHeapUsage() final {
        return Seed::Core::Memory::CurrentInternalHeapHeapsUsage();
    }
};

static Seed::Core::Containers::BaseList<InternalHeap, Seed::SizeT>* TSStorage = nullptr;

void registerTSSVariable(int* const pOnce) noexcept {
    try
    {
        if (TSStorage == nullptr)
        {
            TSStorage = new Seed::Core::Containers::BaseList<InternalHeap, Seed::SizeT>();
        }
        (*TSStorage) += (Seed::SizeT) pOnce;
    }
    catch (Seed::Exception& ex)
    {
        Seed::toCoreLog(L"При реєстрації TSS виникла помилка: ", ex);
        EmergencyShutdown(ExceptionPosition, L"При реєстрації TSS виникла помилка");
    }
    catch (...)
    {
        EmergencyShutdown(ExceptionPosition, L"При реєстрації TSS виникла невідома помилка");
    }
}

void unRegisterTSSVariable() {
    TSStorage->RemoveAll([] (Seed::SizeT&& ppOnce)
    {
        auto pOnce = (int*) ppOnce;
        try
        {
            *pOnce = uninitialized;
        }
        catch (Seed::Exception& ex)
        {
            Seed::toCoreLog(
                L"Warning: exception on TSS unregister on address: " + 
                Seed::ExceptionString::toString(pOnce) + 
                " exception information: ", ex);
        }
        catch (...)
        {
            Seed::toCoreLog(L"Warning: uknown exception on TSS unregister on address: " + Seed::ExceptionString::toString(pOnce));
        }
    });

    delete TSStorage;
    TSStorage = nullptr;
}

// Access to these variables is guarded in the below functions.  They may only
// be modified while the lock is held.  _Tss_epoch is readable from user
// code and is read without taking the lock.
extern "C"
{
    int _Init_global_epoch = epoch_start;
    __declspec(thread) int _Init_thread_epoch = epoch_start;
}

extern "C" void __cdecl _Init_thread_lock()
{
    RtlAcquireSRWLockExclusive(&g_tss_srw);
}

extern "C"  void __cdecl _Init_thread_unlock()
{
    RtlReleaseSRWLockExclusive(&g_tss_srw);
}

extern "C" void __cdecl _Init_thread_notify()
{
    RtlWakeAllConditionVariable(&g_tss_cv);
}

// Wait on the condition variable.  In the XP implementation using only a Windows event
// we can't guarantee that we'll ever actually receive the notification signal, so we
// must use a non-infinite timeout.  This is not optimal: we may wake up early if the
// initializer is long-running, or we may miss the signal and not wake up until the
// timeout expires.  The signal may be missed because the sleeping threads may be
// stolen by the kernel to service an APC, or due to the race condition between the
// unlock call and the WaitForSingleObject call.
extern "C" extern void __cdecl _Init_thread_wait(LARGE_INTEGER timeout)
{
    RtlSleepConditionVariableSRW(&g_tss_cv, &g_tss_srw, &timeout, 0);
}

// Control access to the initialization expression.  Only one thread may leave
// this function before the variable has completed initialization, this thread
// will perform initialization.  All other threads are blocked until the
// initialization completes or fails due to an exception.
extern "C" extern void __cdecl _Init_thread_header(int* const pOnce) noexcept
{
    _Init_thread_lock();

    if (*pOnce == uninitialized)
    {
        *pOnce = being_initialized;
    }
    else
    {
        while (*pOnce == being_initialized)
        {
            // Timeout can be replaced with an infinite wait when XP support is
            // removed or the XP-based condition variable is sophisticated enough
            // to guarantee all waiting threads will be woken when the variable is
            // signalled.
            LARGE_INTEGER timeount;
            timeount.QuadPart = xp_timeout * 1000ULL * 10000ULL;
            _Init_thread_wait(timeount);

            if (*pOnce == uninitialized) //-V547
            {
                *pOnce = being_initialized;
                _Init_thread_unlock();
                return;
            }
        }
        _Init_thread_epoch = _Init_global_epoch;
    }

    _Init_thread_unlock();
}


// Abort processing of the initializer due to an exception.  Reset the state
// to uninitialized and release waiting threads (one of which will take over
// initialization, any remaining will again sleep).
extern "C" extern void __cdecl _Init_thread_abort(int* const pOnce) noexcept
{
    _Init_thread_lock();
    *pOnce = uninitialized;
    _Init_thread_unlock();
    _Init_thread_notify();
}

// Called by the thread that completes initialization of a variable.
// Increment the global and per thread counters, mark the variable as
// initialized, and release waiting threads.
extern "C" extern void __cdecl _Init_thread_footer(int* const pOnce) noexcept
{
    _Init_thread_lock();
    ++_Init_global_epoch;
    *pOnce = _Init_global_epoch;
    _Init_thread_epoch = _Init_global_epoch;
    _Init_thread_unlock();
    _Init_thread_notify();
    registerTSSVariable(pOnce);
}