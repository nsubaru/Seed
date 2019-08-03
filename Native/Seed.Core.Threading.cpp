// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Threading.h"
#include "Seed.Core.Threading.ThreadPool.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.CoreLog.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

static Seed::List<Seed::Threading::Thread>* Threads;

void Seed::Core::Threading::Thread::Event::Dispose()
{
	if (data->handle != nullptr)
		NtClose(data->handle);
}

Seed::Core::Threading::Thread::Event::Event(Seed::Boolean IsManualReset) : data(new event_data(IsManualReset, String()))
{
	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		NULL,
		(IsManualReset) ? EVENT_TYPE::NotificationEvent: EVENT_TYPE::SynchronizationEvent,
		FALSE
	);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

Seed::Core::Threading::Thread::Event::Event(const Seed::String & name, Seed::Boolean IsManualReset) : data(new event_data(IsManualReset, name)) {
	OBJECT_ATTRIBUTES attrbs;
	UNICODE_STRING label;
	RtlInitUnicodeStringEx(&label, (PWSTR)(ConstCharStr)data->Name);
	InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		&attrbs,
		(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
		FALSE
	);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

Seed::Core::Threading::Thread::Event::Event(Seed::String && name, Seed::Boolean IsManualReset) : data(new event_data(IsManualReset, name)) {
	OBJECT_ATTRIBUTES attrbs;
	UNICODE_STRING label;
	RtlInitUnicodeStringEx(&label, (PWSTR)(ConstCharStr)data->Name);
	InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		&attrbs,
		(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
		FALSE
	);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

void Seed::Core::Threading::Thread::Event::Wait()
{
	LARGE_INTEGER delay;
	delay.QuadPart = Math::Max<Int64>();
	NtWaitForSingleObject(this->data->handle, TRUE, &delay);
}

void Seed::Core::Threading::Thread::Event::Wait(SizeT TimeOutMs)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (TimeOutMs > 0) ? -10000LL * TimeOutMs : Math::Max<Int64>();
	NtWaitForSingleObject(this->data->handle, TRUE, &delay);
}

void Seed::Core::Threading::Thread::Dispose()
{
	delete this->thread->EntryPoint;
}

Seed::Core::Threading::Thread Seed::Core::Threading::Thread::GetThis()
{
	ID TID = GetTID();
	for (auto& thread : *Threads) {
		if (thread.thread->ID == TID)
			return thread;
	}
	Throw(InvalidThread(ExceptionPosition, TID));
}

Seed::Core::Threading::Thread Seed::Core::Threading::Thread::GetByID(UInt64 TargetTID)
{
	for (auto& thread : *Threads) {
		if (thread.thread->ID == TargetTID)
			return thread;
	}
	Throw(InvalidThread(ExceptionPosition, TargetTID));
}

Seed::ID Seed::Core::Threading::Thread::GetTID() {
	return reinterpret_cast<ID>(NtCurrentThreadId());
}

void Seed::Core::Threading::Thread::Sleep(UInt64 ms)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (ms > 0) ? -10000LL * ms : Math::Max<Int64>();
	NtDelayExecution(TRUE, &delay);
}

void Seed::Core::Threading::Thread::Sleep()
{
	LARGE_INTEGER delay;
	delay.QuadPart = Math::Max<Int64>();
	NtDelayExecution(TRUE, &delay);
}

void Seed::Core::Threading::Thread::Wait(UInt64 TargetTID, SizeT Timeout)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (Timeout > 0) ? -10000LL * Timeout : Math::Max<Int64>();
	NtDelayExecution(TRUE, &delay);
	for (auto& thread : *Threads) {
		if (thread.thread->ID == TargetTID)
		{
			NtWaitForSingleObject(thread.thread->handle, TRUE, &delay);
			return;
		}
	}
	Throw(InvalidThread(ExceptionPosition, TargetTID));
}

Seed::Core::List<Seed::Core::Threading::Thread> Seed::Core::Threading::Thread::GetByLabel(const Seed::Core::String & Label)
{
	List<Thread> rets;
	for (auto& thread : *Threads) {
		if (thread.thread->Label == Label)
		{
			rets += thread;
		}
	}
	return rets;
}

Seed::Core::List<Seed::Core::Threading::Thread> Seed::Core::Threading::Thread::GetAllThreads()
{
	return *Threads;
}

