#include "Seed.Core.Threading.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.CoreLog.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

static Seed::List<Seed::Thread>* Threads;

void Seed::Core::Thread::Event::Dispose()
{
	if (data->handle != nullptr)
		NtClose(data->handle);
}

Seed::Core::Thread::Event::Event(Seed::Boolean IsManualReset) noexcept : data(new event_data(IsManualReset, String()))
{
	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		NULL,
		(IsManualReset) ? EVENT_TYPE::SynchronizationEvent : EVENT_TYPE::NotificationEvent,
		FALSE
	);

	if (NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

Seed::Core::Thread::Event::Event(const Seed::String & name, Seed::Boolean IsManualReset) noexcept : data(new event_data(IsManualReset, name)) {
	OBJECT_ATTRIBUTES attrbs;
	UNICODE_STRING label;
	RtlInitUnicodeStringEx(&label, (PWSTR)(ConstCharStr)data->Name);
	InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		&attrbs,
		(IsManualReset) ? EVENT_TYPE::SynchronizationEvent : EVENT_TYPE::NotificationEvent,
		FALSE
	);

	if (NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

Seed::Core::Thread::Event::Event(Seed::String && name, Seed::Boolean IsManualReset) noexcept : data(new event_data(IsManualReset, name)) {
	OBJECT_ATTRIBUTES attrbs;
	UNICODE_STRING label;
	RtlInitUnicodeStringEx(&label, (PWSTR)(ConstCharStr)data->Name);
	InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

	NTSTATUS status = NtCreateEvent(
		&data->handle,
		GENERIC_ALL,
		&attrbs,
		(IsManualReset) ? EVENT_TYPE::SynchronizationEvent : EVENT_TYPE::NotificationEvent,
		FALSE
	);

	if (NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
}

void Seed::Core::Thread::Event::Wait()
{
	LARGE_INTEGER delay;
	delay.QuadPart = Math::Max<Int64>();
	NtWaitForSingleObject(this->data->handle, TRUE, &delay);
}

void Seed::Core::Thread::Event::Wait(SizeT TimeOutMs)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (TimeOutMs > 0) ? -10000LL * TimeOutMs : Math::Max<Int64>();
	NtWaitForSingleObject(this->data->handle, TRUE, &delay);
}

void Seed::Core::Thread::Dispose()
{
	if (this->thread->EntryPoint != nullptr)
		delete this->thread->EntryPoint;
}

Seed::Core::Thread Seed::Core::Thread::GetThis()
{
	ID TID = GetTID();
	for (auto& thread : *Threads) {
		if (thread.thread->ID == TID)
			return thread;
	}
	Throw(InvalidThread(ExceptionPosition, TID));
}

Seed::Core::Thread Seed::Core::Thread::GetByID(UInt64 TargetTID)
{
	for (auto& thread : *Threads) {
		if (thread.thread->ID == TargetTID)
			return thread;
	}
	Throw(InvalidThread(ExceptionPosition, TargetTID));
}

Seed::ID Seed::Core::Thread::GetTID() {
	return reinterpret_cast<ID>(NtCurrentThreadId());
}

void Seed::Core::Thread::Sleep(UInt64 ms)
{
	LARGE_INTEGER delay;
	delay.QuadPart = (ms > 0) ? -10000LL * ms : Math::Max<Int64>();
	NtDelayExecution(TRUE, &delay);
}

void Seed::Core::Thread::Sleep()
{
	LARGE_INTEGER delay;
	delay.QuadPart = Math::Max<Int64>();
	NtDelayExecution(TRUE, &delay);
}

void Seed::Core::Thread::Wait(UInt64 TargetTID, SizeT Timeout)
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

Seed::Core::List<Seed::Core::Thread> Seed::Core::Thread::GetByLabel(const Seed::Core::String & Label)
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

Seed::Core::List<Seed::Core::Thread> Seed::Core::Thread::GetAllThreads()
{
	return *Threads;
}

bool Seed::Core::InitCoreThreading() noexcept
{
	auto nativeMainThread = Thread(L"Native main thread");
	THREAD_NAME_INFORMATION name;
	RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)nativeMainThread.thread->Label);
	NtSetInformationThread(
		NtCurrentThread(),
		THREADINFOCLASS::ThreadNameInformation,
		&name,
		sizeof(THREAD_NAME_INFORMATION));
	NTSTATUS status =
		NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &nativeMainThread.thread->handle, GENERIC_ALL, 0, 0);
	if (!NT_SUCCESS(status))
		return false;
	nativeMainThread.thread->state = Thread::ThreadState::Work;
	nativeMainThread.thread->ID = Thread::GetTID();
	Threads = new List<Thread>();
	*Threads += nativeMainThread;
	return true;
}

void Seed::Core::StopCoreThreading() noexcept
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

