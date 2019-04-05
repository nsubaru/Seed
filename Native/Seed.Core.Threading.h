//#pragma once
//#include "Seed.Core.Base.h"
//#include "Seed.Core.Memory.h"
//#include "Seed.Core.String.h"
//#include "Seed.Core.Exceptions.h"
//#include "Seed.Core.Containers.Delegate.h"
//#include "Seed.Core.Security.h"
//#include "Seed.Core.Threading.SRWLock.h"
//
//namespace Seed {
//	namespace Exeptions {
//		//Неіснуючий потік або він не є керований через Core
//		class InvalidThread :public Seed::Exception
//		{
//		private:
//			SizeT TID;
//		public:
//			//Неіснуючий потік або він не є керований через Core
//			__forceinline InvalidThread(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT TID)
//				noexcept : Exception(File, FunctName, Line, L"Cannot manage non managed thread"), TID(TID)
//			{};
//			//ID потоку який згенерував помилку
//			__forceinline SizeT GetTargetTID()
//			{
//				return TID;
//			}
//		};
//		//Не валідний об'єкт потоку
//		class InvalidThreadObj :public Seed::Exception
//		{
//		public:
//			//Не валідний об'єкт потоку
//			__forceinline InvalidThreadObj(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
//				noexcept : Exception(File, FunctName, Line, L"Cannot manage non managed thread")
//			{};
//		};
//		//Генерується при спробі запуску потоку якщо він вже запущений
//		class ThreadAlredyStrated :public Seed::Exception
//		{
//		public:
//			//Спроба запуску потоку якщо він вже запущений
//			__forceinline ThreadAlredyStrated(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
//				noexcept : Exception(File, FunctName, Line, L"Thread alredy started")
//			{};
//		};
//		//Генерується при спробі запуску основного потоку якщо він вже запущений
//		class MainThreadAlredyStrated :public Seed::Exception
//		{
//		public:
//			//Спроба запуску основного потоку якщо він вже запущений
//			__forceinline MainThreadAlredyStrated(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
//				noexcept : Exception(File, FunctName, Line, L"Main thread alredy started")
//			{};
//		};
//		//Генерується при спробі зробити не контрольоване бескінченого блокування
//		class DedlockExpected :public Seed::Exception
//		{
//		public:
//			//Спроба зробити бескінчене не контрольоване блокування
//			__forceinline DedlockExpected(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
//				noexcept : Exception(File, FunctName, Line, L"Dedlock cathed")
//			{};
//		};
//		//Генерується при переривані очікування на nt event
//		class AlertByEvent :public Seed::Exception
//		{
//		public:
//			//Генерується при переривані очікування на nt event
//			__forceinline AlertByEvent(const AChar * File, const AChar * FunctName, SizeT Line)
//				noexcept : Exception(File, FunctName, Line, L"Allerted by event")
//			{};
//		};
//	}
//	inline namespace Core {
//		//namespace Security {
//		//	namespace Thread {
//		//		enum class AccessRight
//		//		{
//		//			Read,
//		//			Suspend,
//		//			Interupt,
//		//			ExecAPC,
//		//			Terminate,
//		//			MaxRight
//		//		};
//		//		constexpr auto GenericReadAccess = [] () {
//		//			BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
//		//			mask.SetBit (static_cast<SizeT>(AccessRight::Read));
//		//			return mask;
//		//		};
//		//		constexpr auto GenericWriteAccess = [] () {
//		//			BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
//		//			mask.SetBit (static_cast<SizeT>(AccessRight::Suspend));
//		//			return mask;
//		//		};
//		//		constexpr auto GenericExecuteAccess = [] () {
//		//			BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
//		//			mask.SetBit (static_cast<SizeT>(AccessRight::Interupt));
//		//			mask.SetBit (static_cast<SizeT>(AccessRight::Terminate));
//		//			return mask;
//		//		};
//		//		constexpr auto GenericDeleteAccess = [] () {
//		//			BitMapT<static_cast<UInt64>(AccessRight::MaxRight)> mask;
//		//			mask.SetBit (static_cast<SizeT>(AccessRight::ExecAPC));
//		//			return mask;
//		//		};
//		//	}
//		//	using ThreadSecurityDescriptor = Seed::Core::Security::GenericSecurityDescriptor<Thread::AccessRight,
//		//		Thread::GenericReadAccess, Thread::GenericWriteAccess, Thread::GenericExecuteAccess, Thread::GenericDeleteAccess
//		//	>;
//		//}
//		//Потік
//		class SeedKernelCall Thread
//		{
//		private:
//			//Список всіх керованих потоків
//#pragma warning(push)
//#pragma warning(disable: 4251)
//			static List<Thread> * Threads;
//#pragma warning(pop)
//			//Головний потік
//			static Thread * MainThread;
//		public:
//			//Подія
//			//Об'єкт синхронізації потоків
//			class SeedKernelCall Event : private IDisposable
//			{
//				//Дані події
//				struct event_data
//				{
//					//Дескриптор
//					void * Handle;
//					//Кількість силок на об'єкт
//					RefCountT RefCount;
//					//Ім'я обєкта події в об'єктній системі NT, якщо немає то рядок пустий
//					String Name;
//					//Конструктор з отриманням дескриптора
//					event_data(const String & name) :Handle(nullptr), RefCount(1), Name(name)
//					{};
//					//Конструктор з оптимізована отриманням дескриптора
//					event_data(String && name) :Handle(nullptr), RefCount(1), Name(name)
//					{};
//				};
//				//Дані об'єкту події
//				event_data * data;
//				//Знищення останнього елемента
//				virtual void Dispose() override final;
//			public:
//				RefCountClass (Event, data);
//				//Конструктор
//				Event(Boolean IsManualReset = false) noexcept;
//				//Конструктор з іменем об'єкта
//				Event(const String & name, Boolean IsManualReset = false) noexcept;
//				//Конструктор з оптимізованою передачею імені об'єкта
//				Event(String && name, Boolean IsManualReset = false) noexcept;
//
//				//Очікування на подію
//				void Wait();
//				//Очікування з таймаутом
//				void Wait(SizeT TimeOutMs);
//
//				//Подати імпульс на подію
//				void Signale();
//				//Виставити true на подію 
//				void Set();
//				//Виставити false на подію
//				void Reset();
//			};
//
//			//Блокувальник для багатопотокових об'єктів
//			// Принцип наступний, дозволяє дочекатись 
//			// завершення всіх операції запущених в блоці Guard
//			// методом WaitFree
//			// Також методом IsFree можна провірити чи зараз відбувається виконання
//			// А методом GetGuardedScopesCount отримати кількість поточних операцій,
//			// виконуються в блоці Guard
//			class SeedKernelCall GuardLock : IDisposable
//			{
//			private:
//				//Дані блокувальника
//				struct guardlock_data
//				{
//					//Дескриптор події
//					Event EventHandle;
//					//SRW блокировка
//					Threading::SRWLock lock;
//					//Кількість активних блокуваннь
//					RefCountT GuardsCount;
//					//Кількість силок на об'єкт
//					RefCountT RefCount;
//					//Конструктор з отриманням дескриптора
//					guardlock_data() :
//						EventHandle(true),
//						RefCount(1),
//						GuardsCount(0)
//					{};
//				};
//				//Дані об'єкту блокуваннь
//				guardlock_data * data;
//
//				// Inherited via IDisposable
//				__forceinline virtual void Dispose() override {
//					delete data;
//				}
//			public:
//				//Конструктор створює об'єкт, ініціалізовуючи внутрішню структуру guardlock_data
//				GuardLock(): data(new guardlock_data) {}
//
//				RefCountClass (GuardLock, data);
//
//				//Блокує WaitFree для чого використовується об'єкт події з NT Native
//				//Для атомарності операції використовується критична секція
//				//Об'єкти даного класу використовують підрахунок силок для ефективного
//				//копіювання та переміщення
//				template<typename lambda>
//				__forceinline auto Guard(lambda scope)
//				{
//					try
//					{
//						if constexpr (Seed::Core::StaticAsserts::IsSame<decltype(scope()), void>())
//						{
//							this->data->lock.ExclusiveLock([&]() {
//								this->data->EventHandle.Reset();
//								this->data->GuardsCount++;
//								scope();
//
//								if (--data->GuardsCount == 0)
//									this->data->EventHandle.Set();
//								});
//						}
//						else
//						{
//							return this->data->lock.ExclusiveLock([&]() -> decltype(scope()) {
//								this->data->EventHandle.Reset();
//								auto&& result = MoveRValue(scope());
//
//								if (--data->GuardsCount == 0)
//									this->data->EventHandle.Set();
//
//								return result;
//								});
//						}
//					}
//					catch (Exception & ex)
//					{
//						this->data->lock.ExclusiveLock([&]() {
//							if (--data->GuardsCount == 0)
//								this->data->EventHandle.Set();
//							});
//						Throw(ex);
//					}
//					catch (InteruptException & ex)
//					{
//						this->data->lock.ExclusiveLock([&]() {
//							if (--data->GuardsCount == 0)
//								this->data->EventHandle.Set();
//							});
//						Throw(ex);
//					}
//				}
//
//				//Очікує завершення всіх блоків запущених в блоці Guard
//				__forceinline void WaitFree()
//				{
//					this->data->EventHandle.Wait();
//				}
//
//				//Вертає true якщо немає блоків в блоці Guard
//				__forceinline Boolean IsFree()
//				{
//					return data->GuardsCount == 0;
//				}
//				//Вертає кількість блоків запущених в блоці Guard
//				__forceinline SizeT GetGuardedScopesCount() noexcept
//				{
//					return data->GuardsCount;
//				}
//			};
//
//			//Стан потоку
//			enum class ThreadState :Seed::UInt8
//			{
//				NotStarted = 0,
//				Work = 1,
//				Terminated = 2,
//				Killed = 3,
//				ClosedByExeption = 4,
//				CorrectlyEnded = 5
//			};
//			//Дані потоку
//			struct ThreadData
//			{
//				//Стан потоку
//				ThreadState state;
//				//Ім'я потоку
//				String Label;
//				//Дескриптор потоку
//				void * Handle;
//				//TID потоку
//				SizeT ID;
//				//Подія завершення
//				Event * abortEvent;
//				//Кількість силок на об'єкт
//				RefCountT RefCount;
//				//Силка на себе в списку потоків
//				List<Thread>::Iterator thread_list_node;
//
//				//Пустий конструктор
//				__forceinline ThreadData() noexcept :
//					state(ThreadState::NotStarted), Label(), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
//				{};
//				//Конструктор з імен потоку
//				__forceinline ThreadData(const String & label) :
//					state(ThreadState::NotStarted), Label(label), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
//				{};
//				//Конструктор з оптимізованою передачею імені потока
//				__forceinline ThreadData(String && label) :
//					state(ThreadState::NotStarted), Label(MoveRValue(label)), Handle(nullptr), ID(-1), RefCount(1), thread_list_node(), abortEvent(nullptr)
//				{};
//
//				~ThreadData()
//				{
//					Label.~BaseString();
//					thread_list_node.~Iterator();
//				}
//			};
//		private:
//			//Дані потоку
//			ThreadData * thread;
//			//Реалізація запуску стартового потоку
//			static Thread & InitMainThreadObj(Delegate && ManagedEntryPoint);
//
//
//			//Ініціалізація підсистеми потоків
//			extern friend bool InitCoreThreading() noexcept;
//			//Зупинка підсистеми потоків
//			extern friend void StopCoreThreading() noexcept;
//		public:
//			//Вертає силку на поточний потік
//			static Thread GetThis();
//			//Вертає силку по ID потоку
//			static Thread GetByID(UInt64 TargetTID);
//			//Вертає ID потоку
//			static ID GetID();
//			//Призупинити виконання потоку на час ms вказаний в мілісекундах
//			static void Sleep(UInt64 ms);
//			//Призупинення потоку навічно(використовувати лише якщо потік сон переривається ззовні)
//			static void Sleep();
//			//Очікувати на завершення потоку по ID, таймаут вказуєтья в мілісекундах
//			//якщо не вказано то без таймаута
//			static void Wait(UInt64 TargetTID, SizeT Timeout = 0);
//			//Очікувати на завершення потоку силці на потік, таймаут вказуєтья в мілісекундах
//			//якщо не вказано то без таймаута
//			static void Wait(Thread &thread, SizeT Timeout = 0);
//			//Очікувати на завершення головного потоку
//			__forceinline static void WaitOnMainThread(SizeT Timeout = 0)
//			{
//				Wait(*MainThread, Timeout);
//			}
//			//Отримати список потоків по імені
//			static List<Thread> GetByLabel(const String & Label);
//			//Отримати список всіх потоків
//			static List<Thread> GetAllThreads()
//			{
//				return *Threads;
//			}
//			//Запуск головного потоку
//			template<typename F, typename ... Args>
//			static Thread & InitMainThread(F f, Args ... args)
//			{
//				return InitMainThreadObj(CreateAndPackFunctionObj(f, args));
//			}
//			//Переривання роботи потоку, аргументом є ідентифікатор потоку
//			static Boolean Interupt(SizeT ThreadID);
//		public:
//			//Конструктор
//			Thread();
//			//Конструктор з імена потоку
//			Thread(const String & Label);
//			//Конструктор з оптимізованою передачею імені
//			Thread(String && Label);
//			//Копіювальний конструктор
//			__forceinline Thread(const Thread & orig) :thread(orig.thread)
//			{
//				this->thread->RefCount++;
//			}
//			//Конструктор переміщення
//			__forceinline Thread(Thread && orig) noexcept
//			{
//				this->thread = orig.thread;
//				orig.thread = nullptr;
//			}
//			//Оператор копіювання
//			__forceinline Thread & operator=(const Thread & orig)
//			{
//				this->~Thread();
//				this->thread = orig.thread;
//				this->thread->RefCount++;
//				return *this;
//			}
//			//Оператор переміщення
//			__forceinline Thread & operator=(Thread && orig) noexcept
//			{
//				this->~Thread();
//				this->thread = orig.thread;
//				orig.thread = nullptr;
//				return *this;
//			}
//			//Зміна імені потоку
//			__forceinline void ChangeLabel(const String & NewLabel)
//			{
//				this->thread->Label = NewLabel;
//			}
//			//Вертає ім'я потоку
//			const String & GetLabel() const
//			{
//				return this->thread->Label;
//			}
//
//			//Запуск потоку, аргументом є делегат який містить функцію з її аргументами
//			void ExecDelegate(Delegate && ThreadMain);
//
//			//Запуск потоку, аргументами є функція і її аргументи
//			template<typename F, typename ... Args>
//			Thread & ExecFunct(F f, Args& ... args)
//			{
//				ExecDelegate(CreateAndPackFunctionObj(f, args...));
//				return *this;
//			}
//
//			//Переривання сну потоку
//			void Alert();
//
//			//Виставлення функції в APC чергу потоку
//			void DispatchAPC(Delegate && APCDelegate);
//
//			//Примусове завершення потоку, використовувати в рідкісних випадках
//			//бо не відбувається видалення об'єктів створених потоком
//			void Kill();
//
//			//Провірка чи потік запущений
//			__forceinline bool isWork()
//			{
//				return thread->ID != -1;
//			}
//
//			//Переривання роботи потоку
//			Thread & Interupt();
//
//			//Стан об'єкту потоку
//			ThreadState State()
//			{
//				return thread->state;
//			}
//
//			//Очікувати на завершення потоку
//			void Join(SizeT Timeout = 0) const;
//
//			//Вертає ID потоку
//			__forceinline ID GetThisThreadID() const
//			{
//				return thread->ID;
//			}
//
//			//Оператор перетворення вертає ID потоку
//			__forceinline operator SizeT() const
//			{
//				return thread->ID;
//			}
//			//Оператор перетворення вертає true яко потік запущений, false якщо ні
//			__forceinline operator Boolean() const
//			{
//				return thread->ID != -1;
//			}
//
//			//Деструктор об'єкта потоку
//			~Thread();
//
//			//Функція обміну для об'єктів потоків
//			friend void Seed::Swap(Seed::Core::Thread & a, Seed::Core::Thread & b) noexcept;
//		};
//	}
//	//Оптимізований обмін об'єктів потоку
//	template<>
//	__forceinline void Swap(Seed::Core::Thread & a, Seed::Core::Thread & b) noexcept
//	{
//		Swap(a.thread, b.thread);
//	}
//}
