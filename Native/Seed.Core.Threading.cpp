// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Threading.Thread.h"

#include "Seed.Core.Threading.UnifiedThreadPool.h"
#include "Seed.Core.Threading.Threadables.h"
#include "Seed.Core.Threading.SystemThreadPool.h"

#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.Manage.h"

#include "Seed.Core.Threading.TLS.GlobalTLSBlock.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 26812)

using namespace Seed;
using namespace Seed::Threading;

#pragma region Generic API

Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>* AllActiveThreadable;

ForceInline IThreadable* GetThisThreadable() {
	return Seed::Core::Threading::GetTLS()->Threadable;
}

ForceInline volatile EThreadabledStates* GetThisThreadableState() {
	return Seed::Core::Threading::GetTLS()->State;
}

SizeT Seed::Core::Threading::GetThreadableCount(Boolean WithPseudoThreadable)
{
	if (!WithPseudoThreadable)
		return AllActiveThreadable->GetCountIf([](const SmartPtr<IThreadable>& item) -> bool InlineLambda {
			return item->GetThreadableType() == EThreadableTypes::Thread;
		});
	else
		return AllActiveThreadable->Count;
}

Containers::ConcurencyList<SmartPtr<IThreadable>> Seed::Core::Threading::GetAllThreadable()
{
	return AllActiveThreadable->CopyTo<Containers::ConcurencyList>();
}

IThreadable& Seed::Core::Threading::GetThis()
{
	return *GetTLS()->Threadable;
}

ID Seed::Core::Threading::GetTID()
{
	return reinterpret_cast<ID>(NtCurrentThreadId());
}

void Seed::Core::Threading::Sleep(Time::DateTime time, Boolean IsRelative) //-V813
{
	LARGE_INTEGER delay;
	delay.QuadPart = (IsRelative) ? time.RelativeTime : time.TimeFromStartEpoch;
	NtDelayExecution(TRUE, &delay);
}

void SetName(const String& NewName, Handle hThread) {
	THREAD_NAME_INFORMATION name;
	RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)const_cast<String&>(NewName));
	NtSetInformationThread(
		hThread,
		THREADINFOCLASS::ThreadNameInformation,
		&name,
		sizeof(THREAD_NAME_INFORMATION)
	);
}

void ReSetName(Handle hThread) {
	THREAD_NAME_INFORMATION name;
	RtlInitUnicodeString(&name.ThreadName, (PWSTR)(ConstCharStr)L"");
	NtSetInformationThread(
		hThread,
		THREADINFOCLASS::ThreadNameInformation,
		&name,
		sizeof(THREAD_NAME_INFORMATION)
	);
}

#pragma endregion

#pragma region Internal API

namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				extern ThreadableRef RegisterThreadThreadable(const Thread& thread, volatile EThreadabledStates* State);
				extern void UnRegisterThreadThreadable(ThreadableRef&& ref);

				extern ThreadableRef RegisterPoolIoThreadable(const UnifiedThreadPoolIoTask& task, const SynchronizeEvent& event, ImplSpecificT instance);
				extern void UnRegisterPoolIoThreadable(ThreadableRef&& ref);

				extern ThreadableRef RegisterPoolSheduledThreadable(const UnifiedThreadPoolSheduledTask& task, ImplSpecificT instance);
				extern void UnRegisterPoolSheduledThreadable(ThreadableRef&& ref);

				extern ThreadableRef RegisterPoolJobThreadable(const UnifiedThreadPoolJob& task, ImplSpecificT instance);
				extern void UnRegisterPoolJobThreadable(ThreadableRef&& ref);
			}
		}
	}
}

