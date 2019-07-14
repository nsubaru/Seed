#include "Seed.Core.TaskShedule.Sheduler.h"

#include "Seed.Core.Threading.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

struct {
	PTP_POOL TaskShedulerPool;
	TP_CALLBACK_ENVIRON TaskShedulerPoolEnviroment;
	PTP_CLEANUP_GROUP TaskShedulerPoolCleanupGroup;
} Sheduler;

VOID NTAPI TaskShedulerTaskCallback(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_TIMER             Timer
) {
	using namespace Seed;
	Seed::TaskSheduler::Job::__task* This = (Seed::TaskSheduler::Job::__task*)Context;
	This->TID = (Seed::UInt64)NtCurrentThreadId();

	try {
		try {
			This->IsExcecute = true;
			This->What->Execute(Seed::Core::DateTime::GetCurrentSystemTimeData());
			This->IsExcecute = false;
			This->ExucutedCount++;
			if (--This->PeriodsCount == 0) {
				TpReleaseTimer(Timer);
				This->hTask = 0;
				if (--This->RefCount == 0) {
					delete This->What;
					delete This;
				}
			}
			return;
		}
		catch ([[maybe_unused]] InteruptException& ex) {
			toCoreLog(L"Задача в планувальнику задач в потоці з ID " + String::toString(This->TID) + "перервано");
			This->IsExcecute = false;
			This->ExucutedCount++;
			if (--This->PeriodsCount == 0) {
				TpReleaseTimer(Timer);
				This->hTask = 0;
				if (--This->RefCount == 0) {
					delete This->What;
					delete This;
				}
			}
			return;
		}
		catch (Exception& ex) {
			toCoreLog({ L"Задача у планувальнику задач в потоці з ID " + String::toString(This->TID) + " перервано помилкою." ,
					ex.toString()
				});
			This->IsExcecute = false;
			This->ExucutedCount++;
			if (--This->PeriodsCount == 0) {
				TpReleaseTimer(Timer);
				This->hTask = 0;
				if (--This->RefCount == 0) {
					delete This->What;
					delete This;
				}
			}
			return;
		}
	}
	catch (Exception& ex) {
		toCoreLog(L"Внутрішня помилка при потоку", ex);
		This->IsExcecute = false;
		This->ExucutedCount++;
		if (--This->PeriodsCount == 0) {
			TpReleaseTimer(Timer);
			This->hTask = 0;
			if (--This->RefCount == 0) {
				delete This->What;
				delete This;
			}
		}
		return;
	}
}

VOID NTAPI TaskShedulerCleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
	Seed::TaskSheduler::Job::__task* This = (Seed::TaskSheduler::Job::__task*)ObjectContext;
	if (CleanupContext == NULL) {
		if (This != nullptr) {
			if (--This->RefCount == 0) {
				TpReleaseTimer(reinterpret_cast<PTP_TIMER>(This->hTask));
				This->hTask = nullptr;
				delete This->What;
				delete This;
			}
		}
	}
	else {
		if(This->hTask != nullptr)
			TpReleaseTimer(reinterpret_cast<PTP_TIMER>(This->hTask));
		delete This->What;
		delete This;
	}
}


bool Seed::Core::Init::TaskSheduling() noexcept {
	NTSTATUS status = TpAllocPool(&Sheduler.TaskShedulerPool, NULL);
	if (!NT_SUCCESS(status))
		return false;
	TpSetPoolMinThreads(Sheduler.TaskShedulerPool, static_cast<LONG>(CurrentSettings::CPU::GetCpuCoresCount() / 2));
	TpSetPoolMaxThreads(Sheduler.TaskShedulerPool, static_cast<LONG>(CurrentSettings::CPU::GetCpuCoresCount()));
	TP_POOL_STACK_INFORMATION info;
	info.StackCommit = CurrentSettings::Threading::Thread::GetStackStartSize();
	info.StackReserve = CurrentSettings::Threading::Thread::GetStackMaxSize();
	TpSetPoolStackInformation(Sheduler.TaskShedulerPool, &info);
	TpAllocCleanupGroup(&Sheduler.TaskShedulerPoolCleanupGroup);
	
	Sheduler.TaskShedulerPoolEnviroment.Version = 3;
	Sheduler.TaskShedulerPoolEnviroment.Pool = Sheduler.TaskShedulerPool;
	Sheduler.TaskShedulerPoolEnviroment.CleanupGroup = Sheduler.TaskShedulerPoolCleanupGroup;
	Sheduler.TaskShedulerPoolEnviroment.CleanupGroupCancelCallback = TaskShedulerCleanupCallback;
	Sheduler.TaskShedulerPoolEnviroment.RaceDll = NULL;
	Sheduler.TaskShedulerPoolEnviroment.ActivationContext = NULL;
	Sheduler.TaskShedulerPoolEnviroment.FinalizationCallback = NULL;
	Sheduler.TaskShedulerPoolEnviroment.u.Flags = 0;
	Sheduler.TaskShedulerPoolEnviroment.CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
	Sheduler.TaskShedulerPoolEnviroment.Size = sizeof(TP_CALLBACK_ENVIRON);
	return true;
}

