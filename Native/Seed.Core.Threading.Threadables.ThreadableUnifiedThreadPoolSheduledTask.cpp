#include "Seed.Core.Threading.Threadables.ThreadableUnifiedThreadPoolSheduledTask.h"

#include "Seed.Core.Threading.Threadables.Base.Internal.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma runtime_checks("", off)
#pragma check_stack(off)
extern __declspec(noreturn, noinline, safebuffers) void InteruptThread();
#pragma check_stack(on)
#pragma runtime_checks("", restore)

using namespace Seed;
using namespace Seed::Threading;

Seed::Core::Threading::Threadables::ThreadableRef Seed::Core::Threading::Threadables::RegisterPoolSheduledThreadable(const UnifiedThreadPoolSheduledTask& task, ImplSpecificT instance)
{
	HANDLE hThread = nullptr;
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &hThread, GENERIC_ALL, 0, 0);
	SynchronizeEvent event = SynchronizeEvent(true);
	auto threadable = new ThreadableUnifiedThreadPoolSheduledTaskV2(task, event, hThread, Threading::GetTID(), instance);
	return RegisterIThreadable(MoveRValue(threadable), &threadable->This->state);
}

void Seed::Core::Threading::Threadables::UnRegisterPoolSheduledThreadable(ThreadableRef&& ref)
{
	NtClose(ref->As<ThreadableUnifiedThreadPoolSheduledTaskV2>().This->hThread);
	ref->As<ThreadableUnifiedThreadPoolSheduledTaskV2>().This->instance = nullptr;
	ref->As<ThreadableUnifiedThreadPoolSheduledTaskV2>().This->state = Threading::EThreadabledStates::Ended;
	UnRegisterIThreadable(MoveRValue(ref));
}

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolSheduledTaskV2::Suspend()
{
	_ReadWriteBarrier();

	switch (This->state)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing:
	case EThreadabledStates::ExceptionProcessing:
	{
		This->secondState = EThreadabledStates::Paused;
		Swap(This->state, This->secondState);

		TpDisassociateCallback((PTP_CALLBACK_INSTANCE)This->instance);

		NTSTATUS status = NtSuspendThread(This->hThread, NULL);
		if (!NT_SUCCESS(status)) {
			Swap(This->state, This->secondState);
			return *this;
		}

		break;
	}
	case EThreadabledStates::Paused: break;
	default:
		break;
	}
	return *this;
}

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolSheduledTaskV2::Resume()
{
	_ReadWriteBarrier();

	switch (This->state)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing: break;
	case EThreadabledStates::ExceptionProcessing: break;
	case EThreadabledStates::Paused:
	{
		Swap(This->state, This->secondState);
		NtAlertResumeThread(This->hThread, NULL);
		break;
	}
	default:
		break;
	}
	return *this;
}

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolSheduledTaskV2::Interupt()
{
	_ReadWriteBarrier();

	switch (This->state)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	default:
		switch (This->state)
		{
		case EThreadabledStates::Executing:
		case EThreadabledStates::ExceptionProcessing:
		case EThreadabledStates::Paused:
		{
			NtSuspendThread(This->hThread, NULL);
			CONTEXT ctx = { 0 };
			ctx.ContextFlags = CONTEXT_ALL;
			NTSTATUS status = ZwGetContextThread(This->hThread, &ctx);
			if (!NT_SUCCESS(status))
			{
				NtResumeThread(This->hThread, NULL);
				Throw(NtException(ExceptionPosition, status));
			}
			DWORD64 ret_point = ctx.Rip;
			ctx.Rip = (DWORD64)(PDWORD64)InteruptThread;
			status = ZwSetContextThread(This->hThread, &ctx);
			NtResumeThread(This->hThread, NULL);
			if (!NT_SUCCESS(status))
				Throw(NtException(ExceptionPosition, status));
			NtAlertThread(This->hThread);
		}
		default:
			break;
		}
		break;
	}
	return *this;
}