template<typename EntryPointT>
static NTSTATUS EntryPoint(EntryPointT&& entryPoint, Thread* thread, volatile EThreadabledStates* State) noexcept {
	using namespace Seed;

	NTSTATUS status = STATUS_SUCCESS;

	Threadables::ThreadableRef ref;

	try {
		try {
			ref = Threadables::RegisterThreadThreadable(*thread, State);
			entryPoint();
		}
		catch (UnusedData Exceptions::InteruptException & er) {
			toCoreLog(L"Потік з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()));
			status = STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exceptions::Exception & ex) {
			toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" виникла помилка " + ex.toString());
			status = STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (...) {
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}

	try
	{
		Threadables::UnRegisterThreadThreadable(MoveRValue(ref));
		delete thread;
	}
	catch (Exception& ex)
	{
		toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" при виключені, виникла помилка " + ex.toString());
		status = STATUS_UNHANDLED_EXCEPTION;
	}
	catch (...)
	{
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}
	return status;
}

#pragma runtime_checks("", off)
#pragma check_stack(off)
extern __declspec(noreturn, noinline, safebuffers) void InteruptThread();
#pragma check_stack(on)
#pragma runtime_checks("", restore)

#pragma endregion
#pragma region Thread

namespace Seed {
	inline namespace Core {
		namespace Threading {
			volatile Delegate& ThreadEntryPoint(Thread* thread) {
				return thread->This->delegate;
			}

			volatile EThreadabledStates* GetActiveState(Thread* thread)
			{
				return &thread->This->State;
			}
		}
	}
}

NTSTATUS __cdecl threadStartRoutineDynEntryPoint(PVOID args) noexcept {
	Thread* This = (Thread*)args;
	return EntryPoint(Seed::Core::Threading::ThreadEntryPoint(This), This, GetActiveState(This));
}
namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{

			bool MainThread(Delegate&& delegate) {
				//Create static list of all active threadable objects
				AllActiveThreadable = new Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>();
				//duplicate main thread handle
				HANDLE threadHandle = nullptr;
				NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &threadHandle, GENERIC_ALL, 0, 0);
				//create main thread object
				Thread* MainThread = new Thread("Main thread", MoveRValue(delegate));
				//fill aditional data as this thread alredy started
				MainThread->This->State = EThreadabledStates::Starting;
				MainThread->This->hThread = threadHandle;
				MainThread->This->AbortEvent = new SynchronizeEvent();
				MainThread->This->TID = reinterpret_cast<SizeT>(NtCurrentThreadId());
				//update main thread name
				MainThread->RefreshName();
				//execute managed entry point
				EntryPoint(MainThread->This->delegate, MainThread, &MainThread->This->State);
				//Interupt and wait for interupting each exist theradable
				AllActiveThreadable->ForEach([] (SmartPtr<IThreadable>& thread) InlineLambda
				{
					toCoreLog(L"Помилка: Потік з ID: " + String::toString(thread->GetID()) + L" та з міткою: \"" + thread->GetName() + L"\" примусово зупиняється внаслідок завершення ядра");
					thread->Interupt()
						.Join(CurrentSettings().Threading.Thread.StopKernelThreadsTimeout.Ticks);
				});
				//Kill each exist thread
				AllActiveThreadable->ForEach([] (SmartPtr<IThreadable>& thread) InlineLambda
				{
					if (thread->GetThreadableType() == EThreadableTypes::Thread)
					{
						if (thread->GetThreadabledState() != EThreadabledStates::Ended &&
							thread->GetThreadabledState() != EThreadabledStates::Interupted)
						{
							auto& threadObj = dynamic_cast<Thread&>(*thread);
							KillThread(threadObj, STATUS_INTERNAL_ERROR);
						}
					}
				});
				//Removing all threadable objects
				AllActiveThreadable->RemoveAll([] (SmartPtr<IThreadable>&& thread) InlineLambda
				{
					thread->Join();
				});
				delete AllActiveThreadable;
				return true;
			}

			void KillThread(Thread& target, UInt32 statusCode) {
				NtTerminateThread(target.This->hThread, statusCode);
				target.This->State = EThreadabledStates::Interupted;
			}

			void Thread::RefreshName() {
				THREAD_NAME_INFORMATION name;
				RtlInitUnicodeString(&name.ThreadName, (PWSTR) (ConstCharStr)const_cast<String&>(This->name));
				NtSetInformationThread(
					This->hThread,
					THREADINFOCLASS::ThreadNameInformation,
					&name,
					sizeof(THREAD_NAME_INFORMATION)
				);
			}

			void Thread::Dispose() {
				delete This->AbortEvent;
				NtClose(This->hThread);
			}

			Thread& Thread::Start() {
				if (this->This->This != nullptr)
					Throw(ThreadableAlredyStarted(ExceptionPosition));

				this->This->AbortEvent = new SynchronizeEvent();
				CLIENT_ID id;
				size_t stack_max = 10485760, stack_alloc = 524288;
				RtlCreateUserThread(
					NtCurrentProcess(),
					NULL,
					false,
					0,
					Seed::CurrentSettings().Threading.Thread.StackMaxSize.Bytes,
					Seed::CurrentSettings().Threading.Thread.StackStartSize.Bytes,
					threadStartRoutineDynEntryPoint,
					(void*)new Thread(*this), //-V572
					&This->hThread,
					&id);
				this->This->State = EThreadabledStates::Starting;
				this->This->TID = reinterpret_cast<SizeT>(id.UniqueThread);

				if (CurrentSettings().Threading.Thread.ThreadCreatingLogging) {
					toCoreLog(
						String::Combine({ L"Запуск потоку з ім'ям \"", 
							this->GetName(), 
							L"\" з ID ", 
							String::toString(this->This->TID), 
							L" з потоку з ID ", 
							String::toString(Threading::GetTID()) }));
				}

				return *this;
			}

			Thread& Thread::Join()
			{
				_ReadWriteBarrier();

				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = Math::Max<Int64>();
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			Thread& Thread::Join(Time::TimeSpan timeOut)
			{
				_ReadWriteBarrier();

				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = timeOut.Ticks;
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			const Thread& Thread::Join() const
			{
				_ReadWriteBarrier();

				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = Math::Max<Int64>();
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			const Thread& Thread::Join(Time::TimeSpan timeOut) const
			{
				_ReadWriteBarrier();

				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = timeOut.Ticks;
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			Thread& Thread::Interupt() {
				_ReadWriteBarrier();

				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting:
					This->AbortEvent->Wait(); //-V796
				default:
					switch (This->State)
					{
					case EThreadabledStates::Executing:
					case EThreadabledStates::ExceptionProcessing:
					case EThreadabledStates::Paused:
					{
						NtSuspendThread(This->hThread, NULL);
						CONTEXT ctx;
						ctx.ContextFlags = CONTEXT_ALL;
						NTSTATUS status = ZwGetContextThread(This->hThread, &ctx);
						if (!NT_SUCCESS(status))
						{
							NtResumeThread(This->hThread, NULL);
							return *this;
						}
						DWORD64 ret_point = ctx.Rip;
						ctx.Rip = (DWORD64) (PDWORD64) InteruptThread;
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

			Thread& Thread::Suspend() {
				_ReadWriteBarrier();

				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting:
					This->AbortEvent->Wait(); //-V796
				case EThreadabledStates::Executing:
				case EThreadabledStates::ExceptionProcessing:
				{
					This->SecondState = EThreadabledStates::Paused;
					Swap(This->State, This->SecondState);
					NtSuspendThread(This->hThread, NULL);
					break;
				}
				case EThreadabledStates::Paused: break;
				default:
					break;
				}
				return *this;
			}

			Thread& Thread::Resume() {
				_ReadWriteBarrier();

				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting: break;
				case EThreadabledStates::Executing: break;
				case EThreadabledStates::ExceptionProcessing: break;
				case EThreadabledStates::Paused:
				{
					Swap(This->State, This->SecondState);
					NtAlertResumeThread(This->hThread, NULL);
					break;
				}
				default:
					break;
				}
				return *this;
			}
		}
	}
}
#pragma endregion

#pragma region SynchronizeEvent
namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{
			void SynchronizeEvent::Dispose() {
				if (data->handle != nullptr)
					NtClose(data->handle);
			}

			SynchronizeEvent::SynchronizeEvent(Boolean IsManualReset) : data(new event_data(IsManualReset, String())) {
				NTSTATUS status = NtCreateEvent(
					&data->handle,
					GENERIC_ALL,
					NULL,
					(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
					FALSE
				);

				if (!NT_SUCCESS(status))
				{
					delete data;
					data = nullptr;
					Throw(NtException(ExceptionPosition, status));
				}
			}

			SynchronizeEvent::SynchronizeEvent(const String& name, Boolean IsManualReset) : data(new event_data(IsManualReset, name)) {
				OBJECT_ATTRIBUTES attrbs;
				UNICODE_STRING label;
				RtlInitUnicodeStringEx(&label, (PWSTR) (ConstCharStr) data->Name);
				InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

				NTSTATUS status = NtCreateEvent(
					&data->handle,
					GENERIC_ALL,
					&attrbs,
					(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
					FALSE
				);

				if (!NT_SUCCESS(status))
				{
					delete data;
					data = nullptr;
					Throw(NtException(ExceptionPosition, status));
				}
			}

			SynchronizeEvent::SynchronizeEvent(String&& name, Boolean IsManualReset) : data(new event_data(IsManualReset, MoveRValue(name))) {
				OBJECT_ATTRIBUTES attrbs;
				UNICODE_STRING label;
				RtlInitUnicodeStringEx(&label, (PWSTR) (ConstCharStr) data->Name);
				InitializeObjectAttributes(&attrbs, &label, NULL, NULL, NULL);

				NTSTATUS status = NtCreateEvent(
					&data->handle,
					GENERIC_ALL,
					&attrbs,
					(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
					FALSE
				);

				if (!NT_SUCCESS(status))
				{
					delete data;
					data = nullptr;
					Throw(NtException(ExceptionPosition, status));
				}
			}

			void SynchronizeEvent::Wait() const {
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = Math::Max<Int64>();
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			void SynchronizeEvent::Wait(SizeT TimeOutMs) const {
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = (TimeOutMs > 0) ? -10000LL * TimeOutMs : Math::Max<Int64>();
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			void SynchronizeEvent::Wait(Time::TimeSpan timeOunt) const
			{
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = timeOunt.Ticks;
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			UInt64 WaitForMany(const Containers::Array<SynchronizeEvent>& events, Boolean isWaitAll, SizeT TimeOutMs) {
				if (events.Count > static_cast<SizeT>(Math::Max<UInt32>()))
					Throw(InvalidArgumentException(ExceptionPosition, L"events.Count", L"events count must be less than ULONG max"));

				HANDLE* hEvents = nullptr;

				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = (TimeOutMs > 0 && TimeOutMs != Math::Max<SizeT>()) ? -10000LL * TimeOutMs : Math::Max<Int64>();

				try
				{
					if (events.Count == 0)
						return 0;
					if (events.Count == 1)
					{
						events.First([] (const SynchronizeEvent&)
						{
							return true;
						}).Wait();
						return 1;
					}
					hEvents = new HANDLE[events.Count];
					events.ForEach([&] (const SynchronizeEvent& event, SizeT Index)
					{
						hEvents[Index] = event.data->handle;
					});

					NTSTATUS status = NtWaitForMultipleObjects(
						static_cast<ULONG>(events.Count),
						hEvents,
						(isWaitAll) ? WaitAll : WaitAny,
						TRUE,
						&delay
					);

					delete[] hEvents;

					return status - STATUS_WAIT_0;
				}
				catch (...)
				{
					delete[] hEvents;
					ReThrow;
				}
			}
		}
	}
}
#pragma endregion

#pragma region Unified thread pool

#pragma region Implementation specific

void Seed::Core::Threading::UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj)
{
	((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->JobsQueue.Remove(jobObj->This->poolQueueRef);
}

void Seed::Core::Threading::UnRegisterTaskFromPoolQueue(UnifiedThreadPoolSheduledTask* thread)
{
	if (thread->This->poolQueueRef != nullptr) {
		((UnifiedThreadPool::Data*)thread->This->ownerPool)->TaskQueue.Remove(thread->This->poolQueueRef);
	}
}

void Seed::Core::Threading::ZeroingTaskHadle(UnifiedThreadPoolSheduledTask* thread)
{
	thread->This->hTask = nullptr;
}

void Seed::Core::Threading::DestroyIoPoolObj(UnifiedThreadPoolIoTask* task)
{
	TpReleaseIoCompletion((PTP_IO)task->This->hIoPoolObj);
	task->This->hIoPoolObj = nullptr;
}

VOID NTAPI UnifiedThreadPoolJobsCleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
	ConfigureTls(nullptr, nullptr);
	try {
	auto jobObj = (UnifiedThreadPoolJob*)ObjectContext;
	UnregisterJobFromJobList(jobObj);
	delete jobObj;
	}
	catch (...) {
		toCoreLog(L"Невідома помилка при знищенні об'єкта задачі в пулі");
	}
	DestroyTls();
}

VOID NTAPI UnifiedThreadPoolTasksCleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
	ConfigureTls(nullptr, nullptr);
	try {
		if (ObjectContext == NULL)
			return;
		Seed::Core::Threading::UnifiedThreadPoolSheduledTask* This = (Seed::Core::Threading::UnifiedThreadPoolSheduledTask*)ObjectContext;
		Seed::Core::Threading::ZeroingTaskHadle(This);
		UnRegisterTaskFromPoolQueue(This);
		delete This;
	}
	catch (...) {
		toCoreLog(L"Невідома помилка при знищенні об'єкта задачі в пулі");
	}
	DestroyTls();
}

VOID NTAPI UnifiedThreadPoolIrpCleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
}

VOID NTAPI IoFinalizer(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context
	) {
}

#pragma endregion

#pragma region Pool Creating
UnifiedThreadPool Seed::Core::Threading::UnifiedThreadPoolBuilder::CreateThreadPool() &&
{
	if (This->maxThreadCount != 0 && This->minThreadCount > This->maxThreadCount)
		Throw(InvalidArgumentException(ExceptionPosition, L"MinThreadCount", L"Мінімальна кількість потоків не може бути менщою ніж максимальна"));

	PTP_POOL hPool = nullptr;
	//Create thread pool object
	NTSTATUS status = TpAllocPool(&hPool, NULL);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	//Set min\max threads count
	TpSetPoolMinThreads(hPool, static_cast<LONG>(This->minThreadCount));
	TpSetPoolMaxThreads(hPool, static_cast<LONG>(This->maxThreadCount));

	//Set min\max stack size
	TP_POOL_STACK_INFORMATION info = { 0 };

	info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize.Bytes;
	info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize.Bytes;

	TpSetPoolStackInformation(hPool, &info);

	//Creating configuration

	//Job configuration
	UnifiedThreadPool::PoolConfigureation jobsConfiguration = { 0 };
	try {
		jobsConfiguration = CreateJobConfiguration(hPool);
	}
	catch (...) {
		UnifiedThreadPool::DestroyPoolObject(hPool);
		ReThrow;
	}

	//Sheduled task configuration
	UnifiedThreadPool::PoolConfigureation tasksConfiguration = { 0 };
	try {
		tasksConfiguration = CreateSheduledTaskConfiguration(hPool);
	}
	catch (...) {
		UnifiedThreadPool::DestroyConfiguration(jobsConfiguration);
		UnifiedThreadPool::DestroyPoolObject(hPool);
		ReThrow;
	}

	//Irp configuration
	UnifiedThreadPool::PoolConfigureation irpConfiguration = { 0 };
	try {
		irpConfiguration = CreateIrpConfiguration(hPool);
	}
	catch (...) {
		UnifiedThreadPool::DestroyConfiguration(jobsConfiguration);
		UnifiedThreadPool::DestroyConfiguration(tasksConfiguration);
		UnifiedThreadPool::DestroyPoolObject(hPool);
		ReThrow;
	}

	//Create impl object

	UnifiedThreadPool::ImplementationSpecificData implData = { 0 };
	implData.PoolHandle = hPool;
	implData.JobConfiguration = jobsConfiguration;
	implData.TaskConfiguration = tasksConfiguration;
	implData.IrpConfiguration = irpConfiguration;

	//Create pool data struct
	UnifiedThreadPool::Data* poolData = new UnifiedThreadPool::Data(This->minThreadCount, This->maxThreadCount, MoveRValue(This->threadPoolName), MoveRValue(implData));

	//Delete this object data as it invalidated
	delete This;
	This = nullptr;

	//Return pool object
	return UnifiedThreadPool(poolData);
}
#pragma endregion

#pragma region Pool configuration creating

UnifiedThreadPool::PoolConfigureation Seed::Core::Threading::UnifiedThreadPoolBuilder::CreateJobConfiguration(ImplSpecificT PoolHandle)
{
	PTP_CLEANUP_GROUP clenupData = nullptr;

	NTSTATUS status = TpAllocCleanupGroup(&clenupData);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	auto threadPoolEnviroment = new TP_CALLBACK_ENVIRON;

	threadPoolEnviroment->Version = 3;
	threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(PoolHandle);
	threadPoolEnviroment->CleanupGroup = clenupData;
	threadPoolEnviroment->CleanupGroupCancelCallback = UnifiedThreadPoolJobsCleanupCallback;
	threadPoolEnviroment->RaceDll = NULL;
	threadPoolEnviroment->FinalizationCallback = NULL;
	threadPoolEnviroment->u.Flags = 0;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);
	(SizeT&)threadPoolEnviroment->ActivationContext = -1;

	switch (This->jobsPriority)
	{
	case UnifiedThreadPool::EPoolThreadsPriority::Low:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_LOW;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::Medium:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::High:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_HIGH;
		break;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"JobsPriority", L"Invalid priority for jobs"));
		break;
	}

	UnifiedThreadPool::PoolConfigureation configuration = { 0 };

	configuration.CallbackEnviroment = threadPoolEnviroment;
	configuration.ClenupGroup = clenupData;
	configuration.Priority = This->jobsPriority;

	return configuration;
}

UnifiedThreadPool::PoolConfigureation Seed::Core::Threading::UnifiedThreadPoolBuilder::CreateSheduledTaskConfiguration(ImplSpecificT PoolHandle)
{
	PTP_CLEANUP_GROUP clenupData = nullptr;

	NTSTATUS status = TpAllocCleanupGroup(&clenupData);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	auto threadPoolEnviroment = new TP_CALLBACK_ENVIRON;

	threadPoolEnviroment->Version = 3;
	threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(PoolHandle);
	threadPoolEnviroment->CleanupGroup = clenupData;
	threadPoolEnviroment->CleanupGroupCancelCallback = UnifiedThreadPoolTasksCleanupCallback;
	threadPoolEnviroment->RaceDll = NULL;
	threadPoolEnviroment->FinalizationCallback = NULL;
	threadPoolEnviroment->u.Flags = 0;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);
	(SizeT&)threadPoolEnviroment->ActivationContext = -1;

	switch (This->sheduledTasksPriority)
	{
	case UnifiedThreadPool::EPoolThreadsPriority::Low:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_LOW;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::Medium:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::High:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_HIGH;
		break;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"JobsPriority", L"Invalid priority for jobs"));
		break;
	}

	UnifiedThreadPool::PoolConfigureation configuration = { 0 };

	configuration.CallbackEnviroment = threadPoolEnviroment;
	configuration.ClenupGroup = clenupData;
	configuration.Priority = This->sheduledTasksPriority;

	return configuration;
}

