// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Threading.Thread.h"
#include "Seed.Core.Threading.TaskSheduler.h"
#include "Seed.Core.Threading.ThreadPool.h"

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

static Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>* AllActiveThreadable;

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

void Seed::Core::Threading::Sleep(DateTime time, Boolean IsRelative) //-V813
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
			class ThreadableThread final : public Seed::Core::Threading::IThreadable {
			private:
				friend void RegisterThread(Thread* thread);
			private:
				Thread* thread;
			private:
				ThreadableThread(Thread* thread) : thread(thread) {}
			public:
				virtual const String& GetName() const { return const_cast<String&>(thread->This->name); }
				virtual void SetName(const String& NewName) { thread->SetName(NewName); }

				virtual UInt64 GetID() const { return thread->This->TID; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::Thread; }
				virtual EThreadabledStates GetThreadabledState() const { return thread->This->State; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { thread->Suspend(); return *this; }
				virtual IThreadable& Resume() { thread->Resume(); return *this; }
				virtual IThreadable& Interupt() { thread->Interupt(); return *this; }

				virtual IThreadable& Join() { thread->Join(); return *this; }
				virtual IThreadable& Join(DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(DateTime timeOut) const { thread->Join(timeOut); return *this; }

				virtual ~ThreadableThread() override
				{
					delete thread;
				}
			};

			class ThreadableJobPool final : public Seed::Core::Threading::IThreadable {
			private:
				friend void RegisterThread(JobPool* thread);
			private:
				JobPool* thread;
			private:
				ThreadableJobPool(JobPool* thread) : thread(thread) {}
			public:
				virtual const String& GetName() const { return const_cast<String&>(thread->This->Name); }
				virtual void SetName(const String& NewName) { thread->SetName(NewName); }

				virtual UInt64 GetID() const { return thread->This->TID; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::JobTask; }
				virtual EThreadabledStates GetThreadabledState() const { return thread->This->State; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { thread->Suspend(); return *this; }
				virtual IThreadable& Resume() { thread->Resume(); return *this; }
				virtual IThreadable& Interupt() { thread->Interupt(); return *this; }

				virtual IThreadable& Join() { thread->Join(); return *this; }
				virtual IThreadable& Join(DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(DateTime timeOut) const { thread->Join(timeOut); return *this; }

				virtual ~ThreadableJobPool()
				{
					delete thread;
				}
			};

			class ThreadableSheduledTask final : public Seed::Core::Threading::IThreadable {
			private:
				friend void RegisterThread(SheduledJob* thread);
			private:
				SheduledJob* thread;
			private:
				ThreadableSheduledTask(SheduledJob* thread) : thread(thread) {}
			public:
				virtual const String& GetName() const { return const_cast<String&>(thread->This->Name); }
				virtual void SetName(const String& NewName) { thread->SetName(NewName); }

				virtual UInt64 GetID() const { return thread->This->TID; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::ScheduledTask; }
				virtual EThreadabledStates GetThreadabledState() const { return thread->This->State; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { thread->Suspend(); return *this; }
				virtual IThreadable& Resume() { thread->Resume(); return *this; }
				virtual IThreadable& Interupt() { thread->Interupt(); return *this; }

				virtual IThreadable& Join() { thread->Join(); return *this; }
				virtual IThreadable& Join(DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(DateTime timeOut) const { thread->Join(timeOut); return *this; }

				virtual ~ThreadableSheduledTask()
				{
					delete thread;
				}
			};

			void RegisterThread(Thread* thread)
			{
				thread->This->This = AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableThread(new Thread(*thread))));
				ConfigureTls(new ThreadableThread(thread), &thread->This->State);
				thread->This->State = EThreadabledStates::Executing;
				thread->This->AbortEvent->Set();
				SetName(const_cast<const String&>(thread->This->name), thread->This->hThread);
			}

			void UnRegisterThread(Thread* thread)
			{
				ReSetName(thread->This->hThread);
				AllActiveThreadable->Remove(thread->This->This);
				thread->This->State = EThreadabledStates::Ended;
				DestroyTls();
			}

			volatile Delegate& ThreadEntryPoint(Thread* thread)
			{
				return thread->This->delegate;
			}
		}
	}
}

template<typename EntryPointT, typename ThreadableType>
static NTSTATUS EntryPoint(EntryPointT&& entryPoint, ThreadableType* thread) noexcept {
	using namespace Seed;

	NTSTATUS status = STATUS_SUCCESS;

	try {
		try {
			RegisterThread(thread);
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
		UnRegisterThread(thread);
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

NTSTATUS __cdecl threadStartRoutineDynEntryPoint(PVOID args) noexcept {
	Thread* This = (Thread*)args;
	return EntryPoint(Seed::Core::Threading::ThreadEntryPoint(This), This);
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
				EntryPoint(MainThread->This->delegate, MainThread);
				//Interupt and wait for interupting each exist theradable
				AllActiveThreadable->ForEach([] (SmartPtr<IThreadable>& thread) InlineLambda
				{
					toCoreLog(L"Помилка: Потік з ID: " + String::toString(thread->GetID()) + L" та з міткою: \"" + thread->GetName() + L"\" примусово зупиняється внаслідок завершення ядра");
					thread->Interupt()
						.Join(CurrentSettings().Threading.Thread.StopKernelThreadsTimeout);
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
					Seed::CurrentSettings().Threading.Thread.StackMaxSize,
					Seed::CurrentSettings().Threading.Thread.StackStartSize,
					threadStartRoutineDynEntryPoint,
					(void*)new Thread(*this), //-V572
					&This->hThread,
					&id);
				this->This->State = EThreadabledStates::Starting;
				this->This->TID = reinterpret_cast<SizeT>(id.UniqueThread);
				toCoreLog(String::Combine({ L"Запуск потоку з ім'ям \"", this->GetName(), L"\" з ID ", String::toString(this->This->TID), L" з потоку з ID ", String::toString(Threading::GetTID()) }));
				return *this;
			}

			Thread& Thread::Join() {
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = Math::Max<Int64>();
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			Thread& Thread::Join(DateTime timeOut) //-V813
			{
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = timeOut.TimeFromStartEpoch;
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			Thread& Thread::Interupt() {
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
					This->ActiveState = EThreadabledStates::Paused;
					Swap(This->State, This->ActiveState);
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
					Swap(This->State, This->ActiveState);
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

#pragma region Thread Pool

VOID NTAPI CleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext
) {
	UNREFERENCED_PARAMETER(CleanupContext);
	try {
		try {
			JobPool* data = reinterpret_cast<JobPool*>(ObjectContext);
			delete data;
		}
		catch (Exception & ex) {
			toCoreLog(L"При скасовуванні задачі виникла помилка: " + ex.toString());
		}
	}
	catch (...) {
		toCoreLog(L"При скасовуванні задачі виникла невідома помилка");
	}
}

namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{

			ForceInline void RegisterThread(JobPool* thread) {
				thread->This->This = AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableJobPool(new JobPool(*thread))));
				HANDLE threadHandle;
				NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &threadHandle, GENERIC_ALL, 0, 0);
				thread->This->hThread = threadHandle;
				ConfigureTls(new ThreadableJobPool(thread), &thread->This->State);
				SetName(thread->This->Name, threadHandle);
				thread->This->State = EThreadabledStates::Executing;
			}

			ForceInline void UnRegisterThread(JobPool* thread) {
				AllActiveThreadable->Remove(thread->This->This);
				--thread->This->Parrent->JobsCounts;
				ReSetName(thread->This->hThread);
				NtClose(thread->This->hThread);
				thread->This->hThread = nullptr;
				thread->This->State = EThreadabledStates::Ended;
				thread->This->instance = nullptr;
				DestroyTls();
			}

			ForceInline void SetEventOnJobEnd(JobPool* thread, void* callbackObj) {
				TpCallbackSetEventOnCompletion((PTP_CALLBACK_INSTANCE) callbackObj, thread->This->event);
			}

			ForceInline void SetCallbackInstanceObj(JobPool* thread, void* callbackObj) {
				thread->This->instance = callbackObj;
			}

			ForceInline Delegate& GetJobEntryPoint(JobPool* thread) {
				return thread->This->delegate;
			}

			ForceInline SRWLock& GetPoolLock(JobPool* thread) {
				return thread->This->Parrent->lock;
			}

			ForceInline Boolean IsPoolTerminating(JobPool* thread) {
				return thread->This->Parrent->IsTerminating;
			}
		}
	}
}

VOID NTAPI PoolWorkCallbak(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
) {
	using namespace Seed;
	if (Context == nullptr)
		return;
	JobPool* thread = (JobPool*)Context;
	SetCallbackInstanceObj(thread, Instance);
	if (!IsPoolTerminating(thread)) {
		SetEventOnJobEnd(thread, Instance);
		SRWLock& lock = GetPoolLock(thread);
		if (!lock.TryInShareLock([&]() { EntryPoint(GetJobEntryPoint(thread), thread); }))
			delete thread;
	}
	else {
		delete thread;
	}
	TpReleaseWork(Work);
}

namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{
			void ThreadPool::InitThreadPool() try
			{
				NTSTATUS status = TpAllocPool((PTP_POOL*) &This->pool, NULL);
				if (!NT_SUCCESS(status))
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(This->pool), NullPointException::Types::Write));

				TpSetPoolMinThreads(reinterpret_cast<PTP_POOL>(This->pool), (LONG) CurrentSettings().VirtualSetting.CpuCoresCount / 2);
				TpSetPoolMaxThreads(reinterpret_cast<PTP_POOL>(This->pool), (LONG) CurrentSettings().VirtualSetting.CpuCoresCount); //-V2571
				TP_POOL_STACK_INFORMATION info;
				info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize;
				info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize;
				TpSetPoolStackInformation(reinterpret_cast<PTP_POOL>(This->pool), &info); //-V2571
				status = TpAllocCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP*>(&This->cleanupData));

				if (!NT_SUCCESS(status))
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(This->cleanupData), NullPointException::Types::Write));

				This->env = new TP_CALLBACK_ENVIRON;
				auto threadPoolEnviroment = reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env); //-V2571
				threadPoolEnviroment->Version = 3;
				threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(This->pool); //-V2571
				threadPoolEnviroment->CleanupGroup = reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData); //-V2571
				threadPoolEnviroment->CleanupGroupCancelCallback = CleanupCallback;
				threadPoolEnviroment->RaceDll = NULL;
				threadPoolEnviroment->ActivationContext = NULL;
				threadPoolEnviroment->FinalizationCallback = NULL;
				threadPoolEnviroment->u.Flags = 0;
				threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
				threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);
			}
			catch (...)
			{
#pragma warning(push)
#pragma warning(disable:6001)
				if (this->This != nullptr)
				{
					if (This->pool != nullptr)
					{
						if (This->cleanupData != nullptr)
						{
							if (This->env != nullptr)
							{
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

			JobPool& JobPool::Dispatch() {
				PTP_WORK work = nullptr;
				HANDLE event = nullptr;

				try
				{
					NTSTATUS status = NtCreateEvent(&event, GENERIC_ALL, NULL, EVENT_TYPE::NotificationEvent, FALSE);
					if (!NT_SUCCESS(status))
						Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(event), NullPointException::Types::Write));

					status = TpAllocWork(&work, PoolWorkCallbak, new JobPool(*this), reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->Parrent->env)); //-V2571

					if (!NT_SUCCESS(status))
						Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(work), NullPointException::Types::Write));

					++This->Parrent->JobsCounts;
					This->event = event;
					This->work = work;
					TpPostWork(work);
				}
				catch (...)
				{
#pragma warning(push)
#pragma warning(disable:6001)
					if (event != nullptr)
					{
						NtClose(event);
					}
#pragma warning(pop)
					ReThrow;
				}
				return *this;
			}

			void ThreadPool::Dispose() {
				This->IsTerminating = true;
				This->lock.ExclusiveLock([&] ()
				{
					TpReleaseCleanupGroupMembers(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData), TRUE, NULL);
					TpReleaseCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData));
					TpReleasePool(reinterpret_cast<PTP_POOL>(This->pool));
					delete reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env);
				});
			}

			JobPool& JobPool::Interupt() {
				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting: break;
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
							Throw(NtException(ExceptionPosition, status));
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

			JobPool& JobPool::Join() {
				LARGE_INTEGER delay;
				delay.QuadPart = Math::Max<Int64>();
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			JobPool& JobPool::Suspend() {
				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting: break;
				case EThreadabledStates::Executing:
				case EThreadabledStates::ExceptionProcessing:
				{
					This->ActiveState = EThreadabledStates::Paused;
					Swap(This->State, This->ActiveState);
					TpDisassociateCallback((PTP_CALLBACK_INSTANCE) This->instance);
					NtSuspendThread(This->hThread, NULL);
					break;
				}
				case EThreadabledStates::Paused:
					NtSuspendThread(This->hThread, NULL);
					break;
				default:
					break;
				}
				return *this;
			}

			JobPool& JobPool::Resume() {
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
					This->ActiveState = EThreadabledStates::Paused;
					Swap(This->State, This->ActiveState);
					NtAlertResumeThread(This->hThread, NULL);
					break;
				}
				default:
					break;
				}
				return *this;
			}

			JobPool& JobPool::Join(DateTime timeOut) //-V813
			{
				LARGE_INTEGER delay;
				delay.QuadPart = timeOut.TimeFromStartEpoch;
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			void JobPool::SetName(const String& NewName) {
				This->Name = NewName;
				if(This->hThread != nullptr)
					::SetName(This->Name, This->hThread);
			}
		}
	}
}
#pragma endregion