bool Seed::Core::Threading::InitCoreThreading() noexcept
{
	auto nativeMainThread = Thread(L"Native main thread");
	THREAD_NAME_INFORMATION name;
	RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)nativeMainThread.thread->Label);
	NTSTATUS status = NtSetInformationThread(
		NtCurrentThread(),
		THREADINFOCLASS::ThreadNameInformation,
		&name,
		sizeof(THREAD_NAME_INFORMATION));
	if (!NT_SUCCESS(status))
		return false;
	status =
		NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &nativeMainThread.thread->handle, GENERIC_ALL, 0, 0);
	if (!NT_SUCCESS(status))
		return false;
	nativeMainThread.thread->state = Thread::ThreadState::Work;
	nativeMainThread.thread->ID = Thread::GetTID();
	try {
		Threads = new List<Thread>();
		*Threads += nativeMainThread;
	}
	catch (...) {
		toCoreLog(L"Не вдалось ініціалізувати підсистему потоків");
		return false;
	}
	return true;
}

void Seed::Core::Threading::StopCoreThreading() noexcept
{
	auto thread = Threads->First;
	while (thread != nullptr) {
		if (thread->TID != Thread::GetTID()) {
			thread->Interupt().Join();
			thread.Remove();
		}
		else
			thread++;
	}
	delete Threads;
}

Seed::Int32 Seed::Core::Threading::ThreadInitRoute(void* arg) noexcept try {
	Thread* This = static_cast<Thread*>(arg);
	THREAD_NAME_INFORMATION name;
	RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)This->thread->Label);
	NtSetInformationThread(
		NtCurrentThread(),
		THREADINFOCLASS::ThreadNameInformation,
		&name,
		sizeof(THREAD_NAME_INFORMATION)
	);
	try {
		try {
			This->thread->abortEvent->Set();
			This->thread->EntryPoint->Init();
		}
		catch ([[maybe_unused]] InteruptException & ex) {
			toCoreLog(L"Роботу потоку з ID " + String::toString(This->thread->ID) + "і міткою" + This->thread->Label + "перервано");
			This->thread->state = Seed::Threading::Thread::ThreadState::Terminated;
			NtTestAlert();
			This->thread->ID = -1;
			Threads->Remove(This->thread->thread_list_node);
			delete This->thread->abortEvent;
			delete This;
			return STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exception & ex) {
			toCoreLog({ L"Роботу потоку з ID " + String::toString(This->thread->ID) + "і міткою" + This->thread->Label + "перервано помилкою." ,
					ex.toString()
				});
			This->thread->state = Seed::Threading::Thread::ThreadState::ClosedByExeption;
			NtTestAlert();
			This->thread->ID = -1;
			Threads->Remove(This->thread->thread_list_node);
			delete This->thread->abortEvent;
			delete This;
			return STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (Exception & ex) {
		toCoreLog(L"Внутрішня помилка при потоку", ex);
		This->thread->state = Seed::Threading::Thread::ThreadState::ClosedByExeption;
		NtTestAlert();
		This->thread->ID = -1;
		Threads->Remove(This->thread->thread_list_node);
		delete This->thread->abortEvent;
		delete This;
		return STATUS_UNHANDLED_EXCEPTION;
	}
	This->thread->state = Seed::Threading::Thread::ThreadState::CorrectlyEnded;
	NtTestAlert();
	This->thread->ID = -1;
	Threads->Remove(This->thread->thread_list_node);
	delete This->thread->abortEvent;
	delete This;
	return STATUS_SUCCESS;
}
catch (...) {
	toCoreLog(L"Внутрішня помилка при потоку");
	return STATUS_UNHANDLED_EXCEPTION;
}

NTSTATUS __cdecl threadStartRoutineDynEntryPoint(PVOID args) {
	extern void InitThreadEH();
	void StopThreadEH();

	InitThreadEH();
	auto result = Seed::Core::Threading::ThreadInitRoute(args);
	StopThreadEH();

	return result;
}

void Seed::Core::Threading::Thread::ExecDelegate(Seed::Delegate && ThreadMain) {
	thread->EntryPoint = new Delegate(MoveRValue(ThreadMain));
	if (this->thread->thread_list_node != nullptr)
		Throw(ThreadAlredyStrated(ExceptionPosition));
	toCoreLog(String(L"Запуск потоку з міткою ") + this->thread->Label + L" з потоку з ID " + String::toString(Thread::GetTID()));
	this->thread->thread_list_node = Threads->AddLast(*this);
	this->thread->abortEvent = new Event();
	CLIENT_ID id;
	size_t stack_max = 10485760, stack_alloc = 524288;
	RtlCreateUserThread(
		NtCurrentProcess(),
		NULL,
		false,
		0,
		Seed::CurrentSettings().Threading.Thread.StackMaxSize,
		Seed::CurrentSettings().Threading.Thread.StackStartSize,
		threadStartRoutineDynEntryPoint,
		(void*)new Thread(*this), //-V572
		&thread->handle,
		&id);
	this->thread->state = Seed::Threading::Thread::ThreadState::Work;
	this->thread->ID = reinterpret_cast<SizeT>(id.UniqueThread);
}

void Seed::Core::Threading::Thread::ChangeLabel(const String & NewLabel) {
	if (thread->ID != -1) {
		if (this->GetTID() == thread->ID) {
			thread->Label = NewLabel;
			THREAD_NAME_INFORMATION name;
			RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)thread->Label);
			NtSetInformationThread(
				NtCurrentThread(),
				THREADINFOCLASS::ThreadNameInformation,
				&name,
				sizeof(THREAD_NAME_INFORMATION)
			);
		}
		else {
			ULONG suspendCount;
			NTSTATUS status = NtSuspendThread(thread->handle, &suspendCount);
			if (NT_SUCCESS(status))
				Throw(InvalidThreadObj(ExceptionPosition));
			thread->Label = NewLabel;
			THREAD_NAME_INFORMATION name;
			RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)thread->Label);
			NtSetInformationThread(
				NtCurrentThread(),
				THREADINFOCLASS::ThreadNameInformation,
				&name,
				sizeof(THREAD_NAME_INFORMATION)
			);
			NtResumeThread(thread->handle, &suspendCount);
		}
	}
	else
		Throw(InvalidThreadObj(ExceptionPosition));
}