UnifiedThreadPool::PoolConfigureation Seed::Core::Threading::UnifiedThreadPoolBuilder::CreateIrpConfiguration(ImplSpecificT PoolHandle)
{
	PTP_CLEANUP_GROUP clenupData = nullptr;

	NTSTATUS status = TpAllocCleanupGroup(&clenupData);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	auto threadPoolEnviroment = new TP_CALLBACK_ENVIRON;

	threadPoolEnviroment->Version = 3;
	threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(PoolHandle);
	threadPoolEnviroment->CleanupGroup = clenupData;
	threadPoolEnviroment->CleanupGroupCancelCallback = UnifiedThreadPoolIrpCleanupCallback;
	threadPoolEnviroment->RaceDll = NULL;
	threadPoolEnviroment->ActivationContext = NULL;
	threadPoolEnviroment->FinalizationCallback = NULL;
	threadPoolEnviroment->u.Flags = 0;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);
	(SizeT&)threadPoolEnviroment->ActivationContext = -1;

	switch (This->irpRequestPriority)
	{
	case UnifiedThreadPool::EPoolThreadsPriority::Low:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_LOW;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::Medium:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
		break;
	case UnifiedThreadPool::EPoolThreadsPriority::High:
		threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_HIGH;
		break;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"JobsPriority", L"Invalid priority for jobs"));
		break;
	}

	UnifiedThreadPool::PoolConfigureation configuration = { 0 };

	configuration.CallbackEnviroment = threadPoolEnviroment;
	configuration.ClenupGroup = clenupData;
	configuration.Priority = This->irpRequestPriority;

	return configuration;
}

