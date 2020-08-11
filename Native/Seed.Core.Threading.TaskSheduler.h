#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.DateTime.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			interface ITask;
			class SheduledJob;
			class SheduledJobBuilder;
			class TaskSheduler;

			interface ITask
			{
				void Execute(DateTime startTimeUtc) volatile;
			};

			/// <summary>
			/// Планувальник задач на базі пулу потоків(виконує задачі в задану дату і час)
			/// </summary>
			class TaskSheduler final : protected IDisposable {
				struct Data {
					///<summary>Керуюча структура пула потоків</summary>
					ImplSpecificT pool;
					///<summary>Настройки середовищп для пула потоків</summary>
					ImplSpecificT env;
					///<summary>Настройка очищення задач</summary>
					ImplSpecificT cleanupData;
					///<summary>Лічильник силок</summary>
					RefCountT RefCount;
					///<summary>Кількість задач у пулі</summary>
					RefCountT TaskCount;
					///<summary>Ім'я пула задач</summary>
					String Name;
					/// <summary>
					/// Блокировка для атомарності
					/// </summary>
					SRWLock lock;
					/// <summary>
					/// Флаг який вказує чи відбувається деструкція пул задач
					/// </summary>
					Boolean IsTerminating;

					/// <summary>
					/// Конструктор з іменем планувальника задач
					/// </summary>
					/// <param name="Name">Ім'я планувальника задач</param>
					ForceInline Data(const String& Name) :
						Name(Name),
						pool(nullptr),
						env(nullptr),
						cleanupData(nullptr),
						IsTerminating(false),
						lock(),
						RefCount(1),
						TaskCount(0)
					{}

					/// <summary>
					/// Конструктор з іменем планувальника задач
					/// </summary>
					/// <param name="Name">Ім'я планувальника задач</param>
					ForceInline Data(String&& Name) :
						Name(MoveRValue(Name)),
						pool(nullptr),
						env(nullptr),
						cleanupData(nullptr),
						IsTerminating(false),
						lock(),
						RefCount(1),
						TaskCount(0)
					{}
				};
			private:
				Data* This;
			private:
				/// <summary>
				/// Ініціалізація панувальник задач на базі пулу потоків
				/// </summary>
				SeedKernelCall void InitTaskSheduler();
				/// <summary>
				/// Конструктор планувальник задач на базі пулу потоків
				/// </summary>
				/// <param name="This">Дані даного планувальника задач</param>
				ForceInline TaskSheduler(Data* This) :This(This) { ++This->RefCount; }
			private:
				/// <summary>
				/// Вертає блокировку пулу до якого відноситься задача
				/// </summary>
				/// <param name="thread">Поінтер на задачу</param>
				/// <returns>Об'єкт блокировки</returns>
				friend SRWLock GetPoolLock(SheduledJob* thread);
				/// <summary>
				/// Вертає true якщо пул впроцесі знищення
				/// </summary>
				/// <param name="thread">Поінтер на задачу</param>
				/// <returns>True якщо пул впроцесі знищення</returns>
				friend Boolean IsPoolTerminating(SheduledJob* thread);
			protected:
				/// <summary>
				/// Знищення пулу
				/// </summary>
				ForceInline virtual void Dispose() override final;
			public:
				/// <summary>
				/// Створення безіменного планувальник задач на базі пулу потоків
				/// </summary>
				ForceInline TaskSheduler() : This(new Data("Unnamed Task Sheduler")) { InitTaskSheduler(); }
				/// <summary>
				/// Створення іменого планувальник задач на базі пулу потоків
				/// </summary>
				/// <param name="Name">Ім'я пулу</param>
				ForceInline TaskSheduler(const String& Name) : This(new Data(Name)) { InitTaskSheduler(); }
				/// <summary>
				/// Створення іменого планувальник задач на базі пулу потоків
				/// </summary>
				/// <param name="Name">Ім'я пулу</param>
				ForceInline TaskSheduler(String&& Name) : This(new Data(MoveRValue(Name))) { InitTaskSheduler(); }
				/// <summary>
				/// Передача задачі в планувальник задач на базі пулу потоків
				/// </summary>
				/// <param name="task">Завдання яке передається</param>
				/// <returns>Об'єкт задачі в пулі потоків</returns>
				SeedKernelCall [[nodiscard]] SheduledJobBuilder NewTask(ITask*&& task);

				/// <summary>
				/// Кількість зареєстрованих задач(включно з тими шо в черзі але без тих які вже виконались)
				/// </summary>
				/// <returns>Кількість задач</returns>
				ForceInline SizeT GetTaskCount() { return this->This->TaskCount; }
				/// <summary>
				/// Кількість зареєстрованих задач(включно з тими шо в черзі але без тих які вже виконались)
				/// </summary>
				SizeT Property(GetTaskCount) TaskCount;
			public:
				RefCountClass(TaskSheduler, This)
			public:
				friend class SheduledJob;
				friend class SheduledJobBuilder;
			};

			/// <summary>
			/// Задача в планувальнику задач
			/// </summary>
			class SheduledJob final : IDisposable {
			private:
				/// <summary>
				/// Внутрішня структура задачі
				/// </summary>
				struct Data {
					/// <summary>
					/// Тривалість періоду в 100 наносекундних інтервалах
					/// </summary>
					volatile Int32 PeriodLenght;

					/// <summary>
					/// Мінімальна прийнятна точність таймера в 100 наносекундних інтервалах
					/// </summary>
					volatile Int32 TimerProximity;

					/// <summary>
					/// Кількість періодів
					/// </summary>
					volatile Int32 PeriodsCount;

					/// <summary>
					/// Поінтер на задачу
					/// </summary>
					volatile ITask* Task;

					/// <summary>
					/// Дескриптор задачі
					/// </summary>
					volatile Handle hTask;

					/// <summary>
					/// Останій час запуску
					/// </summary>
					volatile DateTime LastStartTime;

					/// <summary>
					/// Час запуску
					/// </summary>
					volatile DateTime StartTime;

					/// <summary>
					/// Стан потоку задачі
					/// </summary>
					volatile EThreadabledStates State;

					/// <summary>
					/// ID потоку в якому задача виконується
					/// </summary>
					volatile ID TID;

					/// <summary>
					/// Ітератор на задачу в списку потокових об'єктів
					/// </summary>
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					/// <summary>
					/// Поінтер на пул задач
					/// </summary>
					TaskSheduler::Data* sheduler;

					/// <summary>
					/// Дескриптор потоку в якому задача виконується(під час виконання)
					/// </summary>
					Handle hThread;

					/// <summary>
					/// Ім'я задачі
					/// </summary>
					String Name;

					/// <summary>
					/// Кількість виконаннь
					/// </summary>
					RefCountT ExucutedCount;
					/// <summary>
					/// Кількість виконаннь які ще потрібно зробити
					/// </summary>
					RefCountT RemainingPeriodsCount;
					/// <summary>
					/// Кількість силок на об'єкт
					/// </summary>
					RefCountT RefCount;

					/// <summary>
					/// Конструктор внутрішнього об'єкту задачі
					/// </summary>
					/// <param name="Task">Поінтер на задачу</param>
					/// <param name="sheduler">Поінтер на дані планувальника</param>
					ForceInline Data(ITask* Task, TaskSheduler::Data* sheduler) :
						Task(Task),
						PeriodLenght(0),
						TimerProximity(0),
						PeriodsCount(1),
						hTask(nullptr),
						LastStartTime(0),
						StartTime(0),
						State(EThreadabledStates::NotStarted),
						TID(0),
						sheduler(sheduler),
						hThread(nullptr),
						Name(L"Unnamed Task"),
						ExucutedCount(0),
						RemainingPeriodsCount(0),
						RefCount(1),
						This()
					{}
				};
			private:
				Data* This;
			private:
				/// <summary>
				/// Конструктор задачі
				/// </summary>
				/// <param name="This">Поінтер на дані задачі</param>
				ForceInline SheduledJob(Data* This) : This(This) { ++This->RefCount; }
			private:
				friend void RegisterThread(SheduledJob* thread);
				friend void UnRegisterThread(SheduledJob* thread);
				friend volatile ITask& GetJobEntryPoint(SheduledJob* thread);
				friend SRWLock GetPoolLock(SheduledJob* thread);
				friend Boolean IsPoolTerminating(SheduledJob* thread);
			protected:
				/// <summary>
				/// Знищення об'єкта задачі
				/// </summary>
				virtual void Dispose() override final;
			public:
				/// <summary>
				/// Вертає пул задачі в якому зареєстрована дана задача
				/// </summary>
				/// <returns>Об'єкт планувальника задач</returns>
				ForceInline TaskSheduler& GetThreadPool() { return TaskSheduler(This->sheduler); }
				/// <summary>
				/// Стан задачі
				/// </summary>
				/// <returns>Стан задачі</returns>
				ForceInline EThreadabledStates GetCurrentState() { return This->State; }
				/// <summary>
				/// Переривання виконання задачі
				/// </summary>
				/// <returns>Силку на поточний об'єкт</returns>
				SeedKernelCall SheduledJob& Interupt();
				/// <summary>
				/// Очікування на завершення задачі
				/// </summary>
				/// <returns>Силку на поточний об'єкт</returns>
				SeedKernelCall SheduledJob& Join();
				/// <summary>
				/// Призупинити виконання задачі
				/// </summary>
				/// <returns>Силку на поточний об'єкт</returns>
				SeedKernelCall SheduledJob& Suspend();
				/// <summary>
				/// Очікування на завершення задачі
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns>Силку на поточний об'єкт</returns>
				SeedKernelCall SheduledJob& Join(DateTime timeOut);
			public:
				/// <summary>
				/// Ім'я задачі
				/// </summary>
				/// <returns></returns>
				ForceInline const String& GetName() { return This->Name; }
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="NewName">Нове ім'я задачі</param>
				ForceInline void SetName(const String& newName) { This->Name = newName; }
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="NewName">Нове ім'я задачі</param>
				ForceInline void SetName(String&& newName) { This->Name = MoveRValue(newName); }
				/// <summary>
				/// Ім'я задачі
				/// </summary>
				String Property(GetName, SetName) Name;
			public:
				RefCountClass(SheduledJob, This)
			public:
				friend class SheduledJobBuilder;
				friend class ThreadableSheduledTask;
			};

			/// <summary>
			/// Проміжний клас для побудови задачі
			/// </summary>
			class SheduledJobBuilder final {
			private:
				/// <summary>
				/// Дані задачі
				/// </summary>
				SheduledJob::Data* This;
				/// <summary>
				/// Флаг відносності часу, якщо true то час відносний відносно моменту запуску
				/// </summary>
				Boolean IsExcecutionTimeRelative;
			private:
				/// <summary>
				/// Конструктор будівельника задачі
				/// </summary>
				/// <param name="sheduler">Планувальник потоків</param>
				/// <param name="task">Задача</param>
				ForceInline SheduledJobBuilder(TaskSheduler& sheduler, ITask*&& task) : This(new SheduledJob::Data(MoveRValue(task), sheduler.This)), IsExcecutionTimeRelative(false) {}
			public:
				ForceInline SheduledJobBuilder(const SheduledJobBuilder&) = delete;
				/// <summary>
				/// Конструктор переміщення
				/// </summary>
				/// <param name="orig">Оригинал об'єкту</param>
				ForceInline SheduledJobBuilder(SheduledJobBuilder&& orig) : This(orig.This), IsExcecutionTimeRelative(orig.IsExcecutionTimeRelative) { orig.This = nullptr; }
				
				SheduledJobBuilder& operator=(const SheduledJobBuilder&) = delete;
				/// <summary>
				/// Оператор переміщення
				/// </summary>
				/// <param name="orig">Оригинал об'єкту</param>
				/// <returns>Силка на об'єкт</returns>
				ForceInline SheduledJobBuilder& operator=(SheduledJobBuilder&& orig) { Swap(this->This, orig.This); return *this; }
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline ~SheduledJobBuilder()
				{
					if (This != nullptr) {
						if (--This->RefCount == 0) {
							if (This->Task != nullptr) { //-V809
								delete This->Task;
							}
							delete This;
						}
						This = nullptr;
					}
				}
			public:
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="Name">Нове ім'я задачі</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetName(const String& Name) { This->Name = Name; return *this; }
				/// <summary>
				/// Встановлення імені задачі
				/// </summary>
				/// <param name="Name">Нове ім'я задачі</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetName(String&& Name) { This->Name = MoveRValue(Name); return *this; }
				/// <summary>
				/// Встановлення часу виконання
				/// </summary>
				/// <param name="time">Час виконання</param>
				/// <param name="isExcecutionTimeRelative">Флаг відносності часу, якщо true то час відносний відносно моменту запуску</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetExecutionTime(DateTime time, Boolean isExcecutionTimeRelative = false) {
					This->StartTime = time; 
					this->IsExcecutionTimeRelative = isExcecutionTimeRelative; 
					return *this;
				}
				/// <summary>
				/// Встановлення часу циклу виконання задачі
				/// </summary>
				/// <param name="time">Тривалість циклу</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetLoopDuration(DateTime time) {
					This->PeriodLenght = (Int32) time.TimeFromStartEpoch;
					return *this;
				}
				/// <summary>
				/// Встановлення кількості циклів виконання задачі
				/// </summary>
				/// <param name="count">Кількість циклів виконання задачі</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetLoopsCount(Int32 count) {
					This->PeriodsCount = count; 
					This->RemainingPeriodsCount = count; 
					return *this;
				}
				/// <summary>
				/// Встановлення точності таймера для задачі
				/// </summary>
				/// <param name="proximity">Точність таймера для задачі</param>
				/// <returns>Силку на поточний об'єкт</returns>
				ForceInline SheduledJobBuilder& SetTimerProximity(DateTime proximity) { This->TimerProximity = (Int32)proximity.TimeFromStartEpoch; return *this; }
				/// <summary>
				/// Запуск задачі
				/// </summary>
				/// <returns></returns>
				SeedKernelCall [[maybe_unused]] SheduledJob Shedule();
				/// <summary>
				/// Скасування задачі
				/// </summary>
				ForceInline void Cancel() { this->~SheduledJobBuilder(); }

			public:
				friend class TaskSheduler;
			};

			ForceInline SheduledJobBuilder TaskSheduler::NewTask(ITask*&& task)
			{
				return SheduledJobBuilder(*this, MoveRValue(task));
			}
		}
	}
}