#pragma region Task Sheduler
namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{
			ForceInline void RegisterThread(SheduledJob* thread) {
				thread->This->This = AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableSheduledTask(new SheduledJob(*thread))));
				HANDLE threadHandle;
				NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &threadHandle, GENERIC_ALL, 0, 0);
				thread->This->hThread = threadHandle;
				ConfigureTls(new ThreadableSheduledTask(new SheduledJob(*thread)), &thread->This->State);
				SetName(thread->This->Name, threadHandle);
				thread->This->State = EThreadabledStates::Executing;
			}

			ForceInline void UnRegisterThread(SheduledJob* thread) {
				AllActiveThreadable->Remove(thread->This->This);
				ReSetName(thread->This->hThread);
				NtClose(thread->This->hThread);
				thread->This->State = EThreadabledStates::Ended;
				if (thread->This->RemainingPeriodsCount == 0 && thread->This->PeriodsCount != 1)
					TpReleaseTimer(reinterpret_cast<PTP_TIMER>(thread->This->hTask));
				DestroyTls();
			}

			ForceInline volatile ITask& GetJobEntryPoint(SheduledJob* thread) {
				return *thread->This->Task;
			}

			ForceInline SRWLock& GetPoolLock(SheduledJob* thread) {
				return thread->This->sheduler->lock;
			}

			ForceInline Boolean IsPoolTerminating(SheduledJob* thread) {
				return thread->This->sheduler->IsTerminating;
			}

			ForceInline void SetCallbackInstanceObj(SheduledJob* thread, void* callbackObj) {
				thread->This->hId = callbackObj;
			}

			ForceInline SRWLock& GetTaskLock(SheduledJob* thread) {
				return thread->This->lock;
			}
		}
	}
}

