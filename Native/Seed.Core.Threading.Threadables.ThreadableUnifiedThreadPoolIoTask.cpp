#include "Seed.Core.Threading.Threadables.ThreadableUnifiedThreadPoolIoTask.h"

#include "Seed.Core.Threading.Threadables.Base.Internal.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

#pragma runtime_checks("", off)
#pragma check_stack(off)
extern __declspec(noreturn, noinline, safebuffers) void InteruptThread();
#pragma check_stack(on)
#pragma runtime_checks("", restore)

using namespace Seed;
using namespace Seed::Core::Threading;

Seed::Core::Threading::Threadables::ThreadableRef Seed::Core::Threading::Threadables::RegisterPoolIoThreadable(
	const UnifiedThreadPoolIoTask& task, 
	const SynchronizeEvent& event,
	ImplSpecificT instance)
{
	HANDLE hThread = nullptr;
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &hThread, GENERIC_ALL, 0, 0);
	auto threadable = new ThreadableUnifiedThreadPoolIoTaskV2(task, event, hThread, Threading::GetTID(), instance);
	return RegisterIThreadable(MoveRValue(threadable), &threadable->This->state, &threadable->This->secondState);
}

void Seed::Core::Threading::Threadables::UnRegisterPoolIoThreadable(ThreadableRef&& ref)
{
	NtClose(ref->As<ThreadableUnifiedThreadPoolIoTaskV2>().This->hThread);
	ref->As<ThreadableUnifiedThreadPoolIoTaskV2>().This->instance = nullptr;
	ref->As<ThreadableUnifiedThreadPoolIoTaskV2>().This->state = Threading::EThreadabledStates::Ended;
	UnRegisterIThreadable(MoveRValue(ref));
}

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolIoTaskV2::Suspend()
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

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolIoTaskV2::Resume()
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

IThreadable& Seed::Core::Threading::Threadables::ThreadableUnifiedThreadPoolIoTaskV2::Interupt()
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

			This->secondState = EThreadabledStates::Interupted;
			Swap(This->state, This->secondState);

			CONTEXT ctx = { 0 };
			ctx.ContextFlags = CONTEXT_ALL;
			NTSTATUS status = ZwGetContextThread(This->hThread, &ctx);
			if (!NT_SUCCESS(status))
			{
				NtAlertResumeThread(This->hThread, NULL);
				Throw(NtException(ExceptionPosition, status));
			}
			DWORD64 ret_point = ctx.Rip;
			ctx.Rip = (DWORD64)(PDWORD64)InteruptThread;
			status = ZwSetContextThread(This->hThread, &ctx);

			NtResumeThread(This->hThread, NULL);
			if (!NT_SUCCESS(status))
				Throw(NtException(ExceptionPosition, status));

			//We must call this after resume for resume thread if it wait for something
			NtAlertThread(This->hThread);
		}
		default:
			break;
		}
		break;
	}
	return *this;
}

#pragma warning(pop)
