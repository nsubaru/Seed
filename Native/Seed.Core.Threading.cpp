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
				virtual IThreadable& Join(Time::DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { thread->Join(timeOut); return *this; }

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
				virtual IThreadable& Join(Time::DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { thread->Join(timeOut); return *this; }

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
				virtual IThreadable& Join(Time::DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { thread->Join(timeOut); return *this; }

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

			Thread& Thread::Join(Time::DateTime timeOut) //-V813
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
				info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize.Bytes;
				info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize.Bytes;
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

			JobPool& JobPool::Join(Time::DateTime timeOut) //-V813
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
				if (thread->This->RemainingPeriodsCount == 0 && thread->This->PeriodsCount != 0)
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

			void ZeroingTaskHadle(SheduledJob* thread)
			{
				thread->This->hTask = nullptr;
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
					GetJobEntryPoint(thread).Execute(Time::DateTime::GetCurrentSystemTimeData());
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
	Seed::Core::Threading::ZeroingTaskHadle(This);
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
				info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize.Bytes;
				info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize.Bytes;
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

			void TaskSheduler::InitTaskSheduler(UInt32 minThreadCount, UInt32 maxThreadCount)
			{
				NTSTATUS status = TpAllocPool(reinterpret_cast<PTP_POOL*>(&This->pool), NULL);
				if (!NT_SUCCESS(status))
					Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(reinterpret_cast<PTP_POOL*>(&This->pool)), NullPointException::Types::Write));

				TpSetPoolMinThreads(reinterpret_cast<PTP_POOL>(This->pool), static_cast<LONG>(minThreadCount));
				TpSetPoolMaxThreads(reinterpret_cast<PTP_POOL>(This->pool), static_cast<LONG>(maxThreadCount));
				TP_POOL_STACK_INFORMATION info;
				info.StackCommit = CurrentSettings().Threading.Thread.StackStartSize.Bytes;
				info.StackReserve = CurrentSettings().Threading.Thread.StackMaxSize.Bytes;
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

				LARGE_INTEGER DueTime = { 0 };

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
				}

				if (This->Task != nullptr) {
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
				LARGE_INTEGER delay = { 0 };
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

			SheduledJob& SheduledJob::Join(Time::DateTime timeOut) { //-V813
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
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = Math::Max<Int64>();
				NtWaitForSingleObject(this->data->handle, TRUE, &delay);
			}

			void SynchronizeEvent::Wait(SizeT TimeOutMs) const {
				LARGE_INTEGER delay = { 0 };
				delay.QuadPart = (TimeOutMs > 0) ? -10000LL * TimeOutMs : Math::Max<Int64>();
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

#include "Seed.Core.Threading.UnifiedThreadPool.h"

#pragma region Unified thread pool

#pragma region Implementation specific

void Seed::Core::Threading::UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj)
{
	((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->JobsQueue.Remove(jobObj->This->poolQueueRef);
}

void Seed::Core::Threading::UnRegisterTaskFromPoolQueue(UnifiedThreadPoolSheduledTask* thread)
{
	((UnifiedThreadPool::Data*)thread->This->ownerPool)->TaskQueue.Remove(thread->This->poolQueueRef);
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

namespace Seed{
	inline namespace Core {
		namespace IO {
			void* GetIrpFromReserved(void* reservedAddr)
			{
				return (void*)(StructOffsetOfMember(IO::IRPBase, Reserverd) + (SizeT)reservedAddr);
			}
		}
	}
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
	ConfigureTls(nullptr, nullptr);
	try {
		auto jobObj = (UnifiedThreadPoolIoTask*)ObjectContext;
		delete jobObj;
	}
	catch (...) {
		toCoreLog(L"Невідома помилка при знищенні об'єкта I/O задачі в пулі");
	}
	DestroyTls();
}

VOID NTAPI IoFinalizer(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context
	) {
	ConfigureTls(nullptr, nullptr);
	try{
		//delete IO::GetIrpFromReserved(Context);
	}
	catch (...) {
		toCoreLog(L"Невідома помилка при знищенні об'єкта I/O задачі в пулі");
	}
	DestroyTls();
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

	threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);

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

	threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);

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

	threadPoolEnviroment->CallbackPriority = TP_CALLBACK_PRIORITY_NORMAL;
	threadPoolEnviroment->Size = sizeof(TP_CALLBACK_ENVIRON);

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

#pragma region Unified thread pool threadables
namespace Seed {
	inline namespace Core {
		namespace Threading {
			class ThreadableUnifiedThreadPoolSheduledTask final : public Seed::Core::Threading::IThreadable {
			private:
				friend void RegisterThread(UnifiedThreadPoolSheduledTask* thread);
			private:
				UnifiedThreadPoolSheduledTask* thread;
			private:
				ThreadableUnifiedThreadPoolSheduledTask(UnifiedThreadPoolSheduledTask* thread) : thread(thread) {}
			public:
				virtual const String& GetName() const { return thread->This->taskName; }
				virtual void SetName(const String& newName) { thread->SetName(newName); }

				virtual UInt64 GetID() const { return thread->This->activeTid; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::ScheduledTask; }
				virtual EThreadabledStates GetThreadabledState() const { return thread->This->activeState; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { thread->Suspend(); return *this; }
				virtual IThreadable& Resume() { thread->Resume(); return *this; }
				virtual IThreadable& Interupt() { thread->Interupt(); return *this; }

				virtual IThreadable& Join() { thread->Join(); return *this; }
				virtual IThreadable& Join(Time::DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { thread->Join(timeOut); return *this; }

				virtual ~ThreadableUnifiedThreadPoolSheduledTask()
				{
					delete thread;
				}
			};

			class ThreadableUnifiedThreadPoolJob final : public Seed::Core::Threading::IThreadable {
			private:
				friend void RegisterThread(UnifiedThreadPoolJob* thread);
			private:
				UnifiedThreadPoolJob* thread;
			private:
				ThreadableUnifiedThreadPoolJob(UnifiedThreadPoolJob* thread) : thread(thread) {}
			public:
				virtual const String& GetName() const { return thread->This->jobName; }
				virtual void SetName(const String& newName) { thread->SetName(newName); }

				virtual UInt64 GetID() const { return thread->This->threadId; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::JobTask; }
				virtual EThreadabledStates GetThreadabledState() const { return thread->This->threadableState; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { thread->Suspend(); return *this; }
				virtual IThreadable& Resume() { thread->Resume(); return *this; }
				virtual IThreadable& Interupt() { thread->Interupt(); return *this; }

				virtual IThreadable& Join() { thread->Join(); return *this; }
				virtual IThreadable& Join(Time::DateTime timeOut) { thread->Join(timeOut); return *this; }
				virtual const IThreadable& Join() const { thread->Join(); return *this; }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { thread->Join(timeOut); return *this; }

				virtual ~ThreadableUnifiedThreadPoolJob()
				{
					delete thread;
				}
			};

			class ThreadableUnifiedThreadPoolIoTask final : public Seed::Core::Threading::IThreadable, protected IDisposable {
			private:
				friend Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator RegisterThread(UnifiedThreadPoolIoTask* task, PTP_CALLBACK_INSTANCE instance);
				friend void UnRegisterThread(Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator&& threadable);
			private:
				struct Data {
					UnifiedThreadPoolIoTask* task;
					EThreadabledStates state;
					EThreadabledStates secondState;
					Handle hThread;
					ID threadId;
					PTP_CALLBACK_INSTANCE instance;

					RefCountT RefCount;

					Data(UnifiedThreadPoolIoTask* task,
						Handle hThread,
						ID threadId,
						PTP_CALLBACK_INSTANCE instanc
					) :
						task(task),
						hThread(hThread),
						threadId(threadId),
						instance(instance),
						state(EThreadabledStates::Starting),
						secondState(EThreadabledStates::Paused),
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
					if(This->hThread != nullptr)
						NtClose(This->hThread);

					This->hThread = nullptr;
					This->task = nullptr;
					This->instance = nullptr;
				}
			private:
				ThreadableUnifiedThreadPoolIoTask(
					UnifiedThreadPoolIoTask* task, 
					Handle hThread, 
					ID threadId,
					PTP_CALLBACK_INSTANCE instance
				)  : This(new Data(task, hThread, threadId, instance))
				{}
			public:
				virtual const String& GetName() const { return This->task->GetName(); }
				virtual void SetName(const String& newName) { This->task->SetName(newName); if(This->hThread != nullptr) ::SetName(newName, This->hThread); }

				virtual UInt64 GetID() const { return This->threadId; }

				virtual EThreadableTypes GetThreadableType() const { return EThreadableTypes::IoTask; }
				virtual EThreadabledStates GetThreadabledState() const { return This->state; }

				virtual Containers::Array<String> GetCallStack() const { Throw(Exceptions::NotImplementedException(ExceptionPosition)); }

				virtual IThreadable& GetParrentThreadable() const { Throw(Exceptions::NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), Exceptions::NullPointException::Types::Read)); }
				virtual Boolean IsHaveParrentThreadable() const { return false; }

				virtual IThreadable& Suspend() { 

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
							if (This->hThread != nullptr) {
								This->secondState = EThreadabledStates::Paused;
								Swap(This->state, This->secondState);

								NTSTATUS status = NtSuspendThread(This->hThread, NULL);
								if (!NT_SUCCESS(status)) {
									Swap(This->state, This->secondState);
									return *this;
								}
								TpDisassociateCallback(This->instance);
							}
							break;
						}
						case EThreadabledStates::Paused: break;
						default:
							break;
					}
					return *this;
				}

				virtual IThreadable& Resume() {
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
						if (This->hThread != nullptr) {
							This->secondState = EThreadabledStates::Paused;
							Swap(This->state, This->secondState);
							NtAlertResumeThread(This->hThread, NULL);
							break;
						}
					}
					default:
						break;
					}
					return *this;
				}
				virtual IThreadable& Interupt() {
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

				virtual IThreadable& Join() { Throw(NotImplementedException(ExceptionPosition)); }
				virtual IThreadable& Join(Time::DateTime timeOut) { Throw(NotImplementedException(ExceptionPosition)); }
				virtual const IThreadable& Join() const { Throw(NotImplementedException(ExceptionPosition)); }
				virtual const IThreadable& Join(Time::DateTime timeOut) const { Throw(NotImplementedException(ExceptionPosition)); }
			public:
				RefCountClass(ThreadableUnifiedThreadPoolIoTask, This);
			};
		}
	}
}
#pragma endregion

#pragma region Unified thread pool job

void Seed::Core::Threading::SetUnifiedThreadPoolJobInstanceHandleObj(UnifiedThreadPoolJob* jobObj, ImplSpecificT hInstance)
{
	jobObj->This->hInstance = hInstance;
}

bool Seed::Core::Threading::IsUnifiedThreadPoolTerminate(UnifiedThreadPoolJob* jobObj)
{
	return ((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->isPoolTermination;
}

void Seed::Core::Threading::SetEventOnJobFinish(UnifiedThreadPoolJob* jobObj)
{
	TpCallbackSetEventOnCompletion((PTP_CALLBACK_INSTANCE)jobObj->This->hInstance, jobObj->This->hEvent);
}

Delegate& Seed::Core::Threading::GetJobEntryPoint(UnifiedThreadPoolJob* jobObj)
{
	return jobObj->This->action;
}
namespace Seed {
	inline namespace Core {
		namespace Threading {
			Seed::Core::Threading::SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj)
			{
				return ((UnifiedThreadPool::Data*)jobObj->This->pOwnerPool)->handlesData.JobConfiguration.PoolLock;
			}
		}
	}
}


void Seed::Core::Threading::RegisterThread(UnifiedThreadPoolJob* thread)
{
	thread->This->threadbleRef = AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableUnifiedThreadPoolJob(new UnifiedThreadPoolJob(*thread))));
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &thread->This->hThread, GENERIC_ALL, 0, 0);
	ConfigureTls(new ThreadableUnifiedThreadPoolJob(thread), &thread->This->threadableState);
	SetName(thread->This->jobName, thread->This->hThread);
	thread->This->threadableState = EThreadabledStates::Executing;
	_ReadWriteBarrier();
}

void Seed::Core::Threading::UnRegisterThread(UnifiedThreadPoolJob* thread)
{
	AllActiveThreadable->Remove(thread->This->threadbleRef);
	ReSetName(thread->This->hThread);
	NtClose(thread->This->hThread);
	thread->This->hThread = nullptr;
	thread->This->threadableState = EThreadabledStates::Ended;
	thread->This->hInstance = nullptr;
	thread->This->poolQueueRef.Remove();
	_ReadWriteBarrier();
	DestroyTls();
}

UnifiedThreadPool Seed::Core::Threading::UnifiedThreadPoolJob::GetOwnerPool() const
{
	auto poolObj = (UnifiedThreadPool::Data*)(UnifiedThreadPool::Data*)This->pOwnerPool;
	return UnifiedThreadPool((UnifiedThreadPool::Data*&)poolObj);
}

VOID NTAPI UnifiedPoolWorkCallbak(
	_Inout_     PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID                 Context,
	_Inout_     PTP_WORK              Work
) {
	using namespace Seed;
	if (Context == nullptr)
		return;
	UnifiedThreadPoolJob* thread = (UnifiedThreadPoolJob*)Context;
	SetUnifiedThreadPoolJobInstanceHandleObj(thread, Instance);
	if (!IsUnifiedThreadPoolTerminate(thread)) {
		SetEventOnJobFinish(thread);
		SRWLock& lock = GetUnifiedPoolLock(thread);
		if (!lock.TryInShareLock([&]() { EntryPoint(GetJobEntryPoint(thread), thread); })) {
			UnregisterJobFromJobList(thread);
			delete thread;
		}
	}
	else {
		UnregisterJobFromJobList(thread);
		delete thread;
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

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join(Time::DateTime timeOut)
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.TimeFromStartEpoch;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

const UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Join(Time::DateTime timeOut) const
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.TimeFromStartEpoch;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Suspend()
{
	_ReadWriteBarrier();

	switch (This->threadableState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing:
	case EThreadabledStates::ExceptionProcessing:
	{
		This->secondThreadableState = EThreadabledStates::Paused;
		Swap(This->threadableState, This->secondThreadableState);
		
		NTSTATUS status = NtSuspendThread(This->hThread, NULL);
		if (!NT_SUCCESS(status)) {
			Swap(This->threadableState, This->secondThreadableState);
			return *this;
		}
		TpDisassociateCallback((PTP_CALLBACK_INSTANCE)This->hInstance);
		break;
	}
	case EThreadabledStates::Paused: break;
	default:
		break;
	}
	return *this;
}

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Resume()
{
	_ReadWriteBarrier();

	switch (This->threadableState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing: break;
	case EThreadabledStates::ExceptionProcessing: break;
	case EThreadabledStates::Paused:
	{
		This->secondThreadableState = EThreadabledStates::Paused;
		Swap(This->threadableState, This->secondThreadableState);
		NtAlertResumeThread(This->hThread, NULL);
		break;
	}
	default:
		break;
	}
	return *this;
}

UnifiedThreadPoolJob& Seed::Core::Threading::UnifiedThreadPoolJob::Interupt()
{
	_ReadWriteBarrier();

	switch (This->threadableState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	default:
		switch (This->threadableState)
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

void Seed::Core::Threading::UnifiedThreadPoolJob::SetName(String&& newName)
{
	_ReadWriteBarrier();

	switch (This->threadableState)
	{
	case EThreadabledStates::NotStarted:
	case EThreadabledStates::Interupted: 
	case EThreadabledStates::Unknown:
	case EThreadabledStates::Starting:
		This->jobName = MoveRValue(newName);
	break;
	default:
		switch (This->threadableState)
		{
		case EThreadabledStates::Executing:
		case EThreadabledStates::ExceptionProcessing:
		case EThreadabledStates::Paused:
		default:
			This->jobName = MoveRValue(newName);
			::SetName(This->jobName, This->hThread);
			break;
		}
	}
}

void Seed::Core::Threading::UnifiedThreadPoolJob::Dispose()
{
	NtClose(This->hEvent);
}
#pragma endregion

#pragma region Unified thread pool sheduler task

Seed::Core::Threading::UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::UnifiedThreadPool::SheduleJob(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
{
	return UnifiedThreadPoolSheduledTaskBuilder(*this, MoveRValue(callback));
}

Seed::Core::Threading::UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::UnifiedThreadPool::SheduleJob(String&& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
{
	return UnifiedThreadPoolSheduledTaskBuilder(*this, MoveRValue(callback), MoveRValue(taskName));
}

UnifiedThreadPoolSheduledTaskBuilder Seed::Core::Threading::UnifiedThreadPool::SheduleJob(const String& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
{
	return UnifiedThreadPoolSheduledTaskBuilder(*this, MoveRValue(callback), String(taskName));
}

SRWLock& Seed::Core::Threading::GetTaskLock(UnifiedThreadPoolSheduledTask* task)
{
	return task->This->taskLock;
}

void Seed::Core::Threading::SetCallbackInstanceObj(UnifiedThreadPoolSheduledTask* task, ImplSpecificT callbackInstance)
{
	task->This->callbackInstance = callbackInstance;
}

bool Seed::Core::Threading::IsUnifiedThreadPoolTerminate(UnifiedThreadPoolSheduledTask* task)
{
	return ((UnifiedThreadPool::Data*)(task->This->ownerPool))->isPoolTermination;
}

SRWLock& Seed::Core::Threading::GetUnifiedPoolLock(UnifiedThreadPoolSheduledTask* task)
{
	return ((UnifiedThreadPool::Data*)(task->This->ownerPool))->handlesData.TaskConfiguration.PoolLock;
}

SmartPtr<IUnifiedThreadPoolShedulerCallback>& Seed::Core::Threading::GetJobEntryPoint(UnifiedThreadPoolSheduledTask* task)
{
	return task->This->callback;
}

void Seed::Core::Threading::PulseEvent(UnifiedThreadPoolSheduledTask* task)
{
	NtPulseEvent(task->This->hEvent, NULL);
}

UnifiedThreadPool Seed::Core::Threading::UnifiedThreadPoolSheduledTask::GetOwnerPool() const
{
	auto poolData = ((UnifiedThreadPool::Data*)(This->ownerPool));
	return UnifiedThreadPool(poolData);
}

void Seed::Core::Threading::RegisterThread(UnifiedThreadPoolSheduledTask* thread)
{
	thread->This->activeTid = (SizeT)NtCurrentThreadId();
	thread->This->threadbleRef = AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableUnifiedThreadPoolSheduledTask(new UnifiedThreadPoolSheduledTask(*thread))));
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &thread->This->hActiveThread, GENERIC_ALL, 0, 0);
	ConfigureTls(new ThreadableUnifiedThreadPoolSheduledTask(new UnifiedThreadPoolSheduledTask(*thread)), &thread->This->activeState);
	SetName(thread->This->taskName, thread->This->hActiveThread);
	thread->This->activeState = EThreadabledStates::Executing;
	_ReadWriteBarrier();
}

void Seed::Core::Threading::UnRegisterThread(UnifiedThreadPoolSheduledTask* thread)
{
	AllActiveThreadable->Remove(thread->This->threadbleRef);
	ReSetName(thread->This->hActiveThread);
	NtClose(thread->This->hActiveThread);

	thread->This->activeState = EThreadabledStates::Ended;
	thread->This->callbackInstance = nullptr;
	thread->This->activeTid = 0;
	thread->This->hActiveThread = nullptr;
	thread->This->executedCount++;

	if (thread->This->remainingCount != 0) {
		--thread->This->remainingCount;
	}

	if (thread->This->remainingCount == 0 && thread->This->intervalsCount != 0) {
		TpReleaseTimer(reinterpret_cast<PTP_TIMER>(thread->This->hTask));
		delete thread;
	}

	_ReadWriteBarrier();
	DestroyTls();
}

VOID NTAPI UnifiedTaskShedulerTaskCallback(
	_Inout_					PTP_CALLBACK_INSTANCE Instance,
	_Inout_ _Notnull_		PVOID                 Context,
	_Inout_					PTP_TIMER             Timer
) {
	using namespace Seed;

	if (Context == NULL)
		return;

	UnifiedThreadPoolSheduledTask* thread = (UnifiedThreadPoolSheduledTask*)Context;
	GetTaskLock(thread).ExclusiveLock([&]()
		{
			SetCallbackInstanceObj(thread, Instance);
			if (!IsUnifiedThreadPoolTerminate(thread))
			{
				SRWLock& lock = GetUnifiedPoolLock(thread);
				if (!lock.TryInShareLock([&]()
					{
						EntryPoint([&]()
							{
								GetJobEntryPoint(thread)->Shedule();
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

	NTSTATUS status = NtCreateEvent(&shedulerTask.This->hEvent, GENERIC_ALL, NULL, EVENT_TYPE::NotificationEvent, FALSE);

	if (!NT_SUCCESS(status))
		Throw(NtException(ExceptionPosition, status));

	status = TpAllocTimer(
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
	if (This->hActiveThread != nullptr) {
		::SetName(This->taskName, This->hActiveThread);
	}
}

UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Suspend()
{
	_ReadWriteBarrier();
	switch (This->activeState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing:
	case EThreadabledStates::ExceptionProcessing:
	{
		This->secondState = EThreadabledStates::Paused;
		Swap(This->activeState, This->secondState);
		
		NTSTATUS status = NtSuspendThread(This->hActiveThread, NULL);
		if (!NT_SUCCESS(status)) {
			Swap(This->activeState, This->secondState);
			return *this;
		}
		TpDisassociateCallback((PTP_CALLBACK_INSTANCE)This->callbackInstance);
		break;
	}
	case EThreadabledStates::Paused: break;
	default:
		break;
	}
	return *this;
}

UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Resume()
{
	_ReadWriteBarrier();
	switch (This->activeState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	case EThreadabledStates::Executing: break;
	case EThreadabledStates::ExceptionProcessing: break;
	case EThreadabledStates::Paused:
	{
		This->secondState = EThreadabledStates::Paused;
		Swap(This->activeState, This->secondState);
		NTSTATUS status = NtAlertResumeThread(This->hActiveThread, NULL);
		if (!NT_SUCCESS(status)) {
			Swap(This->activeState, This->secondState);
		}
		break;
	}
	default:
		break;
	}
	return *this;
}

UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Interupt()
{
	_ReadWriteBarrier();
	switch (This->activeState)
	{
	case EThreadabledStates::NotStarted: break;
	case EThreadabledStates::Interupted: break;
	case EThreadabledStates::Unknown: break;
	case EThreadabledStates::Starting: break;
	default:
		switch (This->activeState)
		{
		case EThreadabledStates::Executing:
		case EThreadabledStates::ExceptionProcessing:
		case EThreadabledStates::Paused:
		{
			NTSTATUS status = NtSuspendThread(This->hActiveThread, NULL);
			if (!NT_SUCCESS(status)) {
				return *this;
			}
			CONTEXT ctx = { 0 };
			ctx.ContextFlags = CONTEXT_ALL;
			status = ZwGetContextThread(This->hActiveThread, &ctx);
			if (!NT_SUCCESS(status))
			{
				NtResumeThread(This->hActiveThread, NULL);
				Throw(NtException(ExceptionPosition, status));
			}
			DWORD64 ret_point = ctx.Rip;
			ctx.Rip = (DWORD64)(PDWORD64)InteruptThread;
			status = ZwSetContextThread(This->hActiveThread, &ctx);
			NtResumeThread(This->hActiveThread, NULL);
			if (!NT_SUCCESS(status))
				Throw(NtException(ExceptionPosition, status));
			NtAlertThread(This->hActiveThread);
		}
		default:
			break;
		}
		break;
	}
	return *this;
}

UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Join()
{
	NtWaitForSingleObject(This->hEvent, TRUE, NULL);
	return *this;
}

UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Join(Time::DateTime timeOut)
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.TimeFromStartEpoch;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

const UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Join() const
{
	NtWaitForSingleObject(This->hEvent, TRUE, NULL);
	return *this;
}

const UnifiedThreadPoolSheduledTask& Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Join(Time::DateTime timeOut) const
{
	LARGE_INTEGER delay = { 0 };
	delay.QuadPart = timeOut.TimeFromStartEpoch;
	NtWaitForSingleObject(This->hEvent, TRUE, &delay);
	return *this;
}

void Seed::Core::Threading::UnifiedThreadPoolSheduledTask::Dispose()
{
}

#pragma endregion

#pragma region Unified thread pool io tasks

Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator Seed::Core::Threading::RegisterThread(UnifiedThreadPoolIoTask* task, PTP_CALLBACK_INSTANCE instance)
{
	Handle hThread = nullptr;
	ID threadId = Threading::GetTID();
	NtDuplicateObject(NtCurrentProcess(), NtCurrentThread(), NtCurrentProcess(), &hThread, GENERIC_ALL, 0, 0);

	auto threadable = ThreadableUnifiedThreadPoolIoTask(new UnifiedThreadPoolIoTask(*task), hThread, threadId, instance);

	auto threadbleRef = 
		AllActiveThreadable->AddLast(SmartPtr<IThreadable>(new ThreadableUnifiedThreadPoolIoTask(threadable)));
	
	ConfigureTls(new ThreadableUnifiedThreadPoolIoTask(threadable), &threadable.This->state);
	SetName(threadable.GetName(), threadable.This->hThread);
	threadable.This->state = EThreadabledStates::Executing;
	_ReadWriteBarrier();

	return threadbleRef;
}

void Seed::Core::Threading::UnRegisterThread(Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator&& threadableRef)
{
	SmartPtr<IThreadable> ref = *threadableRef;
	AllActiveThreadable->Remove(threadableRef);
	ReSetName(ref.As<ThreadableUnifiedThreadPoolIoTask>().This->hThread);
	NtClose(ref.As<ThreadableUnifiedThreadPoolIoTask>().This->hThread);
	ref.As<ThreadableUnifiedThreadPoolIoTask>().This->hThread = nullptr;
	ref.As<ThreadableUnifiedThreadPoolIoTask>().This->instance = nullptr;
	ref.As<ThreadableUnifiedThreadPoolIoTask>().This->task = nullptr;
	DestroyTls();
	
	_ReadWriteBarrier();
}

void Seed::Core::IO::SetEventIfEventAvaiable(IRPBase* irp)
{
	if (irp->event != nullptr)
		irp->event->Set();
}

VOID NTAPI IoThreadPoolCallback(
	_Inout_ PTP_CALLBACK_INSTANCE Instance,
	_Inout_opt_ PVOID Context,
	_In_ PVOID ApcContext,
	_In_ PIO_STATUS_BLOCK IoSB,
	_In_ PTP_IO Io
) {
	Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator threadableRef;
	try {
		try {
			threadableRef = RegisterThread((UnifiedThreadPoolIoTask*)Context, Instance);
			auto irp = (IO::IRPBase*)ApcContext;
			irp->OnIORequestComplete(IoSB->Information, NT_SUCCESS(IoSB->Status), IoSB->Status);
			IO::SetEventIfEventAvaiable(irp);
			delete irp;
		}
		catch (InteruptException&) {
			toCoreLog(L"I/O задача була скасована");
		}
		catch (Exception& ex) {
			toCoreLog(L"Виникла помилка при обробці I/O задачі: ", ex);
		}
		catch (...) {
			toCoreLog(L"Виникла невідома помилка при обробці I/O задачі");
		}

		UnRegisterThread(MoveRValue(threadableRef));
	}
	catch (InteruptException&) {
		toCoreLog(L"I/O задача була скасована");
	}
	catch (Exception& ex) {
		toCoreLog(L"Виникла помилка при обробці I/O задачі: ", ex);
	}
	catch (...) {
		toCoreLog(L"Виникла невідома помилка при обробці I/O задачі");
	}	
}

Seed::Core::Threading::UnifiedThreadPoolIoTask Seed::Core::IO::IAsyncIOBase::RegisterNtIoObjectInThreadPool(const Threading::UnifiedThreadPool& pool, Handle hFileObj)
{
	auto This = UnifiedThreadPoolIoTask(new UnifiedThreadPoolIoTask::Data(hFileObj, pool.This));
	NTSTATUS status = TpAllocIoCompletion(
		(PTP_IO*)&This.This->hIoPoolObj,
		hFileObj,
		IoThreadPoolCallback,
		new UnifiedThreadPoolIoTask(This),
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
	TpWaitForIoCompletion((PTP_IO)task.This->hIoPoolObj, TRUE);
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

#pragma warning(pop)