Seed::Int32 Seed::Core::ThreadInitRoute(void* arg) noexcept {
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
			This->thread->state = Seed::Thread::ThreadState::Terminated;
			NtTestAlert();
			This->thread->ID = -1;
			This->thread->thread_list_node.Remove();
			delete This->thread->abortEvent;
			delete This;
			return STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exception & ex) {
			toCoreLog({ L"Роботу потоку з ID " + String::toString(This->thread->ID) + "і міткою" + This->thread->Label + "перервано помилкою." ,
					ex.toString()
				});
			This->thread->state = Seed::Thread::ThreadState::ClosedByExeption;
			NtTestAlert();
			This->thread->ID = -1;
			This->thread->thread_list_node.Remove();
			delete This->thread->abortEvent;
			delete This;
			return STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (Exception & ex) {
		toCoreLog(L"Внутрішня помилка при виконані APC", ex);
		This->thread->state = Seed::Thread::ThreadState::ClosedByExeption;
		NtTestAlert();
		This->thread->ID = -1;
		This->thread->thread_list_node.Remove();
		delete This->thread->abortEvent;
		delete This;
		return STATUS_UNHANDLED_EXCEPTION;
	}
	This->thread->state = Seed::Thread::ThreadState::CorrectlyEnded;
	NtTestAlert();
	This->thread->ID = -1;
	This->thread->thread_list_node.Remove();
	delete This->thread->abortEvent;
	delete This;
	return STATUS_SUCCESS;
}

NTSTATUS __cdecl threadStartRoutineDynEntryPoint(PVOID args) {
	extern void InitThreadEH();
	void StopThreadEH();

	InitThreadEH();
	auto result = Seed::Core::ThreadInitRoute(args);
	StopThreadEH();

	return result;
}

void Seed::Core::Thread::ExecDelegate(Seed::Delegate && ThreadMain) {
	thread->EntryPoint = new Delegate(MoveRValue(ThreadMain));
	if (this->thread->thread_list_node != nullptr)
		Throw(ThreadAlredyStrated(ExceptionPosition));
	toCoreLog(String(L"Запуск потоку з міткою ") + this->thread->Label + " з потоку з ID " + String::toString(Thread::GetTID()));
	this->thread->thread_list_node = Threads->AddLast(*this);
	this->thread->abortEvent = new Event();
	CLIENT_ID id;
	size_t stack_max = 10485760, stack_alloc = 524288;
	RtlCreateUserThread(
		NtCurrentProcess(),
		NULL,
		false,
		0,
		Seed::Core::CurrentSettings::Threading::Thread::GetStackMaxSize(),
		Seed::Core::CurrentSettings::Threading::Thread::GetStackStartSize(),
		threadStartRoutineDynEntryPoint,
		(void*)new Thread(*this),
		&thread->handle,
		&id);
	this->thread->state = Seed::Thread::ThreadState::Work;
	this->thread->ID = reinterpret_cast<SizeT>(id.UniqueThread);
}

void Seed::Core::Thread::ChangeLabel(const String & NewLabel) {
	if (thread->ID != -1) {
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
	else
		Throw(InvalidThreadObj(ExceptionPosition));
}

void Seed::Core::Thread::ExecApc() {
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
	}
	catch (Exception & ex) {
		toCoreLog(L"Внутрішня помилка при виконані APC", ex);
	}
	delete delegate;
}

void Seed::Core::Thread::DispatchAPC(Delegate && APCDelegate) {
	if (thread->ID != -1) {
		NTSTATUS status =
			NtQueueApcThread(thread->handle, APCDelegateExecutor, new Delegate(MoveRValue(APCDelegate)), NULL, NULL);
		if (NT_SUCCESS(status))
			Throw(NtException(ExceptionPosition, status));
	}
	else
		Throw(InvalidThreadObj(ExceptionPosition));
}

void Seed::Core::Thread::Kill()
{
	if (this->thread->state == ThreadState::Work || this->thread->state == ThreadState::Init)
		NtTerminateThread(this->thread->handle, STATUS_CANCELLED);
}

#pragma runtime_checks("", off)
#pragma check_stack(off)
__declspec(noreturn, noinline, safebuffers) void InterupThread() {
	Throw(Seed::InteruptException(ExceptionPosition));
}

__declspec(noreturn, noinline, safebuffers) void DontOptimize() {
	volatile int i = 0;
	if (i) InterupThread();
}
#pragma check_stack(on)
#pragma runtime_checks("", restore)

Seed::Core::Thread& Seed::Core::Thread::Interupt()
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
	ctx.Rip = (DWORD64)(PDWORD64)InterupThread;
	status = ZwSetContextThread(this->thread->handle, &ctx);
	NtResumeThread(this->thread->handle, NULL);
	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));
	NtAlertThread(this->thread->handle);
	return *this;
}
void Seed::Core::Thread::Join(SizeT Timeout) const
{
	LARGE_INTEGER delay;
	delay.QuadPart = (Timeout > 0) ? -10000LL * Timeout : Math::Max<Int64>();
	if (this->thread->state == ThreadState::Work)
		NtWaitForSingleObject(this->thread->handle, TRUE, &delay);
}

extern void InitThreadEH();
extern void StopThreadEH();

Seed::Boolean Seed::Core::Init::Threading() noexcept {
	toCoreLog(L"Ініціалізація підсистеми багатопотоковості...");
	InitThreadEH();
	return Seed::Core::InitCoreThreading();
}

void Seed::Core::Stop::Threading() noexcept {
	toCoreLog(L"Зупинка підсистеми багатопотоковості...");
	StopThreadEH();
	Seed::Core::StopCoreThreading();
}

#pragma warning(pop)