void Seed::Core::Threading::UnifiedThreadPool::DestroyConfiguration(PoolConfigureation& poolConfiguration)
{
	poolConfiguration.PoolLock.ExclusiveLock([&]() {
		TpReleaseCleanupGroupMembers(reinterpret_cast<PTP_CLEANUP_GROUP>(poolConfiguration.ClenupGroup), TRUE, NULL);
		TpReleaseCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP>(poolConfiguration.ClenupGroup));
		delete poolConfiguration.CallbackEnviroment;
	});
}

void Seed::Core::Threading::UnifiedThreadPool::DestroyPoolObject(ImplSpecificT PoolHandle)
{
	TpReleasePool(reinterpret_cast<PTP_POOL>(PoolHandle));
}
#pragma endregion

#pragma region Unified thread pool job

namespace Seed {
	inline namespace Core {
		namespace Threading {
			Threadables::ThreadableRef RegisterThreadable(UnifiedThreadPoolJob* thread, ImplSpecificT callbackInstance) {
				auto ref = Threadables::RegisterPoolJobThreadable(*thread, callbackInstance);
				SetName(thread->This->jobName, NtCurrentThread());
				_ReadWriteBarrier();
				return ref;
			}

			void UnRegisterThreadable(UnifiedThreadPoolJob* thread, Threadables::ThreadableRef&& ref) {
				Threadables::UnRegisterPoolJobThreadable(MoveRValue(ref));
				UnregisterJobFromJobList(thread);
				ReSetName(NtCurrentThread());
				NtSetEvent(thread->This->hEvent, NULL);
				_ReadWriteBarrier();
				DestroyTls();
			}
		}
	}
}