void Seed::Core::Threading::Thread::ExecApc() {
	if (thread->ID != -1) {
		ULONG suspendCount;
		NTSTATUS status = NtSuspendThread(thread->handle, &suspendCount);
		if (NT_SUCCESS(status))
			Throw(InvalidThreadObj(ExceptionPosition));
		NtAlertResumeThread(thread->handle, &suspendCount);
	}
	else
		Throw(InvalidThreadObj(ExceptionPosition));
}

void NTAPI APCDelegateExecutor(void* arg, void*, void*) {
	using namespace Seed;
	using namespace Seed::Threading;
	Delegate* delegate = reinterpret_cast<Delegate*>(arg);
	try {
		try {
			delegate->Init();
		}
		catch ([[maybe_unused]] InteruptException & ex) {
			auto This = Thread::GetThis();
			toCoreLog(L"APC в потоці з ID " + String::toString(This.GetThisThreadID()) + " і міткою" + This.GetLabel() + "перервано помилкою.");
		}
		catch (Exception & ex) {
			auto This = Thread::GetThis();
			toCoreLog({ L"Роботу потоку з ID " + String::toString(This.GetThisThreadID()) + " і міткою" + This.GetLabel() + "перервано помилкою." ,
					ex.toString()
				});
		}
		delete delegate;
	}
	catch (...) {
		toCoreLog(L"Внутрішня помилка при виконані APC");
	}
}

void Seed::Core::Threading::Thread::DispatchAPC(Delegate && APCDelegate) {
	if (thread->ID != -1) {
		NTSTATUS status =
			NtQueueApcThread(thread->handle, APCDelegateExecutor, new Delegate(MoveRValue(APCDelegate)), NULL, NULL);
		if (NT_SUCCESS(status))
			Throw(NtException(ExceptionPosition, status));
	}
	else
		Throw(InvalidThreadObj(ExceptionPosition));
}

void Seed::Core::Threading::Thread::Kill()
{
	if (this->thread->state == ThreadState::Work || this->thread->state == ThreadState::Init)
		NtTerminateThread(this->thread->handle, STATUS_CANCELLED);
}

#pragma runtime_checks("", off)
#pragma check_stack(off)
__declspec(noreturn, noinline, safebuffers) void InteruptThread() {
	Throw(Seed::InteruptException(ExceptionPosition));
}
#pragma check_stack(on)
#pragma runtime_checks("", restore)

