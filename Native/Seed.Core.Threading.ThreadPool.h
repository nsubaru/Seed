#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class JobPool;
			class ThreadPool;

			/// <summary>
			/// Пул потоків
			/// </summary>
			class ThreadPool final: IDisposable{
			private:
				/// <summary>
				/// Дані диспетчера
				/// </summary>
				struct dispatcher {
					/// <summary>
					/// Керуюча структура пула потоків
					/// </summary>
					ImplSpecificT pool;
					/// <summary>
					/// Настройки середовищп для пула потоків
					/// </summary>
					ImplSpecificT env;
					/// <summary>
					/// Налаштування завершення задач
					/// </summary>
					ImplSpecificT cleanupData;
					/// <summary>
					/// Синхронізація задач з деструктором і підрахунок активних задач
					/// </summary>
					SRWLock lock;
					/// <summary>
					/// Кількість задач в пулі
					/// </summary>
					RefCountT JobsCounts;
					/// <summary>
					/// Індекс останьої задачі
					/// </summary>
					RefCountT JobIndex;
					/// <summary>
					/// Кількість силок на даний екземаляр
					/// </summary>
					RefCountT RefCount;
					/// <summary>
					/// true якщо пул завершує роботу
					/// </summary>
					volatile Boolean IsTerminating;
					/// <summary>
					/// Ім'я пулу потоків
					/// </summary>
					String Name;

					ForceInline dispatcher(const String& Name) :
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

					ForceInline dispatcher(String&& Name) :
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
				/// <summary>
				/// Дані даного екземаляра
				/// </summary>
				dispatcher* This;
			protected:
				SeedKernelCall virtual void Dispose() override final;
			private:
				SeedKernelCall void InitThreadPool();
				ForceInline ThreadPool(dispatcher* This) : This(This) { ++This->RefCount; }
			private:
				SeedKernelCall friend SRWLock& GetPoolLock(JobPool* thread);
				SeedKernelCall friend Boolean IsPoolTerminating(JobPool* thread);
			public:
				/// <summary>
				/// Конструктор пулу потоків з стандартним іменем
				/// </summary>
				ForceInline ThreadPool() : This(new dispatcher("Unnamed Thread Pool")) { InitThreadPool(); }
				/// <summary>
				/// Конструктор пулу потоків з заданим іменем
				/// </summary>
				/// <param name="Name">Ім'я пулу</param>
				ForceInline ThreadPool(const String& Name) : This(new dispatcher(Name)) { InitThreadPool(); }
				/// <summary>
				/// Конструктор пулу потоків з заданим іменем
				/// </summary>
				/// <param name="Name">Ім'я пулу</param>
				ForceInline ThreadPool(String&& Name) : This(new dispatcher(MoveRValue(Name))) { InitThreadPool(); }
			public:
				/// <summary>
				/// Запуск іменованої задачі в пул потоків
				/// </summary>
				/// <param name="Name">Ім'я задачі</param>
				/// <param name="delegate">Дія</param>
				/// <returns>Об'єкт задачі</returns>
				SeedKernelCall JobPool Dispatch(const String& Name, Delegate&& delegate);
				/// <summary>
				/// Запуск іменованої задачі в пул потоків
				/// </summary>
				/// <param name="Name">Ім'я задачі</param>
				/// <param name="delegate">Дія</param>
				/// <returns>Об'єкт задачі</returns>
				SeedKernelCall JobPool Dispatch(String&& Name, Delegate&& delegate);
				/// <summary>
				/// Запуск безіменної задачі в пул потоків
				/// </summary>
				/// <param name="delegate">Дія</param>
				/// <returns>Об'єкт задачі</returns>
				SeedKernelCall JobPool Dispatch(Delegate&& delegate);
			public:
				/// <summary>
				/// Кількість задач в пулі
				/// </summary>
				/// <returns></returns>
				SeedKernelCall SizeT GetTasksCount() { return This->JobsCounts; }
			public:
				friend class JobPool;
			public:
				RefCountClass(ThreadPool, This)
			};

			/// <summary>
			/// Задача в пулі задач
			/// </summary>
			class JobPool final : IDisposable{
			private:
				/// <summary>
				/// Дані задачі
				/// </summary>
				struct Data {
					/// <summary>
					/// Стан задачі
					/// </summary>
					volatile EThreadabledStates State;
					/// <summary>
					/// Активний стан задачі
					/// </summary>
					volatile EThreadabledStates ActiveState;
					/// <summary>
					/// Івент який буде переведено в сигнальний стан лиш коли задача завершиться
					/// </summary>
					Handle event;
					/// <summary>
					/// Дескриптор задачі в пулі потоків NT
					/// </summary>
					ImplSpecificT work;
					/// <summary>
					/// Дескриптор екземпляра завдання в пулі потоків NT
					/// </summary>
					ImplSpecificT instance;
					/// <summary>
					/// Кількість силок на задачу
					/// </summary>
					RefCountT RefCount;
					/// <summary>
					/// Делегат
					/// </summary>
					Delegate delegate;
					/// <summary>
					/// ID потоку в якому виконується задача
					/// </summary>
					SizeT TID;
					/// <summary>
					/// Ім'я
					/// </summary>
					String Name;
					/// <summary>
					/// Дескриптор на потік
					/// </summary>
					volatile Handle hThread;
					/// <summary>
					/// Ідентифікатор задачі
					/// </summary>
					RefCountT JobIndex;
					/// <summary>
					/// Посилання на пул який є власником задачі
					/// </summary>
					ThreadPool::dispatcher* Parrent;
					/// <summary>
					/// Ітератор на задачу в списку потокових сутностей
					/// </summary>
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					/// <summary>
					/// Конструктор
					/// </summary>
					/// <param name="delegate">Дія</param>
					/// <param name="Name">Ім'я задачі</param>
					/// <param name="Parrent">Посилання на пул який є власником задачі</param>
					ForceInline Data(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) :
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
					
					/// <summary>
					/// Конструктор
					/// </summary>
					/// <param name="delegate">Дія</param>
					/// <param name="Name">Ім'я задачі</param>
					/// <param name="Parrent">Посилання на пул який є власником задачі</param>
					ForceInline Data(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) :
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
				SeedKernelCall friend void SetCallbackInstanceObj(JobPool* thread, void* callbackObj);
				SeedKernelCall friend SRWLock& GetPoolLock(JobPool* thread);
				SeedKernelCall friend Boolean IsPoolTerminating(JobPool* thread);
				SeedKernelCall friend class ThreadableJobPool;
			private:
				ForceInline JobPool(Delegate&& delegate, const String& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), Name, Parrent)) {}
				ForceInline JobPool(Delegate&& delegate, String&& Name, const ThreadPool& Parrent) : This(new Data(MoveRValue(delegate), MoveRValue(Name), Parrent)) {}
			private:
				SeedKernelCall JobPool& Dispatch();
			protected:
				ForceInline virtual void Dispose() override final { This->Parrent = nullptr; }
			public:
				/// <summary>
				/// Вертає пул потоків який є власником задачі
				/// </summary>
				/// <returns>Пул потоків який є власником задачі</returns>
				ForceInline ThreadPool GetThreadPool() { return ThreadPool(This->Parrent); }
				/// <summary>
				/// Вертає поточний стан задачі
				/// </summary>
				/// <returns>Поточний стан задачі</returns>
				ForceInline EThreadabledStates GetCurrentState() { return This->State; }
				/// <summary>
				/// Перериває задачу
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				SeedKernelCall JobPool& Interupt();
				/// <summary>
				/// Очікує завершення задачі
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				SeedKernelCall JobPool& Join();
				/// <summary>
				/// Замороження задачі(потоку який виконує задачу)
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				SeedKernelCall JobPool& Suspend();
				/// <summary>
				/// Розмороження задачі(потоку який виконує задачу)
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				SeedKernelCall JobPool& Resume();
				/// <summary>
				/// Очікує завершення задачі
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns>Посилання на об'єкт</returns>
				SeedKernelCall JobPool& Join(Time::DateTime timeOut);
				/// <summary>
				/// Вказує чи задача існує
				/// </summary>
				/// <returns>true якщо задача валідна</returns>
				ForceInline Boolean IsValid() { return This != nullptr; }
			public:
				/// <summary>
				/// Вертає ім'я задачі
				/// </summary>
				/// <returns>Рядкове представлення задачі</returns>
				ForceInline const String& GetName() { return This->Name; }
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="NewName">Нове ім'я задачі</param>
				ForceInline void SetName(const String& NewName);
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="NewName">Нове ім'я задачі</param>
				ForceInline void SetName(String&& NewName) { This->Name = MoveRValue(NewName); }
			public:
				/// <summary>
				/// Ім'я задачі
				/// </summary>
				String Property(GetName, SetName) Name;
			public:
				RefCountClass(JobPool, This)
			public:
				friend class ThreadPool;
			};

			/// <summary>
			/// Запуск іменованої задачі в пул потоків
			/// </summary>
			/// <param name="Name">Ім'я задачі</param>
			/// <param name="delegate">Дія</param>
			/// <returns>Об'єкт задачі</returns>
			ForceInline JobPool ThreadPool::Dispatch(const String& Name, Delegate&& delegate)
			{
				auto obj = JobPool(MoveRValue(delegate), Name, *this);
				obj.Dispatch();
				return obj;
			}

			/// <summary>
			/// Запуск іменованої задачі в пул потоків
			/// </summary>
			/// <param name="Name">Ім'я задачі</param>
			/// <param name="delegate">Дія</param>
			/// <returns>Об'єкт задачі</returns>
			ForceInline JobPool ThreadPool::Dispatch(String&& Name, Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), MoveRValue(Name), *this);
				obj.Dispatch();
				return obj;
			}

			/// <summary>
			/// Запуск безіменної задачі в пул потоків
			/// </summary>
			/// <param name="delegate">Дія</param>
			/// <returns>Об'єкт задачі</returns>
			ForceInline JobPool ThreadPool::Dispatch(Delegate&& delegate) {
				auto obj = JobPool(MoveRValue(delegate), L"Unnamed job", *this);
				obj.Dispatch();
				return obj;
			}
		}
	}
}