void Seed::Core::Stop::TaskSheduling() noexcept {
	TpReleaseCleanupGroupMembers(Sheduler.TaskShedulerPoolCleanupGroup, TRUE, (void*)(SizeT)TRUE); //-V566
	TpReleaseCleanupGroup(Sheduler.TaskShedulerPoolCleanupGroup);
	TpReleasePool(Sheduler.TaskShedulerPool);
}

extern __declspec(noreturn, noinline) void InteruptThread();

void Seed::Core::TaskSheduler::Job::Cancel()
{
	if (!This->IsCanceled) {
		if (!This->IsExcecute) {
			TpReleaseTimer(reinterpret_cast<PTP_TIMER>(This->hTask));
		}
		else {
			Handle threadHandle;
			CLIENT_ID id;
			id.UniqueProcess = NtCurrentProcessId();
			id.UniqueThread = reinterpret_cast<HANDLE>(This->TID);
			NTSTATUS status;
			OBJECT_ATTRIBUTES obj;
			InitializeObjectAttributes(&obj, NULL, 0, NULL, NULL);
			status = NtOpenThread(&threadHandle, GENERIC_ALL, &obj, &id);
			status = NtSuspendThread(threadHandle, NULL);
			CONTEXT ctx;
			ctx.ContextFlags = CONTEXT_ALL;
			status = ZwGetContextThread(threadHandle, &ctx);
			if (!NT_SUCCESS(status)) {
				NtResumeThread(threadHandle, NULL);
				Throw(NtException(ExceptionPosition, status));
			}
			DWORD64 ret_point = ctx.Rip;
			ctx.Rip = (DWORD64)(PDWORD64) &InteruptThread;
			status = ZwSetContextThread(threadHandle, &ctx);
			NtResumeThread(threadHandle, NULL);
			NtAlertThread(threadHandle);
			if (!NT_SUCCESS(status))
				Throw(NtException(ExceptionPosition, status));
			NtClose(threadHandle);
		}
	}
}

void Seed::Core::TaskSheduler::Job::Dispose()
{
}

void Seed::Core::TaskSheduler::Job::SetPeriodLenght(DateTime PeriodLenght)
{
	This->PeriodLenght = static_cast<Int32>(PeriodLenght.TimeFromStartEpoch);

	LARGE_INTEGER DueTime;

	DueTime.QuadPart = This->StartTime.TimeFromStartEpoch;

	TpSetTimer(
		const_cast<PTP_TIMER>(reinterpret_cast<PTP_TIMER>(This->hTask)), 
		&DueTime, 
		static_cast<Int32>(This->PeriodsCount), 
		static_cast<Int32>(This->TimerProximity)
	);
}

void Seed::Core::TaskSheduler::Job::SetTimerProximity(DateTime NewTimerProximity)
{
	This->TimerProximity = static_cast<Int32>(NewTimerProximity.TimeFromStartEpoch);

	LARGE_INTEGER DueTime;

	DueTime.QuadPart = This->StartTime.TimeFromStartEpoch;

	TpSetTimer(
		const_cast<PTP_TIMER>(reinterpret_cast<PTP_TIMER>(This->hTask)),
		&DueTime,
		static_cast<Int32>(This->PeriodsCount),
		static_cast<Int32>(This->TimerProximity)
	);
}

void Seed::Core::TaskSheduler::Job::SetPeriodsCount(Int32 NewPeriodsCount)
{
	if (This->PeriodLenght == 0)
		Throw(
			InvalidArgumentException(
				ExceptionPosition, 
				L"PeriodsCount", 
				L"Не можливо вказати кількість періодів якщо не вказана тривалість одного"
			)
		);

	This->PeriodsCount = NewPeriodsCount;

	LARGE_INTEGER DueTime;

	DueTime.QuadPart = This->StartTime.TimeFromStartEpoch;

	TpSetTimer(
		const_cast<PTP_TIMER>(reinterpret_cast<PTP_TIMER>(This->hTask)),
		&DueTime,
		static_cast<Int32>(This->PeriodsCount),
		static_cast<Int32>(This->TimerProximity)
	);
}

Seed::Core::TaskSheduler::Job Seed::Core::TaskSheduler::Task::Shedule()
{

	if (Data->StartTime.TimeFromStartEpoch == 0 && Data->PeriodLenght == 0)
		Throw(
			InvalidArgumentException(
				ExceptionPosition, 
				L"StartTime", 
				L"Час запуску або довжина періода мають бути вказані"
			)
		);
	if (Data->What == nullptr)
		Throw(
			InvalidArgumentException(
				ExceptionPosition, 
				L"Task", 
				L"Дія має бути вказана обов'язково"
			)
		);

	TpAllocTimer(
		const_cast<PTP_TIMER*>(reinterpret_cast<volatile PTP_TIMER*>(&Data->hTask)),
		TaskShedulerTaskCallback,
		Data,
		&Sheduler.TaskShedulerPoolEnviroment
	);

	LARGE_INTEGER DueTime;

	DueTime.QuadPart = Data->StartTime.TimeFromStartEpoch;

	TpSetTimer(
		const_cast<PTP_TIMER>(reinterpret_cast<PTP_TIMER>(Data->hTask)), 
		&DueTime, 
		static_cast<Int32>(Data->PeriodLenght), 
		static_cast<Int32>(Data->TimerProximity)
	);

	return Job(Data);
}

#pragma warning(pop)