Seed::Core::Threading::Thread& Seed::Core::Threading::Thread::Interupt()
{
	if (this->thread->state == ThreadState::Init)
	{
		this->thread->abortEvent->Wait();
	}
	else
		if (this->thread->state != ThreadState::Work)
		{
			Throw(InvalidThreadObj(ExceptionPosition));
		}

	NtSuspendThread(this->thread->handle, NULL);
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_ALL;
	NTSTATUS status = ZwGetContextThread(this->thread->handle, &ctx);
	if (!NT_SUCCESS(status)) {
		NtResumeThread(this->thread->handle, NULL);
		Throw(NtException(ExceptionPosition, status));
	}
	DWORD64 ret_point = ctx.Rip;
	ctx.Rip = (DWORD64)(PDWORD64)InteruptThread;
	status = ZwSetContextThread(this->thread->handle, &ctx);
	NtResumeThread(this->thread->handle, NULL);
	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
	NtAlertThread(this->thread->handle);
	return *this;
}

Seed::Core::Threading::Thread& Seed::Core::Threading::Thread::Join(SizeT Timeout)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (Timeout > 0) ? -10000LL * Timeout : Math::Max<Int64>();
	if (this->thread->state == ThreadState::Work)
		NtWaitForSingleObject(this->thread->handle, TRUE, &delay);
	return *this;
}

const Seed::Core::Threading::Thread& Seed::Core::Threading::Thread::Join(SizeT Timeout) const
{
	LARGE_INTEGER delay;
	delay.QuadPart = (Timeout > 0) ? -10000LL * Timeout : Math::Max<Int64>();
	if (this->thread->state == ThreadState::Work)
		NtWaitForSingleObject(this->thread->handle, TRUE, &delay);
	return *this;
}

extern void InitThreadEH();
extern void StopThreadEH();

Seed::Boolean Seed::Core::Init::Threading() noexcept {
	toCoreLog(L"Ініціалізація підсистеми багатопотоковості...");
	InitThreadEH();
	return Seed::Core::Threading::InitCoreThreading();
}

void Seed::Core::Stop::Threading() noexcept {
	toCoreLog(L"Зупинка підсистеми багатопотоковості...");
	StopThreadEH();
	Seed::Core::Threading::StopCoreThreading();
}

//Thread pools

struct WorkData {
	Seed::Core::Threading::Job::data* Data;
	Seed::Core::Threading::Job WorkObj;
	Seed::Core::Delegate EntryPoint;
	Seed::Core::Threading::Job::data* JobData;
	Seed::Core::Threading::Thread::GuardLock lock;

	__forceinline WorkData(
		Seed::Core::Threading::Job::data* WorkData,
		Seed::Core::Threading::Job& WorkObj,
		Seed::Core::Delegate& EntryPoint,
		Seed::Core::Threading::Job::data* JobData,
		Seed::Core::Threading::Thread::GuardLock& lock
	) :
		Data(WorkData),
		EntryPoint(EntryPoint),
		WorkObj(WorkObj),
		JobData(JobData),
		lock(lock)
	{}
};

VOID NTAPI CleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
	UNREFERENCED_PARAMETER(CleanupContext);
	WorkData* data = reinterpret_cast<WorkData*>(ObjectContext);
	delete data;
}

VOID NTAPI PoolWorkCallbak(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
) {
	using namespace Seed;
	if (Context == nullptr)
		return;
	WorkData* data = (WorkData*)Context;
	data->Data->IsStarted = true;
	TpCallbackSetEventOnCompletion(Instance, data->Data->event);
	try {
		try {
			data->JobData->TID = Seed::Threading::Thread::GetTID();
			data->lock.Guard([&]() {
				data->EntryPoint.Init();
				});
			data->JobData->TID = 0;
		}
		catch ([[maybe_unused]] InteruptException& ex) {
			toCoreLog(
				ExceptionString(L"Задача в потоці з ID ") + ExceptionString::toString(Threading::Thread::GetTID()) + ExceptionString(L" з пула потоків була перервана"));
		}
		catch (Seed::Exception& ex) {
			Seed::toCoreLog({
				Seed::ExceptionString(
					L"Задача в потоці пула потоків була завершена в наслідок перехоплення не оброленої помилки:"
				) 
				+
				ex.toString()
				});
		}
		delete data;
		TpReleaseWork(Work);
	}
	catch (...) {
		toCoreLog(L"Внутрішня помилка при роботі потоку");
	}
}

void Seed::Core::Threading::Dispatcher::Dispose()
{
	This->lock.WaitFree();
	TpReleaseCleanupGroupMembers(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData), TRUE, NULL);
	TpReleaseCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData));
	TpReleasePool(reinterpret_cast<PTP_POOL>(This->pool));
	delete reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env);
}