template<typename EntryPointT>
static NTSTATUS EntryPoint(EntryPointT&& entryPoint, UnifiedThreadPoolJob* thread, ImplSpecificT instance) noexcept {
	using namespace Seed;

	NTSTATUS status = STATUS_SUCCESS;

	Threadables::ThreadableRef ref;

	try {
		try {
			ref = Threading::RegisterThreadable(thread, instance);
			entryPoint();
		}
		catch (UnusedData Exceptions::InteruptException& er) {
			toCoreLog(L"Потік з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()));
			status = STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exceptions::Exception& ex) {
			toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" виникла помилка " + ex.toString());
			status = STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (...) {
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}

	try
	{
		Threading::UnRegisterThreadable(thread, MoveRValue(ref));
		delete thread;
	}
	catch (Exception& ex)
	{
		toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" при виключені, виникла помилка " + ex.toString());
		status = STATUS_UNHANDLED_EXCEPTION;
	}
	catch (...)
	{
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}
	return status;
}

bool Seed::Core::Threading::IsUnifiedThreadPoolTerminate(UnifiedThreadPoolJob* jobObj)
{
	return ((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->isPoolTermination;
}

namespace Seed {
	inline namespace Core {
		namespace Threading {
			Seed::Core::Threading::SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj)
			{
				return ((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->handlesData.JobConfiguration.PoolLock;
			}

			Delegate& GetJobEntryPoint(UnifiedThreadPoolJob* jobObj)
			{
				return jobObj->This->action;
			}

			UnifiedThreadPool UnifiedThreadPoolJob::GetOwnerPool() const
			{
				auto poolObj = (UnifiedThreadPool::Data*)(UnifiedThreadPool::Data*)This->pOwnerPool;
				return UnifiedThreadPool((UnifiedThreadPool::Data*&)poolObj);
			}
		}
	}
}

VOID NTAPI UnifiedPoolWorkCallbak(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
) {
	using namespace Seed;
	using namespace Seed::Core::Threading;

	if (Context == nullptr)
		return;

	UnifiedThreadPoolJob* thread = (UnifiedThreadPoolJob*)Context;

	if (!IsUnifiedThreadPoolTerminate(thread)) {
		SRWLock& lock = GetUnifiedPoolLock(thread);
		lock.TryInShareLock([&]() { EntryPoint(GetJobEntryPoint(thread), thread, Instance); });
	}

	//Free work as we use each work object only one time
	//(if we don't free each work also will be free in cleanup group close callback and we catch double free exception)
	TpReleaseWork(Work); 
}

UnifiedThreadPoolJob Seed::Core::Threading::UnifiedThreadPool::PostJob(String&& jobName, Delegate&& action)
{
	PTP_WORK work = nullptr;
	HANDLE event = nullptr;
	UnifiedThreadPoolJob* jobObj = nullptr;

	try
	{
		NTSTATUS status = NtCreateEvent(&event, GENERIC_ALL, NULL, EVENT_TYPE::NotificationEvent, FALSE);
		if (!NT_SUCCESS(status))
			Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(event), NullPointException::Types::Write));

		jobObj = new UnifiedThreadPoolJob(new UnifiedThreadPoolJob::Data(This, MoveRValue(jobName), MoveRValue(action), event));

		status = TpAllocWork(&work, UnifiedPoolWorkCallbak, jobObj, reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->handlesData.JobConfiguration.CallbackEnviroment)); //-V2571

		if (!NT_SUCCESS(status))
			Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(work), NullPointException::Types::Write));

		auto jobObjCp = UnifiedThreadPoolJob(*jobObj);

		jobObj->This->hWork = work;
		jobObj->This->poolQueueRef = This->JobsQueue.AddLast(jobObjCp);

		TpPostWork(work);

		return jobObjCp;
	}
	catch (...)
	{
		if (jobObj != nullptr) {
			if (jobObj->This->poolQueueRef != nullptr) {
				jobObj->This->poolQueueRef.Remove();
			}

			delete jobObj;
		}

		if (event != nullptr)
		{
			NtClose(event);
		}
		ReThrow;
	}
}

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join()
{
	NtWaitForSingleObject(This->hEvent, TRUE, NULL);
	return *this;
}

const UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join() const
{
	NtWaitForSingleObject(This->hEvent, TRUE, NULL);
	return *this;
}

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join(Time::TimeSpan timeOut)
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.Ticks;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

const UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join(Time::TimeSpan timeOut) const
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.Ticks;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

void Seed::Core::Threading::UnifiedThreadPoolJob::Dispose()
{
	NtClose(This->hEvent);
	This->hEvent = nullptr;
}
#pragma endregion

#pragma region Unified thread pool sheduler task

