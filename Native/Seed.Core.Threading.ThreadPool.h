#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class JobPool;
			class ThreadPool;

			class ThreadPool final: IDisposable{
			private:
				//Дані диспетчера
				struct dispatcher {
					//Керуюча структура пула потоків
					ImplSpecificT pool;
					//Настройки середовищп для пула потоків
					ImplSpecificT env;

					ImplSpecificT cleanupData;
					//Синхронізація задач з деструктором і підрахунок активних задач
					SRWLock lock;
					//Кількість задач в пулі
					RefCountT JobsCounts;

					RefCountT JobIndex;
					//Кількість силок на даний екземаляр
					RefCountT RefCount;

					Boolean IsTerminating;

					String Name;

					forceinline dispatcher(const String& Name) :
						Name(Name),
						pool(nullptr),
						env(nullptr),
						cleanupData(nullptr),
						lock(),
						JobsCounts(0),
						JobIndex(0),
						RefCount(1),
						IsTerminating(false)
					{}

					forceinline dispatcher(String&& Name) :
						Name(MoveRValue(Name)),
						pool(nullptr),
						env(nullptr),
						cleanupData(nullptr),
						lock(),
						JobsCounts(0),
						JobIndex(0),
						RefCount(1),
						IsTerminating(false)
					{}
				};
			private:
				//Дані даного екземаляра
				dispatcher* This;
			protected:
				SeedKernelCall virtual void Dispose() override final;
			private:
				SeedKernelCall void InitThreadPool();
				forceinline ThreadPool(dispatcher* This) : This(This) { ++This->RefCount; }
			private:
				SeedKernelCall friend SRWLock GetPoolLock(JobPool* thread);
				SeedKernelCall friend Boolean IsPoolTerminating(JobPool* thread);
			public:
				forceinline ThreadPool() : This(new dispatcher("Unnamed Thread Pool")) { InitThreadPool(); }
				forceinline ThreadPool(const String& Name) : This(new dispatcher(Name)) { InitThreadPool(); }
				forceinline ThreadPool(String&& Name) : This(new dispatcher(MoveRValue(Name))) { InitThreadPool(); }
			public:
				SeedKernelCall JobPool Dispatch(const String& Name, Delegate&& delegate);
				SeedKernelCall JobPool Dispatch(String&& Name, Delegate&& delegate);
				SeedKernelCall JobPool Dispatch(Delegate&& delegate);
			public:
				SeedKernelCall SizeT GetTasksCount() { return This->JobsCounts; }
			public:
				friend class JobPool;
			public:
				RefCountClass(ThreadPool, This)
			};

			class JobPool final :IDisposable{
			private:
				struct Data {
					volatile EThreadabledStates State;
					//Івент який буде переведено в сигнальний стан лиш коли задача завершиться
					Handle event;
					//Дескриптор задачі в пулі потоків NT
					ImplSpecificT work;
					//Кількість силок на задачу
					RefCountT RefCount;
					//Делегат
					Delegate delegate;
					//ID потоку в якому виконується задача
					SizeT TID;
					//Ім'я
					String Name;

					Handle hThread;

					RefCountT JobIndex;

					ThreadPool::dispatcher* Parrent;

					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;
					//Конструктор
					forceinline Data(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) :
						State(EThreadabledStates::NotStarted),
						delegate(MoveRValue(delegate)),
						Parrent(Parrent.This),
						JobIndex(Parrent.This->JobIndex++),
						event(nullptr),
						work(nullptr),
						hThread(nullptr),
						This(),
						Name(Name),
						RefCount(1),
						TID(0)
					{}
					//Конструктор
					forceinline Data(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) :
						State(EThreadabledStates::NotStarted),
						delegate(MoveRValue(delegate)),
						Parrent(Parrent.This),
						JobIndex(Parrent.This->JobIndex++),
						event(nullptr),
						work(nullptr),
						hThread(nullptr),
						This(),
						Name(MoveRValue(Name)),
						RefCount(1),
						TID(0)
					{}
				};
			private:
				Data* This;
			private:
				SeedKernelCall friend void RegisterThread(JobPool* thread);
				SeedKernelCall friend void UnRegisterThread(JobPool* thread);
				SeedKernelCall friend Delegate& GetJobEntryPoint(JobPool* thread);
				SeedKernelCall friend void SetEventOnJobEnd(JobPool* thread, void* callbackObj);
				SeedKernelCall friend SRWLock GetPoolLock(JobPool* thread);
				SeedKernelCall friend Boolean IsPoolTerminating(JobPool* thread);
				SeedKernelCall friend class ThreadableJobPool;
			private:
				forceinline JobPool(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), Name, Parrent)) {}
				forceinline JobPool(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), MoveRValue(Name), Parrent)) {}
			private:
				SeedKernelCall JobPool& Dispatch();
			protected:
				forceinline virtual void Dispose() override final { This->Parrent = nullptr; }
			public:
				forceinline ThreadPool GetThreadPool() { return ThreadPool(This->Parrent); }
				forceinline EThreadabledStates GetCurrentState() { return This->State; }
				SeedKernelCall JobPool& Interupt();
				SeedKernelCall JobPool& Join();
				SeedKernelCall JobPool& Suspend();
				SeedKernelCall JobPool& Join(DateTime timeOut);
				forceinline Boolean IsValid() { return This != nullptr; }
			public:
				forceinline const String& GetName() { return This->Name; }
				forceinline void SetName(const String& NewName) { This->Name = NewName; }
				forceinline void SetName(String&& NewName) { This->Name = MoveRValue(NewName); }
			public:
				String Property(GetName, SetName) Name;
			public:
				RefCountClass(JobPool, This)
			public:
				friend class ThreadPool;
			};

			forceinline JobPool ThreadPool::Dispatch(const String& Name, Delegate&& delegate)
			{
				auto obj = JobPool(MoveRValue(delegate), Name, *this);
				obj.Dispatch();
				return obj;
			}

			forceinline JobPool ThreadPool::Dispatch(String&& Name, Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), MoveRValue(Name), *this);
				obj.Dispatch();
				return obj;
			}

			forceinline JobPool ThreadPool::Dispatch(Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), L"Unnamed job", *this);
				obj.Dispatch();
				return obj;
			}
		}
	}
}