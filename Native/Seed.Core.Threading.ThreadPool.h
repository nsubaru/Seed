#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class JobPool;
			class ThreadPool;

			class SeedKernelCall ThreadPool final: IDisposable {
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

					dispatcher(const String& Name) :
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

					dispatcher(String&& Name) :
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
				virtual void Dispose() override final;
			private:
				void InitThreadPool();
				ThreadPool(dispatcher* This) : This(This) { ++This->RefCount; }
			private:
				friend SRWLock GetPoolLock(JobPool* thread);
				friend Boolean IsPoolTerminating(JobPool* thread);
			public:
				ThreadPool() : This(new dispatcher("Unnamed Thread Pool")) { InitThreadPool(); }
				ThreadPool(const String& Name) : This(new dispatcher(Name)) { InitThreadPool(); }
				ThreadPool(String&& Name) : This(new dispatcher(MoveRValue(Name))) { InitThreadPool(); }
			public:
				JobPool Dispatch(const String& Name, Delegate&& delegate);
				JobPool Dispatch(String&& Name, Delegate&& delegate);
				JobPool Dispatch(Delegate&& delegate);
			public:
				SizeT GetTasksCount() { return This->JobsCounts; }
			public:
				friend class JobPool;
			public:
				RefCountClass(ThreadPool, This)
			};

			class SeedKernelCall JobPool final :IDisposable{
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

					SensitiveList<SmartPtr<IThreadable>>::Iterator This;
					//Конструктор
					Data(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) :
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
					Data(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) :
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
				friend void RegisterThread(JobPool* thread);
				friend void UnRegisterThread(JobPool* thread);
				friend Delegate& GetJobEntryPoint(JobPool* thread);
				friend void SetEventOnJobEnd(JobPool* thread, void* callbackObj);
				friend SRWLock GetPoolLock(JobPool* thread);
				friend Boolean IsPoolTerminating(JobPool* thread);
				friend class ThreadableJobPool;
			private:
				JobPool(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), Name, Parrent)) {}
				JobPool(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), MoveRValue(Name), Parrent)) {}
			private:
				JobPool& Dispatch();
			protected:
				virtual void Dispose() override final { This->Parrent = nullptr; }
			public:
				ThreadPool GetThreadPool() { return ThreadPool(This->Parrent); }
				EThreadabledStates GetCurrentState() { return This->State; }
				JobPool& Interupt();
				JobPool& Join();
				JobPool& Suspend();
				JobPool& Join(DateTime timeOut);
				Boolean IsValid() { return This != nullptr; }
			public:
				const String& GetName() { return This->Name; }
				void SetName(const String& NewName) { This->Name = NewName; }
				void SetName(String&& NewName) { This->Name = MoveRValue(NewName); }
			public:
				RefCountClass(JobPool, This)
			public:
				friend class ThreadPool;
			};

			inline JobPool ThreadPool::Dispatch(const String& Name, Delegate&& delegate)
			{
				auto obj = JobPool(MoveRValue(delegate), Name, *this);
				obj.Dispatch();
				return obj;
			}

			inline JobPool ThreadPool::Dispatch(String&& Name, Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), MoveRValue(Name), *this);
				obj.Dispatch();
				return obj;
			}

			inline JobPool ThreadPool::Dispatch(Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), L"Unnamed job", *this);
				obj.Dispatch();
				return obj;
			}
		}
	}
}