namespace Seed {
	inline namespace Core {
		namespace Threading {
			Threadables::ThreadableRef RegisterThreadable(UnifiedThreadPoolSheduledTask* thread, ImplSpecificT callbackInstance) {
				thread->This->lastStartTime = Time::DateTime::GetCurrentSystemTimeData();
				thread->This->activeInstanceRefCount++;

				auto ref = Threadables::RegisterPoolSheduledThreadable(*thread, callbackInstance);
				SetName(thread->This->taskName, NtCurrentThread());
				_ReadWriteBarrier();
				return ref;
			}

			void UnRegisterThreadable(UnifiedThreadPoolSheduledTask* thread, Threadables::ThreadableRef&& ref) {
				Threadables::UnRegisterPoolSheduledThreadable(MoveRValue(ref));

				--thread->This->activeInstanceRefCount;
				++thread->This->executedCount;

				if (thread->This->remainingCount != 0) {
					--thread->This->remainingCount;
				}

				if (thread->This->remainingCount == 0 && thread->This->intervalsCount != 0) {
					TpReleaseTimer(reinterpret_cast<PTP_TIMER>(thread->This->hTask));
					UnRegisterTaskFromPoolQueue(thread);
					delete thread;
				}

				ReSetName(NtCurrentThread());

				_ReadWriteBarrier();
				DestroyTls();
			}
		}
	}
}


template<typename EntryPointT>
static NTSTATUS EntryPoint(EntryPointT&& entryPoint, UnifiedThreadPoolSheduledTask* thread, ImplSpecificT instance) noexcept {
	using namespace Seed;

	NTSTATUS status = STATUS_SUCCESS;

	Threadables::ThreadableRef ref;

	try {
		try {
			ref = RegisterThreadable(thread, instance);
			entryPoint();
		}
		catch (UnusedData Exceptions::InteruptException& er) {
			toCoreLog(L"Потік з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()));
			status = STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exceptions::Exception& ex) {
			toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" виникла помилка " + ex.toString());
			status = STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (...) {
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}

	try
	{
		UnRegisterThreadable(thread, MoveRValue(ref));
	}
	catch (Exception& ex)
	{
		toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" при виключені, виникла помилка " + ex.toString());
		status = STATUS_UNHANDLED_EXCEPTION;
	}
	catch (...)
	{
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}
	return status;
}

namespace Seed {
	inline namespace Core {
		namespace Threading {
			bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolSheduledTask* task)
			{
				return ((UnifiedThreadPool::Data*)(task->This->ownerPool))->isPoolTermination;
			}

			SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolSheduledTask* task)
			{
				return ((UnifiedThreadPool::Data*)(task->This->ownerPool))->handlesData.TaskConfiguration.PoolLock;
			}

			SmartPtr<IUnifiedThreadPoolShedulerCallback>& Seed::Core::Threading::GetJobEntryPoint(UnifiedThreadPoolSheduledTask* task)
			{
				return task->This->callback;
			}

			UnifiedThreadPool UnifiedThreadPoolSheduledTask::GetOwnerPool() const
			{
				auto poolData = ((UnifiedThreadPool::Data*)(This->ownerPool));
				return UnifiedThreadPool(poolData);
			}
		}
	}
}

VOID NTAPI UnifiedTaskShedulerTaskCallback(
	_Inout_					PTP_CALLBACK_INSTANCE Instance,
	_Inout_ _Notnull_		PVOID                 Context,
	_Inout_					PTP_TIMER             Timer
) {
	using namespace Seed;
	using namespace Seed::Core::Threading;

	if (Context == NULL)
		return;

	UnifiedThreadPoolSheduledTask* thread = (UnifiedThreadPoolSheduledTask*)Context;

	if (!IsUnifiedThreadPoolTerminate(thread))
	{
		SRWLock& lock = GetUnifiedPoolLock(thread);
		if (!lock.TryInShareLock([&]()
			{
				EntryPoint([&]()
					{
						GetJobEntryPoint(thread)->Shedule();
					}, thread, Instance);
			}
		))
		{
					
			delete thread;
		}
	}
	else
	{
		delete thread;
	}
}

Seed::Core::Threading::UnifiedThreadPoolSheduledTask Seed::Core::Threading::UnifiedThreadPoolSheduledTaskBuilder::Shedule()&&
{
	if (This->proximity.Miliseconds > Math::Max<Int32>())
		Throw(InvalidArgumentException(ExceptionPosition, L"TimerProximity", L"Too low timer proximity(too big maximum allowd delay)"));

	if(This->intervalLenght.Ticks > Math::Max<Int32>())
		Throw(InvalidArgumentException(ExceptionPosition, L"IntervalLenght", L"Too big interval lenght"));

	Time::DateTime startTime = (!This->isRelaive) ? This->startTime : Time::DateTime::GetCurrentSystemTimeData().Ticks + This->startTime;

	auto shedulerTask = UnifiedThreadPoolSheduledTask(
		new UnifiedThreadPoolSheduledTask::Data(
			MoveRValue(This->taskName),
			MoveRValue(This->callback),
			This->ownerPool.This,
			This->startTime,
			This->intervalLenght,
			This->intervalsCount,
			(Int32)This->proximity.Ticks
		));

	NTSTATUS status = TpAllocTimer(
		(PTP_TIMER*)&shedulerTask.This->hTask,
		UnifiedTaskShedulerTaskCallback,
		new UnifiedThreadPoolSheduledTask(shedulerTask),
		reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->ownerPool.This->handlesData.TaskConfiguration.CallbackEnviroment)
	);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));

	LARGE_INTEGER DueTime = { 0 };

	DueTime.QuadPart = startTime.Ticks; 

	TpSetTimer(
		(PTP_TIMER)shedulerTask.This->hTask,
		&DueTime,
		static_cast<Int32>(This->intervalLenght.Miliseconds),
		static_cast<Int32>(This->proximity.Ticks)
	);

	return shedulerTask;
}

void Seed::Core::Threading::UnifiedThreadPoolSheduledTask::SetName(String&& newTaskName)
{
	_ReadWriteBarrier();
	This->taskName = MoveRValue(newTaskName);
}

void Seed::Core::Threading::UnifiedThreadPoolSheduledTask::StopSheduling()&&
{
	TpWaitForTimer((PTP_TIMER)This->hTask, TRUE);
	UnRegisterTaskFromPoolQueue(this);
}

void Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Dispose()
{
}

#pragma endregion

#pragma region Unified thread pool io tasks

