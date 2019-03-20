#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Delegate.h"
#include "Seed.Core.Security.h"

namespace Seed {
	namespace Exeptions {
		//Неіснуючий потік або він не є керований через Core
		class InvalidThread :public Seed::Exception
		{
		private:
			SizeT TID;
		public:
			//Неіснуючий потік або він не є керований через Core
			__forceinline InvalidThread(const AChar * File, const AChar * FunctName, SizeT Line, SizeT TID)
				noexcept : Exception(File, FunctName, Line, L"Cannot manage non managed thread"), TID(TID)
			{};
			//ID потоку який згенерував помилку
			__forceinline SizeT GetTargetTID()
			{
				return TID;
			}
		};
		//Не валідний об'єкт потоку
		class InvalidThreadObj :public Seed::Exception
		{
		public:
			//Не валідний об'єкт потоку
			__forceinline InvalidThreadObj(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Cannot manage non managed thread")
			{};
		};
		//Генерується при спробі запуску потоку якщо він вже запущений
		class ThreadAlredyStrated :public Seed::Exception
		{
		public:
			//Спроба запуску потоку якщо він вже запущений
			__forceinline ThreadAlredyStrated(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Thread alredy started")
			{};
		};
		//Генерується при спробі запуску основного потоку якщо він вже запущений
		class MainThreadAlredyStrated :public Seed::Exception
		{
		public:
			//Спроба запуску основного потоку якщо він вже запущений
			__forceinline MainThreadAlredyStrated(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Main thread alredy started")
			{};
		};
		//Генерується при спробі зробити не контрольоване бескінченого блокування
		class DedlockExpected :public Seed::Exception
		{
		public:
			//Спроба зробити бескінчене не контрольоване блокування
			__forceinline DedlockExpected(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Dedlock cathed")
			{};
		};
		//Генерується при переривані очікування на nt event
		class AlertByEvent :public Seed::Exception
		{
		public:
			//Спроба зробити бескінчене не контрольоване блокування
			__forceinline AlertByEvent(const AChar * File, const AChar * FunctName, SizeT Line)
				noexcept : Exception(File, FunctName, Line, L"Allerted by event")
			{};
		};
		//Генерується при перериванні потоку
		class InteruptExeption
		{
		public:
			//Прохання перервати потік
			__forceinline InteruptExeption() noexcept
			{};
		};
	}
	inline namespace Core {
		namespace Security {
			namespace Thread {
				enum class AccessRight
				{
					Read,
					Suspend,
					Interupt,
					ExecAPC,
					Terminate,
					MaxRight
				};
				constexpr auto GenericReadAccess = [] () {
					BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
					mask.SetBit (static_cast<SizeT>(AccessRight::Read));
					return mask;
				};
				constexpr auto GenericWriteAccess = [] () {
					BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
					mask.SetBit (static_cast<SizeT>(AccessRight::Suspend));
					return mask;
				};
				constexpr auto GenericExecuteAccess = [] () {
					BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
					mask.SetBit (static_cast<SizeT>(AccessRight::Interupt));
					mask.SetBit (static_cast<SizeT>(AccessRight::Terminate));
					return mask;
				};
				constexpr auto GenericDeleteAccess = [] () {
					BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
					mask.SetBit (static_cast<SizeT>(AccessRight::ExecAPC));
					return mask;
				};
			}
			using ThreadSecurityDescriptor = Seed::Core::Security::GenericSecurityDescriptor<Thread::AccessRight,
				Thread::GenericReadAccess, Thread::GenericWriteAccess, Thread::GenericExecuteAccess, Thread::GenericDeleteAccess
			>;
		}
		//Потік
		class SeedKernelCall Thread : private Security::ThreadSecurityDescriptor
		{
		private:
			//Список всіх керованих потоків
#pragma warning(push)
#pragma warning(disable: 4251)
			static List<Thread> * Threads;
#pragma warning(pop)
			//Головний потік
			static Thread * MainThread;
		public:
			//Подія
			//Об'єкт синхронізації потоків
			class SeedKernelCall Event : private IDisposable
			{
				//Дані події
				struct event_data
				{
					//Дескриптор
					void * Handle;
					//Кількість силок на об'єкт
					RefCountT RefCount;
					//Ім'я обєкта події в об'єктній системі NT
					String Name;
					//Конструктор з отриманням дескриптора
					event_data(void * handle, const String & name) :Handle(handle), RefCount(1), Name(name)
					{};
					//Конструктор з оптимізована отриманням дескриптора
					event_data(void * handle, String && name) :Handle(handle), RefCount(1), Name(name)
					{};
				};
				//Дані об'єкту події
				event_data * data;
			public:
				RefCountClass (Event, data);
				//Конструктор
				Event(Boolean IsManualReset = false) noexcept;
				//Конструктор з іменем об'єкта
				Event(const String & name, Boolean IsManualReset = false) noexcept;
				//Конструктор з оптимізованою передачею імені об'єкта
				Event(String && name, Boolean IsManualReset = false) noexcept;

				//Очікування на подію
				void Wait();
				//Очікування з таймаутом
				void Wait(SizeT TimeOutMs);

				//Подати імпульс на подію
				void Signale();
				//Виставити true на подію 
				void Set();
				//Виставити false на подію
				void Reset();

				//Поточна кількість об'єктів подій
				static SizeT EventObjCount();

				//Знищення останнього елемента
				virtual void Dispose () override final;
			};

			//Блокувальник для багатопотокових об'єктів
			// Принцип наступний, дозволяє дочекатись 
			// завершення всіх операції запущених в блоці Guard
			// методом WaitFree
			// Також методом IsFree можна провірити чи зараз відбувається виконання
			// А методом GetGuardedScopesCount отримати кількість поточних операцій,
			// виконуються в блоці Guard
			class SeedKernelCall GuardLock : IDisposable
			{
			private:
			#pragma pack(push, 8)

				typedef struct _RTL_CRITICAL_SECTION
				{
					void* DebugInfo;

					//
					//  The following three fields control entering and exiting the critical
					//  section for the resource
					//

					long LockCount;
					long RecursionCount;
					Handle OwningThread;        // from the thread's ClientId->UniqueThread
					Handle LockSemaphore;
					SizeT SpinCount;        // force size on 64-bit systems when packed
				} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

			#pragma pack(pop)
			private:
				//Дані блокувальника
				struct guardlock_data
				{
					//Дескриптор події
					Handle EventHandle;
					//Дескриптор критичної секції
					RTL_CRITICAL_SECTION CriticalSection;
					//Кількість активних блокуваннь
					RefCountT GuardsCount;
					//Кількість силок на об'єкт
					RefCountT RefCount;
					//Конструктор з отриманням дескриптора
					guardlock_data(void * EventHandle, RTL_CRITICAL_SECTION CriticalSection) :
						EventHandle(EventHandle),
						CriticalSection(CriticalSection),
						RefCount(1),
						GuardsCount(0)
					{};
				};
				//Дані об'єкту блокуваннь
				guardlock_data * data;
			public:
				//Конструктор створює об'єкт, ініціалізовуючи внутрішню структуру guardlock_data
				GuardLock() noexcept;

				RefCountClass (GuardLock, data);

				//Блокує WaitFree для чого використовується об'єкт події з NT Native
				//Для атомарності операції використовується критична секція
				//Об'єкти даного класу використовують підрахунок силок для ефективного
				//копіювання та переміщення
				template<typename lambda>
				__forceinline auto Guard(lambda scope)
				{
					extern void EnterCriticalSection(void * CriticalSection);
					extern void LeaveCriticalSection(void * CriticalSection);
					extern void SetNtEvent(void * handle);
					extern void ResetNtEvent(void * handle);

					EnterCriticalSection(&this->data->CriticalSection);
					ResetNtEvent(this->data->EventHandle, NULL);
					this->data->GuardsCount++;
					LeaveCriticalSection(&this->data->CriticalSection);

					try
					{
						if constexpr (Seed::Core::StaticAsserrts::IsSame<decltype(scope()), void>())
						{
							scope();

							EnterCriticalSection(&this->data->CriticalSection);
							if (--data->GuardsCount == 0)
								SetNtEvent(this->data->EventHandle);
							LeaveCriticalSection(&this->data->CriticalSection);
						}
						else
						{
							auto && result = MoveRValue(scope());

							EnterCriticalSection(&this->data->CriticalSection);
							if (--data->GuardsCount == 0)
								NtSetEvent(this->data->EventHandle, NULL);
							LeaveCriticalSection(&this->data->CriticalSection);

							return result;
						}
					}
					catch (Exception * ex)
					{
						EnterCriticalSection(&this->data->CriticalSection);
						if (--data->GuardsCount == 0)
							NtSetEvent(this->data->EventHandle, NULL);
						LeaveCriticalSection(&this->data->CriticalSection);
						Throw(ex);
					}
					catch (InteruptExeption * ex)
					{
						EnterCriticalSection(&this->data->CriticalSection);
						if (--data->GuardsCount == 0)
							NtSetEvent(this->data->EventHandle, NULL);
						LeaveCriticalSection(&this->data->CriticalSection);
						Throw(ex);
					}
				}

				//Очікує завершення всіх блоків запущених в блоці Guard
				__forceinline void WaitFree()
				{
					//TODO: NtWaitForSingleObject(data->EventHandle, 1, nullptr);
				}

				//Вертає true якщо немає блоків в блоці Guard
				__forceinline Boolean IsFree()
				{
					return data->GuardsCount == 0;
				}
				//Вертає кількість блоків запущених в блоці Guard
				__forceinline SizeT GetGuardedScopesCount() noexcept
				{
					return data->GuardsCount;
				}

				// Inherited via IDisposable
				virtual void Dispose () override;
			};

			//Об'єкт критичної секції
			//Метод ExecuteInCriticalSection дозволяє виконати лямбду l 
			//автоматично ввійшовши в критичну секцію і вийшовши з неї по завершенню виконання лямбди
			class SeedKernelCall CriticalSection
			{
			private:
#pragma pack(push, 8)

				typedef struct _RTL_CRITICAL_SECTION
				{
					void* DebugInfo;

					//
					//  The following three fields control entering and exiting the critical
					//  section for the resource
					//

					long LockCount;
					long RecursionCount;
					Handle OwningThread;        // from the thread's ClientId->UniqueThread
					Handle LockSemaphore;
					SizeT SpinCount;        // force size on 64-bit systems when packed
				} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

#pragma pack(pop)
				//Внутрішня структура даних
				struct __section
				{
					//Критична секція
					RTL_CRITICAL_SECTION section;
					//Кількість силок на даний екзмемпляр
					RefCountT RefCount;
				};
				//Поле цього екземпляра класу
				__section * This;
			public:
				//Ініціалізовує нову критичну секцію
				__forceinline CriticalSection()
				{
					This = new __section;
					//TODO: RtlInitializeCriticalSection(&This->section);
					This->RefCount = 1;
				}
				//Створює копію секції збільшуючи кількість силок на даний екземаляр
				__forceinline CriticalSection(const CriticalSection & orig) noexcept :This(orig.This)
				{
					++This->RefCount;
				}
				//Переміщає покажчик в новий екзмемпляр, а в старому обнулює
				__forceinline CriticalSection(CriticalSection && from) noexcept :This(from.This)
				{
					from.This = nullptr;
				}
				//Оператор присвоєння, знищує поточний об'єкт і створює копію задопомогою конструктор
				__forceinline CriticalSection operator=(const CriticalSection & orig) noexcept
				{
					this->~CriticalSection();
					this->CriticalSection::CriticalSection(orig);
					return *this;
				}
				//Оператор переміщення, знищує поточний об'єкт викликом деструктора
				//І переміщує задопомогою конструктор переміщення критичну секцію в новий об'єкт
				__forceinline CriticalSection operator=(CriticalSection && orig) noexcept
				{
					this->~CriticalSection();
					this->CriticalSection::CriticalSection(MoveRValue(orig));
					return *this;
				}

				//Виконує лямбду l 
				//автоматично ввійшовши в критичну секцію перед виконанням і 
				//вийшовши з неї після виконання
				template<typename lambda>
				__forceinline auto ExecuteInCriticalSection(lambda l)
				{
					RtlEnterCriticalSection(&This->section);
					try
					{
						if constexpr (Seed::CoreAPI::StaticAsserrts::IsSame<decltype(l()), void>())
						{
							l();
							RtlLeaveCriticalSection(&This->section);
						}
						else
						{
							auto res = l();
							RtlLeaveCriticalSection(&This->section);
							return res;
						}
					}
					catch (Exeption * ex)
					{
						RtlLeaveCriticalSection(&This->section);
						throw ex;
					}
					catch (InteruptExeption * ex)
					{
						RtlLeaveCriticalSection(&This->section);
						throw ex;
					}
				}

				//Деструктор перевіряє чи цей об'єкт нульовий
				//Якщо не нульовий то декрементить лічильник силок
				//Якщо силок лишилось 0 то знищує секцію і видаляє виділену пам'ять
				__forceinline ~CriticalSection() noexcept
				{
					if (This != nullptr)
					{
						if (--This->RefCount == 0)
						{
							//TODO: RtlDeleteCriticalSection(&This->section);
							delete This;
						}
					}
				}
			};

			//Стан потоку
			enum class ThreadState :Seed::UInt8
			{
				NotStarted = 0,
				Work = 1,
				Terminated = 2,
				Killed = 3,
				ClosedByExeption = 4,
				CorrectlyEnded = 5
			};
			//Дані потоку
			struct ThreadData
			{
				//Стан потоку
				ThreadState state;
				//Ім'я потоку
				String Label;
				//Дескриптор потоку
				void * Handle;
				//TID потоку
				SizeT ID;
				//Подія завершення
				Event * abortEvent;
				//Кількість силок на об'єкт
				RefCountT RefCount;
				//Силка на себе в списку потоків
				List<Thread>::Iterator thread_list_node;

				//Пустий конструктор
				__forceinline ThreadData() noexcept :
					state(ThreadState::NotStarted), Label(), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
				{};
				//Конструктор з імен потоку
				__forceinline ThreadData(const String & label) :
					state(ThreadState::NotStarted), Label(label), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
				{};
				//Конструктор з оптимізованою передачею імені потока
				__forceinline ThreadData(String && label) :
					state(ThreadState::NotStarted), Label(MoveRValue(label)), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
				{};

				~ThreadData()
				{
					Label.~BaseString();
					thread_list_node.~Iterator();
				}
			};
		private:
			//Дані потоку
			ThreadData * thread;
			//Реалізація запуску стартового потоку
			static Thread & InitMainThreadObj(Delegate && ManagedEntryPoint);


			//Ініціалізація підсистеми потоків
			extern friend bool InitCoreThreading() noexcept;
			//Зупинка підсистеми потоків
			extern friend void StopCoreThreading() noexcept;
		public:
			//Вертає силку на поточний потік
			static Thread GetThis();
			//Вертає силку по ID потоку
			static Thread GetByID(UInt64 TargetTID);
			//Вертає ID потоку
			static ID GetID();
			//Призупинити виконання потоку на час ms вказаний в мілісекундах
			static void Sleep(UInt64 ms);
			//Призупинення потоку навічно(використовувати лише якщо потік сон переривається ззовні)
			static void Sleep();
			//Очікувати на завершення потоку по ID, таймаут вказуєтья в мілісекундах
			//якщо не вказано то без таймаута
			static void Wait(UInt64 TargetTID, SizeT Timeout = 0);
			//Очікувати на завершення потоку силці на потік, таймаут вказуєтья в мілісекундах
			//якщо не вказано то без таймаута
			static void Wait(Thread &thread, SizeT Timeout = 0);
			//Очікувати на завершення головного потоку
			__forceinline static void WaitOnMainThread(SizeT Timeout = 0)
			{
				Wait(*MainThread, Timeout);
			}
			//Отримати список потоків по імені
			static List<Thread> GetByLabel(const String & Label);
			//Отримати список всіх потоків
			static List<Thread> GetAllThreads()
			{
				return *Threads;
			}
			//Запуск головного потоку
			template<typename F, typename ... Args>
			static Thread & InitMainThread(F f, Args ... args)
			{
				return InitMainThreadObj(CreateAndPackFunctionObj(f, args));
			}
			//Переривання роботи потоку, аргументом є ідентифікатор потоку
			static Boolean Interupt(SizeT ThreadID);
		public:
			//Конструктор
			Thread();
			//Конструктор з імена потоку
			Thread(const String & Label);
			//Конструктор з оптимізованою передачею імені
			Thread(String && Label);
			//Копіювальний конструктор
			__forceinline Thread(const Thread & orig) :thread(orig.thread)
			{
				this->thread->RefCount++;
			}
			//Конструктор переміщення
			__forceinline Thread(Thread && orig) noexcept
			{
				this->thread = orig.thread;
				orig.thread = nullptr;
			}
			//Оператор копіювання
			__forceinline Thread & operator=(const Thread & orig)
			{
				this->~Thread();
				this->thread = orig.thread;
				this->thread->RefCount++;
				return *this;
			}
			//Оператор переміщення
			__forceinline Thread & operator=(Thread && orig) noexcept
			{
				this->~Thread();
				this->thread = orig.thread;
				orig.thread = nullptr;
				return *this;
			}
			//Зміна імені потоку
			__forceinline void ChangeLabel(const String & NewLabel)
			{
				this->thread->Label = NewLabel;
			}
			//Вертає ім'я потоку
			const String & GetLabel() const
			{
				return this->thread->Label;
			}

			//Запуск потоку, аргументом є делегат який містить функцію з її аргументами
			void ExecDelegate(Delegate && ThreadMain);

			//Запуск потоку, аргументами є функція і її аргументи
			template<typename F, typename ... Args>
			Thread & ExecFunct(F f, Args& ... args)
			{
				ExecDelegate(CreateAndPackFunctionObj(f, args...));
				return *this;
			}

			//Переривання сну потоку
			void Alert();

			//Виставлення функції в APC чергу потоку
			void DispatchAPC(Delegate && APCDelegate);

			//Примусове завершення потоку, використовувати в рідкісних випадках
			//бо не відбувається видалення об'єктів створених потоком
			void Kill();

			//Провірка чи потік запущений
			__forceinline bool isWork()
			{
				return thread->ID != -1;
			}

			//Переривання роботи потоку
			Thread & Interupt();

			//Стан об'єкту потоку
			ThreadState State()
			{
				return thread->state;
			}

			//Очікувати на завершення потоку
			void Join(SizeT Timeout = 0) const;

			//Вертає ID потоку
			__forceinline ID GetThisThreadID() const
			{
				return thread->ID;
			}

			//Оператор перетворення вертає ID потоку
			__forceinline operator SizeT() const
			{
				return thread->ID;
			}
			//Оператор перетворення вертає true яко потік запущений, false якщо ні
			__forceinline operator Boolean() const
			{
				return thread->ID != -1;
			}

			//Деструктор об'єкта потоку
			~Thread();

			//Функція обміну для об'єктів потоків
			friend void Seed::Swap(Seed::Core::Thread & a, Seed::Core::Thread & b) noexcept;
		};
	}
	//Оптимізований обмін об'єктів потоку
	template<>
	__forceinline void Swap(Seed::Core::Thread & a, Seed::Core::Thread & b) noexcept
	{
		Swap(a.thread, b.thread);
	}
}