Seed::Core::Threading::Dispatcher::Dispatcher() try :
	This(nullptr)
{
	This = new __dispatcher;
	NTSTATUS status = TpAllocPool((PTP_POOL*)& This->pool, NULL);
	if (!NT_SUCCESS(status))
		Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(This->pool), NullPointException::Types::Write));

	TpSetPoolMinThreads(reinterpret_cast<PTP_POOL>(This->pool), (LONG)CurrentSettings().VirtualSetting.CpuCoresCount / 2);
	TpSetPoolMaxThreads(reinterpret_cast<PTP_POOL>(This->pool), (LONG)CurrentSettings().VirtualSetting.CpuCoresCount);
	TP_POOL_STACK_INFORMATION info;
	info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize;
	info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize;
	TpSetPoolStackInformation(reinterpret_cast<PTP_POOL>(This->pool), &info);
	status = TpAllocCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP*>(&This->cleanupData));

	if (!NT_SUCCESS(status)) 
		Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(This->cleanupData), NullPointException::Types::Write));

	This->env = new TP_CALLBACK_ENVIRON;
	auto threadPoolEnviroment = reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env);
	threadPoolEnviroment->Version = 3;
	threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(This->pool);
	threadPoolEnviroment->CleanupGroup = reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData);
	threadPoolEnviroment->CleanupGroupCancelCallback = CleanupCallback;
	threadPoolEnviroment->RaceDll = NULL;
	threadPoolEnviroment->ActivationContext = NULL;
	threadPoolEnviroment->FinalizationCallback = NULL;
	threadPoolEnviroment->u.Flags = 0;
	threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);

}
catch (...) {
#pragma warning(push)
#pragma warning(disable:6001)
	if (this->This != nullptr) {
		if (This->pool != nullptr) {
			if (This->cleanupData != nullptr) {
				if (This->env != nullptr) {
					delete This->env;
				}
				__assume(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData) != nullptr);
				TpReleaseCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData));
			}
			TpReleasePool(reinterpret_cast<PTP_POOL>(This->pool));
		}
		delete This;
		This = nullptr;
	}
#pragma warning(pop)
	ReThrow;
}

Seed::Core::Threading::Job Seed::Core::Threading::Dispatcher::Dispatch(Seed::Core::Delegate&& EntryPoint) 
{
	WorkData* data = nullptr;
	Job WorkObj;
	PTP_WORK work = nullptr;
	HANDLE event = nullptr;

	try {
		NTSTATUS status = NtCreateEvent(&event, GENERIC_ALL, NULL, EVENT_TYPE::NotificationEvent, FALSE);
		if (!NT_SUCCESS(status))
			Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(event), NullPointException::Types::Write));
		
		WorkObj.This = new Job::data(event, MoveRValue(EntryPoint), nullptr);
		data = new WorkData(WorkObj.This, WorkObj, WorkObj.This->delegate, WorkObj.This, This->lock);
		status = TpAllocWork(&work, PoolWorkCallbak, data, reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env));
		
		if (!NT_SUCCESS(status))
			Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(work), NullPointException::Types::Write));
		
		WorkObj.This->work = work;
		TpPostWork(work);
	}
	catch (...) {
#pragma warning(push)
#pragma warning(disable:6001)
		if (event != nullptr) {
			NtClose(event);
			if (WorkObj.This != nullptr)
			{
				delete WorkObj.This;
				WorkObj.This = nullptr;

				if (data != nullptr) {
					delete data;
					data = nullptr;
				}
			}
		}
#pragma warning(pop)
		ReThrow;
	}
	return WorkObj;
}

void Seed::Core::Threading::Job::Dispose() {}

void Seed::Core::Threading::Job::Interupt()
{
	Handle threadHandle;
	CLIENT_ID id;
	id.UniqueProcess = NtCurrentProcessId();
	id.UniqueThread = reinterpret_cast<HANDLE>(this->This->TID);
	if (this->This->TID == 0)
	{
		TpReleaseWork(reinterpret_cast<PTP_WORK>(this->This->work));
		return;
	}
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
	ctx.Rip = (DWORD64)(PDWORD64)InteruptThread;
	status = ZwSetContextThread(threadHandle, &ctx);
	NtResumeThread(threadHandle, NULL);
	NtAlertThread(threadHandle);
	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
	NtClose(threadHandle);
}

#pragma warning(pop)