VOID NTAPI TaskShedulerTaskCallback(
	_Inout_					PTP_CALLBACK_INSTANCE Instance,
	_Inout_ _Notnull_		PVOID                 Context,
	_Inout_					PTP_TIMER             Timer
) {
	using namespace Seed;
	if (Context == NULL)
		return;
	SheduledJob* thread = (SheduledJob*)Context;
	GetTaskLock(thread).ExclusiveLock([&] ()
	{
		SetCallbackInstanceObj(thread, Instance);
		if (!IsPoolTerminating(thread))
		{
			SRWLock& lock = GetPoolLock(thread);
			if (!lock.TryInShareLock([&] ()
			{
				EntryPoint([&] ()
				{
					GetJobEntryPoint(thread).Execute(DateTime::GetCurrentSystemTimeData());
				}, thread);
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
	});
}

VOID NTAPI TaskShedulerCleanupCallback(
	_Inout_opt_ PVOID ObjectContext,
	_Inout_opt_ PVOID CleanupContext	
) {
	if (ObjectContext == NULL)
		return;
	Seed::Core::Threading::SheduledJob* This = (Seed::Core::Threading::SheduledJob*)ObjectContext;
	delete This;
}

namespace Seed
{
	inline namespace Core
	{
		namespace Threading
		{
			void TaskSheduler::InitTaskSheduler() {
				NTSTATUS status = TpAllocPool(reinterpret_cast<PTP_POOL*>(&This->pool), NULL);
				if (!NT_SUCCESS(status))
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(reinterpret_cast<PTP_POOL*>(&This->pool)), NullPointException::Types::Write));

				TpSetPoolMinThreads(reinterpret_cast<PTP_POOL>(This->pool), static_cast<LONG>(CurrentSettings().VirtualSetting.CpuCoresCount / 2));
				TpSetPoolMaxThreads(reinterpret_cast<PTP_POOL>(This->pool), static_cast<LONG>(CurrentSettings().VirtualSetting.CpuCoresCount));
				TP_POOL_STACK_INFORMATION info;
				info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize;
				info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize;
				TpSetPoolStackInformation(reinterpret_cast<PTP_POOL>(This->pool), &info);
				status = TpAllocCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP*>(&This->cleanupData));

				if (!NT_SUCCESS(status))
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData)), NullPointException::Types::Write));

				This->env = new TP_CALLBACK_ENVIRON;
				auto threadPoolEnviroment = reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env);

				threadPoolEnviroment->Version = 3;
				threadPoolEnviroment->Pool = reinterpret_cast<PTP_POOL>(This->pool);
				threadPoolEnviroment->CleanupGroup = reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData);
				threadPoolEnviroment->CleanupGroupCancelCallback = TaskShedulerCleanupCallback;
				threadPoolEnviroment->RaceDll = NULL;
				threadPoolEnviroment->ActivationContext = NULL;
				threadPoolEnviroment->FinalizationCallback = NULL;
				threadPoolEnviroment->u.Flags = 0;
				threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
				threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);
			}

			void TaskSheduler::Dispose() {
				This->IsTerminating = true;
				This->lock.ExclusiveLock([&] ()
				{
					TpReleaseCleanupGroupMembers(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData), TRUE, NULL);
					TpReleaseCleanupGroup(reinterpret_cast<PTP_CLEANUP_GROUP>(This->cleanupData));
					TpReleasePool(reinterpret_cast<PTP_POOL>(This->pool));
					delete reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->env);
				});
			}

			SheduledJob SheduledJobBuilder::Shedule() {
				if (This->StartTime.TimeFromStartEpoch == 0 && This->PeriodLenght == 0)
					Throw(
						InvalidArgumentException(
							ExceptionPosition,
							L"StartTime",
							L"Час запуску або довжина періода мають бути вказані"
						)
					);
				if (This->Task == nullptr)
					Throw(
						InvalidArgumentException(
							ExceptionPosition,
							L"Task",
							L"Дія має бути вказана обов'язково"
						)
					);

				NTSTATUS status = TpAllocTimer(
					const_cast<PTP_TIMER*>(reinterpret_cast<volatile PTP_TIMER*>(&This->hTask)),
					TaskShedulerTaskCallback,
					new SheduledJob(This),
					reinterpret_cast<PTP_CALLBACK_ENVIRON>(This->sheduler->env)
				);
				if (!NT_SUCCESS(status)) //TODO: Analyse status code to more precision exception generation
					Throw(NtException(ExceptionPosition, status));

				LARGE_INTEGER DueTime;

				DueTime.QuadPart = (this->IsExcecutionTimeRelative) ? This->StartTime.RelativeTime : This->StartTime.TimeFromStartEpoch;

				TpSetTimer(
					const_cast<PTP_TIMER>(reinterpret_cast<PTP_TIMER>(This->hTask)),
					&DueTime,
					static_cast<Int32>(This->PeriodLenght),
					static_cast<Int32>(This->TimerProximity)
				);

				return SheduledJob(MoveRValue(This));
			}

			void Seed::Core::Threading::SheduledJob::Dispose() {
				if (This->hTask != nullptr)
				{
					if (This->RemainingPeriodsCount != 0 && This->PeriodsCount != 1)
						TpReleaseTimer(reinterpret_cast<PTP_TIMER>(This->hTask));
					delete This->Task;
				}
			}

			SheduledJob& SheduledJob::Interupt() {
				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting: break;
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
							Throw(NtException(ExceptionPosition, status));
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

			SheduledJob& SheduledJob::Join() {
				LARGE_INTEGER delay;
				delay.QuadPart = Math::Max<Int64>();
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
				return *this;
			}

			SheduledJob& SheduledJob::Suspend() {
				switch (This->State)
				{
				case EThreadabledStates::NotStarted: break;
				case EThreadabledStates::Interupted: break;
				case EThreadabledStates::Unknown: break;
				case EThreadabledStates::Starting: break;
				case EThreadabledStates::Executing:
				case EThreadabledStates::ExceptionProcessing:
				{
					This->ActiveState = EThreadabledStates::Paused;
					Swap(This->State, This->ActiveState);
					NtSuspendThread(This->hThread, NULL);
					break;
				}
				case EThreadabledStates::Paused: break;
				default:
					break;
				}
				return *this;
			}

			SheduledJob& SheduledJob::Resume() {
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
					This->ActiveState = EThreadabledStates::Paused;
					Swap(This->State, This->ActiveState);
					NtAlertResumeThread(This->hThread, NULL);
					break;
				}
				default:
					break;
				}
				return *this;
			}

			SheduledJob& SheduledJob::Join(DateTime timeOut) { //-V813
				LARGE_INTEGER delay;
				delay.QuadPart = timeOut.TimeFromStartEpoch;
				if (This->State == EThreadabledStates::Starting ||
					This->State == EThreadabledStates::Executing ||
					This->State == EThreadabledStates::ExceptionProcessing ||
					This->State == EThreadabledStates::Paused)
					NtWaitForSingleObject(This->hThread, TRUE, &delay);
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
				LARGE_INTEGER delay;
				delay.QuadPart = Math::Max<Int64>();
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			void SynchronizeEvent::Wait(SizeT TimeOutMs) const {
				LARGE_INTEGER delay;
				delay.QuadPart = (TimeOutMs > 0) ? -10000LL * TimeOutMs : Math::Max<Int64>();
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			UInt64 WaitForMany(const Containers::Array<SynchronizeEvent>& events, Boolean isWaitAll, SizeT TimeOutMs) {
				if (events.Count > static_cast<SizeT>(Math::Max<UInt32>()))
					Throw(InvalidArgumentException(ExceptionPosition, L"events.Count", L"events count must be less than ULONG max"));

				HANDLE* hEvents = nullptr;

				LARGE_INTEGER delay;
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
		Threading::MainThread([]() {Stage2(); });
	}
	catch (...) {
		EmergencyShutdown(ExceptionPosition, L"Не можливо ініціалізувати підсистему підтримки багатопотоковості");
	}
}

#pragma endregion

#pragma warning(pop)