namespace Seed {
	inline namespace Core {
		namespace IO {
			const SynchronizeEvent& GetIrpCompleteEvent(IRPBase* irp)
			{
				return irp->event;
			}

			void Seed::Core::IO::MarkIrpCompleted(IRPBase* irp)
			{
				irp->isCompleted = true;
				_ReadWriteBarrier();
			}
		}

		namespace Threading {
			Threadables::ThreadableRef RegisterThreadable(const UnifiedThreadPoolIoTask& task, const SynchronizeEvent& event, ImplSpecificT instance) {
				auto ref = Threadables::RegisterPoolIoThreadable(task, event, instance);
				SetName(task.Name, NtCurrentThread());
				_ReadWriteBarrier();
				return ref;
			}

			void UnRegisterThreadable(const UnifiedThreadPoolIoTask& task, Threadables::ThreadableRef&& ref) {
				Threadables::UnRegisterPoolIoThreadable(MoveRValue(ref));
				ReSetName(NtCurrentThread());
			}
		}
	}
}

template<typename EntryPointT>
static NTSTATUS EntryPoint(EntryPointT&& entryPoint, const UnifiedThreadPoolIoTask& thread, const SynchronizeEvent& event, ImplSpecificT instance) noexcept {
	using namespace Seed;
	using namespace Seed::Core::Threading;

	NTSTATUS status = STATUS_SUCCESS;

	Threadables::ThreadableRef ref;

	try {
		try {
			ref = RegisterThreadable(thread, event, instance);
			entryPoint();
		}
		catch (UnusedData Exceptions::InteruptException& er) {
			toCoreLog(L"Потік з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()));
			status = STATUS_THREAD_IS_TERMINATING;
		}
		catch (Exceptions::Exception& ex) {
			toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" виникла помилка " + ex.toString());
			status = STATUS_UNHANDLED_EXCEPTION;
		}
	}
	catch (...) {
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}

	try
	{
		UnRegisterThreadable(thread, MoveRValue(ref));
	}
	catch (Exception& ex)
	{
		toCoreLog(L"В потоці з іменем " + GetThisThreadable()->GetName() + L" та id " + String::toString(GetThisThreadable()->GetID()) + L" при виключені, виникла помилка " + ex.toString());
		status = STATUS_UNHANDLED_EXCEPTION;
	}
	catch (...)
	{
		toCoreLog(L"Не передбачувана помилка в " __FUNCSIG__);
		status = STATUS_INTERNAL_ERROR;
	}
	return status;
}

VOID NTAPI IoThreadPoolCallback(
	_Inout_ PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID Context,
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoSB,
	_In_ PTP_IO Io
) {
	using namespace Seed;
	using namespace Seed::Core::Threading;

	try {
		auto irp = (IO::IRPBase*)ApcContext;
		
		EntryPoint(
			[&]() { irp->OnIORequestComplete(IoSB->Information, NT_SUCCESS(IoSB->Status), IoSB->Status); },
			irp->TargetTask,
			GetIrpCompleteEvent(irp),
			Instance
		);

		MarkIrpCompleted(irp);

		delete irp;
	}
	catch (...) {
		toCoreLog(L"Виникла невідома помилка при обробці асинхроної операції в пулі потоків");
	}
}

Seed::Core::Threading::UnifiedThreadPoolIoTask Seed::Core::IO::IAsyncIOBase::RegisterNtIoObjectInThreadPool(const Threading::UnifiedThreadPool& pool, Handle hFileObj)
{
	auto This = UnifiedThreadPoolIoTask(new UnifiedThreadPoolIoTask::Data(hFileObj, pool.This));
	NTSTATUS status = TpAllocIoCompletion(
		(PTP_IO*)&This.This->hIoPoolObj,
		hFileObj,
		IoThreadPoolCallback,
		This.This,
		(PTP_CALLBACK_ENVIRON)pool.This->handlesData.IrpConfiguration.CallbackEnviroment);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	This.This->poolRef = pool.This->IrpQueue.AddLast(This);

	_ReadWriteBarrier();
	return This;
}

void Seed::Core::IO::IAsyncIOBase::UnRegisterNtIoObjectInThreadPool(Seed::Core::Threading::UnifiedThreadPoolIoTask task)
{
	UnifiedThreadPool((UnifiedThreadPool::Data*&)task.This->ownerPool).This->IrpQueue.Remove(task.This->poolRef);
	task.This->hFile = nullptr;
	task.This->ownerPool = nullptr;
	TpWaitForIoCompletion((PTP_IO)task.This->hIoPoolObj, FALSE);
	DestroyIoPoolObj(&task);
}

void* Seed::Core::IO::IAsyncIOBase::GetReservedPlaceInIrp(IRPBase* irp)
{
	return &irp->Reserverd;
}

Seed::Core::IO::IOTask Seed::Core::IO::IAsyncIOBase::GetIOTaskFromIRPBase(IRPBase* irp)
{
	return IOTask(irp);
}

void Seed::Core::IO::IAsyncIOBase::NotifyIoOperation(const Threading::UnifiedThreadPoolIoTask& ptpIo)
{
	TpStartAsyncIoOperation((PTP_IO)ptpIo.This->hIoPoolObj);
}

void Seed::Core::IO::IAsyncIOBase::CancelNotifyIoOperation(const Threading::UnifiedThreadPoolIoTask& ptpIo)
{
	TpCancelAsyncIoOperation((PTP_IO)ptpIo.This->hIoPoolObj);
}

void Seed::Core::IO::IRPBase::CancelIo()
{
	HANDLE hFile = this->irpTarget->GetHandle();

	if (hFile == nullptr) {
		Throw(NullPointException(ExceptionPosition));
	}

	IO_STATUS_BLOCK ioSb = { 0 };
	NTSTATUS status = NtCancelIoFileEx(hFile, (PIO_STATUS_BLOCK)&this->Reserverd, &ioSb);

	if (!NT_SUCCESS(status)) {
		Throw(NtException(ExceptionPosition, status));
	}

	this->irpTarget->CancelNotifyIoOperation(this->TargetTask);
}

void Seed::Core::Threading::UnifiedThreadPoolIoTask::Dispose()
{
	This->hFile = nullptr;
	This->hIoPoolObj = nullptr;
	if (This->ownerPool != nullptr) {
		UnifiedThreadPool((UnifiedThreadPool::Data*&)This->ownerPool).This->IrpQueue.Remove(This->poolRef);
		This->ownerPool = nullptr;
	}
}

#pragma endregion
#pragma endregion

#pragma region System Thread Pool
UnifiedThreadPool* lowPriorityPool = nullptr;
UnifiedThreadPool* mediumPriorityPool = nullptr;
UnifiedThreadPool* highPriorityPool = nullptr;

#pragma region Job
UnifiedThreadPoolJob Seed::Core::Threading::SystemThreadPool::PostPriorityJob(Delegate&& action, String&& name, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->PostJob(MoveRValue(name), MoveRValue(action));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->PostJob(MoveRValue(name), MoveRValue(action));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->PostJob(MoveRValue(name), MoveRValue(action));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

UnifiedThreadPoolJob Seed::Core::Threading::SystemThreadPool::PostPriorityJob(Delegate&& action, const String& name, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->PostJob(name, MoveRValue(action));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->PostJob(name, MoveRValue(action));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->PostJob(name, MoveRValue(action));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

UnifiedThreadPoolJob Seed::Core::Threading::SystemThreadPool::PostPriorityJob(Delegate&& action, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->PostJob(MoveRValue(action));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->PostJob(MoveRValue(action));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->PostJob(MoveRValue(action));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}
#pragma endregion
#pragma region Scheduled tasks
UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::SystemThreadPool::CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& name, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->SheduleJob(MoveRValue(name), MoveRValue(callback));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->SheduleJob(MoveRValue(name), MoveRValue(callback));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->SheduleJob(MoveRValue(name), MoveRValue(callback));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::SystemThreadPool::CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, const String& name, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->SheduleJob(name, MoveRValue(callback));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->SheduleJob(name, MoveRValue(callback));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->SheduleJob(name, MoveRValue(callback));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::SystemThreadPool::CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->SheduleJob(MoveRValue(callback));
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->SheduleJob(MoveRValue(callback));
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->SheduleJob(MoveRValue(callback));
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}
#pragma endregion
#pragma region Statistic
SizeT Seed::Core::Threading::SystemThreadPool::AllItemQueueLenght()
{
	return
		lowPriorityPool->JobsCount		+ lowPriorityPool->SheduledTaskCount	+ lowPriorityPool->IORequestCount	 +
		mediumPriorityPool->JobsCount	+ mediumPriorityPool->SheduledTaskCount + mediumPriorityPool->IORequestCount +
		highPriorityPool->JobsCount		+ highPriorityPool->SheduledTaskCount	+ highPriorityPool->IORequestCount;
}

SizeT Seed::Core::Threading::SystemThreadPool::AllItemSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->JobsCount + lowPriorityPool->SheduledTaskCount + lowPriorityPool->IORequestCount;
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->JobsCount + mediumPriorityPool->SheduledTaskCount + mediumPriorityPool->IORequestCount;
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->JobsCount + highPriorityPool->SheduledTaskCount + highPriorityPool->IORequestCount;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

SizeT Seed::Core::Threading::SystemThreadPool::JobQueueLenght()
{
	return lowPriorityPool->JobsCount + mediumPriorityPool->JobsCount + highPriorityPool->JobsCount;
}

SizeT Seed::Core::Threading::SystemThreadPool::JobSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->JobsCount;
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->JobsCount;
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->JobsCount;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

SizeT Seed::Core::Threading::SystemThreadPool::ScheduledTaskQueueLenght()
{
	return lowPriorityPool->SheduledTaskCount + mediumPriorityPool->SheduledTaskCount + highPriorityPool->SheduledTaskCount;
}

SizeT Seed::Core::Threading::SystemThreadPool::ScheduledTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->SheduledTaskCount;
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->SheduledTaskCount;
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->SheduledTaskCount;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

SizeT Seed::Core::Threading::SystemThreadPool::IoTaskQueueLenght()
{
	return lowPriorityPool->IORequestCount + mediumPriorityPool->IORequestCount + highPriorityPool->IORequestCount;
}

SizeT Seed::Core::Threading::SystemThreadPool::IoTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority)
{
	switch (priority)
	{
	case Threading::EPoolThreadsPriority::Low:
		return lowPriorityPool->IORequestCount;
	case Threading::EPoolThreadsPriority::Medium:
		return mediumPriorityPool->IORequestCount;
	case Threading::EPoolThreadsPriority::High:
		return highPriorityPool->IORequestCount;
	default:
		Throw(InvalidArgumentException(ExceptionPosition, L"priority", L"Invalid thread pool task priority"));
	}
}

#pragma endregion
#pragma endregion

#pragma region Init & Stop

extern __declspec(noreturn) InternalApiCall void EmergencyShutdown(
	Seed::Core::ConstAString File,
	Seed::Core::ConstAString Function,
	Seed::Core::UInt64 Line,
	Seed::Core::ConstString Reason
);

void Seed::Core::Stage1Complete() noexcept
{
	try {
		
		lowPriorityPool = new UnifiedThreadPool(UnifiedThreadPoolBuilder(L"Low priority default system thread pool")
			.SetMinThreadCount(0)
			.SetMaxThreadCount((UInt32)CurrentSettings().VirtualSetting.CpuCoresCount)
			.SetThreadPoolPriority(Threading::EPoolThreadsPriority::Low)
			.CreateThreadPool());

		mediumPriorityPool = new UnifiedThreadPool(UnifiedThreadPoolBuilder(L"Low priority default system thread pool")
			.SetMinThreadCount(0)
			.SetMaxThreadCount((UInt32)CurrentSettings().VirtualSetting.CpuCoresCount)
			.SetThreadPoolPriority(Threading::EPoolThreadsPriority::Medium)
			.CreateThreadPool());

		highPriorityPool = new UnifiedThreadPool(UnifiedThreadPoolBuilder(L"Low priority default system thread pool")
			.SetMinThreadCount(0)
			.SetMaxThreadCount((UInt32)CurrentSettings().VirtualSetting.CpuCoresCount)
			.SetThreadPoolPriority(Threading::EPoolThreadsPriority::High)
			.CreateThreadPool());

		Threading::MainThread([]() {Stage2(); });

		delete lowPriorityPool;
		delete mediumPriorityPool;
		delete highPriorityPool;

		lowPriorityPool = nullptr;
		mediumPriorityPool = nullptr;
		highPriorityPool = nullptr;
	}
	catch (...) {
		EmergencyShutdown(ExceptionPosition, L"Не можливо ініціалізувати підсистему підтримки багатопотоковості");
	}
}

#pragma endregion

#pragma warning(pop)