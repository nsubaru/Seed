#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Containers.Delegate.h"
#include "Seed.Core.Security.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.CoreLog.Managed.h"

namespace Seed {
	inline namespace Core {
		inline namespace Exceptions {
			///<summary>Неіснуючий потік або він не є керований через Core</summary>
			class SeedKernelCall InvalidThread :public Seed::Exception
			{
			private:
				///<summary>ID не існуючого потоку</summary>
				ID tid;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'TID'>Id не існуючого потоку</param>
				__forceinline InvalidThread(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ID TID) //-V730
					noexcept : Exception(File, FunctName, Line, L"Не можна працювати з незареєстрованим в рантаймі потоком"), tid(TID)
				{};

				///<summary>Вертаж ID неіснуючого потку</summary>
				ID GetTID() { return tid; }

				///<summary>ID не існуючого потоку</summary>
				ID Property(GetTID) TID;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(this->exeptionName)
						+ ExceptionString(L". Не знайдено потоку з ID ") + ExceptionString(tid) + ExceptionString(L". ")
						+ this->Location.toString() + ExceptionString(L".");
				}

				virtual ~InvalidThread() override final {};
			};

			///<summary>Не валідний об'єкт потоку</summary>
			class SeedKernelCall InvalidThreadObj :public Seed::Exception
			{
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				__forceinline InvalidThreadObj(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Не валідний об'єкт потоку")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(exeptionName)
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~InvalidThreadObj() override final {}
			};
			///<summary>Запуск запущеного потоку</summary>
			class SeedKernelCall ThreadAlredyStrated :public Seed::Exception
			{
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				__forceinline ThreadAlredyStrated(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Запуск запущеного потоку")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(exeptionName)
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~ThreadAlredyStrated() override final {}
			};

			///<summary>Взаємне блокування</summary>
			class SeedKernelCall DedlockExpected :public Seed::Exception
			{
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				__forceinline DedlockExpected(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Взаємне блокування")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString(exeptionName)
						+ ExceptionString(L". ")
						+ location.toString()
						+ ExceptionString(L". ");
				}

				virtual ~DedlockExpected() override final {}
			};
		}
		namespace Threading {
			///<summary>Керований рантаймом потік</summary>
			class SeedKernelCall Thread : protected IDisposable
			{
			public:
				///<summary>Подія NT
				///<para>Об'єкт синхронізації потоків</para></summary>
				class SeedKernelCall Event : protected IDisposable
				{
					///<summary>Дані події</summary>
					struct event_data
					{
						///<summary>Тип</summary>
						Boolean IsManualReset;
						///<summary>Дескриптор</summary>
						Handle handle;
						///<summary>Кількість силок на об'єкт</summary>
						RefCountT RefCount;
						///<summary>Ім'я обєкта події в об'єктній системі NT, якщо немає то рядок пустий</summary>
						String Name;

						///<summary>Конструктор з отриманням дескриптора</summary>
						event_data(Boolean IsManualReset, const String& name)
							:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(name)
						{};
						///<summary>Конструктор з оптимізована отриманням дескриптора</summary>
						event_data(Boolean IsManualReset, String&& name)
							:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(MoveRValue(name))
						{};
					};
					///<summary>Дані об'єкту події</summary>
					event_data* data;
					///<summary>Знищення останнього елемента</summary>
					virtual void Dispose() override final;
				public:
					///<summary>Конструктор</summary>
					Event(Boolean IsManualReset = false);
					///<summary>Конструктор з іменем об'єкта</summary>
					Event(const String& name, Boolean IsManualReset = false);
					///<summary>Конструктор з оптимізованою передачею імені об'єкта</summary>
					Event(String&& name, Boolean IsManualReset = false);

					RefCountClass(Event, data);

					///<summary>Очікування на подію</summary>
					void Wait();
					///<summary>Очікування з таймаутом</summary>
					void Wait(SizeT TimeOutMs);

					///<summary>Подати імпульс на подію</summary>
					void Signale() {
						extern void PulseNtEvent(void* handle);
						PulseNtEvent(data->handle);
					}
					///<summary>Виставити true на подію</summary>
					void Set() {
						extern void SetNtEvent(void* handle);
						SetNtEvent(data->handle);
					}
					///<summary>Виставити false на подію</summary>
					void Reset() {
						extern void ResetNtEvent(void* handle);
						ResetNtEvent(data->handle);
					}
				};

				///<summary>Блокувальник для багатопотокових об'єктів
				///<para> Принцип наступний, дозволяє дочекатись
				///завершення всіх операції запущених в блоці Guard
				///методом WaitFree
				///Також методом IsFree можна провірити чи зараз відбувається виконання
				///А методом GetGuardedScopesCount отримати кількість поточних операцій,
				///виконуються в блоці Guard</para></summary>
				class SeedKernelCall GuardLock : protected IDisposable
				{
				private:
					///<summary>Дані блокувальника</summary>
					struct guardlock_data
					{
						///<summary>Дескриптор події</summary>
						Event EventHandle;
						///<summary>SRW блокировка</summary>
						Threading::SRWLock lock;
						///<summary>Кількість активних блокуваннь</summary>
						RefCountT GuardsCount;
						///<summary>Кількість силок на об'єкт</summary>
						RefCountT RefCount;
						///<summary>Конструктор з отриманням дескриптора</summary>
						guardlock_data() :
							EventHandle(true),
							RefCount(1),
							GuardsCount(0)
						{};
					};
					///<summary>Дані об'єкту блокуваннь</summary>
					guardlock_data* data;

					///<summary>Звільнення ресурсів</summary>
					__forceinline virtual void Dispose() override {}
				public:
					///<summary>Конструктор створює об'єкт, ініціалізовуючи внутрішню структуру guardlock_data</summary>
					GuardLock() : data(new guardlock_data) {}

					RefCountClass(GuardLock, data);

					///<summary>Блокує WaitFree для чого використовується об'єкт події з NT Native
					///<para>Для атомарності операції використовується критична секція</para>
					///<para>Об'єкти даного класу використовують підрахунок силок для ефективного</para>
					///<para>копіювання та переміщення</para></summary>
					template<typename lambda>
					__forceinline auto Guard(lambda scope)
					{
						try
						{
							if constexpr (Seed::Core::StaticAsserts::IsSameT<decltype(scope()), void>())
							{
								this->data->lock.ExclusiveLock([&]() {
									this->data->EventHandle.Reset();
									++this->data->GuardsCount;
									scope();

									if (--data->GuardsCount == 0)
										this->data->EventHandle.Set();
									});
							}
							else
							{
								return this->data->lock.ExclusiveLock([&]() -> decltype(scope()) {
									this->data->EventHandle.Reset();
									auto&& result = MoveRValue(scope());

									if (--data->GuardsCount == 0)
										this->data->EventHandle.Set();

									return result;
									});
							}
						}
						catch ([[maybe_unused]] InteruptException& ex)
						{
							this->data->lock.ExclusiveLock([&]() {
								if (--data->GuardsCount == 0)
									this->data->EventHandle.Set();
								});
							ReThrow;
						}
						catch ([[maybe_unused]] Exception& ex)
						{
							this->data->lock.ExclusiveLock([&]() {
								if (--data->GuardsCount == 0)
									this->data->EventHandle.Set();
								});
							ReThrow;
						}
					}

					///<summary>Очікує завершення всіх блоків запущених в блоці Guard</summary>
					__forceinline void WaitFree()
					{
						this->data->EventHandle.Wait();
					}

					///<summary>Вертає true якщо немає блоків в блоці Guard</summary>
					__forceinline Boolean IsFree()
					{
						return data->GuardsCount == 0;
					}

					///<summary>Вертає кількість блоків запущених в блоці Guard</summary>
					__forceinline SizeT GetGuardedScopesCount() noexcept
					{
						return data->GuardsCount;
					}

					///<summary>Кількість блоків запущених в блоці Guard</summary>
					SizeT Property(GetGuardedScopesCount) GuardedScopesCount;
				};

				///<summary>Стан потоку</summary>
				enum class SeedKernelCall ThreadState :Seed::UInt8
				{
					///<summary>Не запущений</summary>
					NotStarted = 0,
					///<summary>Ініціалізація потоку</summary>
					Init = 1,
					///<summary>Активний</summary>
					Work = 2,
					///<summary>Примусово завершив роботу</summary>
					Terminated = 3,
					///<summary>Вбитий</summary>
					Killed = 254U,
					///<summary>Завершив роботу внаслідок помилки</summary>
					ClosedByExeption = 255U,
					///<summary>Коректно завершив роботу</summary>
					CorrectlyEnded = 5
				};
			private:
				///<summary>Дані потоку</summary>
				struct SeedKernelCall ThreadData
				{
					///<summary>Стан потоку</summary>
					ThreadState state;
					///<summary>Дескриптор потоку</summary>
					Handle handle;
					///<summary>TID потоку</summary>
					SizeT ID;
					///<summary>Подія завершення</summary>
					Event* abortEvent;
					///<summary>Точка входу потоку</summary>
					Delegate* EntryPoint;
					///<summary>Кількість силок на об'єкт</summary>
					RefCountT RefCount;
					///<summary>Ім'я потоку</summary>
					String Label;
#pragma warning(push)
#pragma warning(disable: 4251)
					///<summary>Силка на себе в списку потоків</summary>
					List<Thread>::Iterator thread_list_node;
#pragma warning(pop)
					///<summary>Пустий конструктор</summary>
					__forceinline ThreadData() noexcept :
						state(ThreadState::NotStarted),
						Label(),
						handle(nullptr),
						ID(-1),
						RefCount(1),
						thread_list_node(),
						abortEvent(nullptr),
						EntryPoint(nullptr)
					{};
					///<summary>Конструктор з імен потоку</summary>
					///<param name = "label">Назва потоку</param>
					__forceinline ThreadData(const String& label) :
						state(ThreadState::NotStarted),
						Label(label),
						handle(nullptr),
						ID(-1),
						RefCount(1),
						thread_list_node(),
						abortEvent(nullptr),
						EntryPoint(nullptr)
					{};
					///<summary>Конструктор з оптимізованою передачею імені потока</summary>
					///<param name = "label">Назва потоку</param>
					__forceinline ThreadData(String&& label) :
						state(ThreadState::NotStarted),
						Label(MoveRValue(label)),
						handle(nullptr), ID(-1),
						RefCount(1),
						thread_list_node(),
						abortEvent(nullptr),
						EntryPoint(nullptr)
					{};
				};
			private:
				///<summary>Дані цього потоку</summary>
				ThreadData* thread;

				///<sumary>Ініціалізація підсистеми потоків</summary>
				extern friend bool InitCoreThreading() noexcept;
				///<summary>Зупинка підсистеми потоків</summary>
				extern friend void StopCoreThreading() noexcept;

				///<summary>Внутрішня функція ініціалізації потоку</summary>
				extern friend Int32 ThreadInitRoute(void* arg) noexcept;
			protected:
				///<summary>Знищення останього екземпляра</summary>
				virtual void Dispose() override final;
			public:
				///<summary>Вертає силку на поточний потік</summary>
				static Thread GetThis();
				///<summary>Вертає силку по ID потоку</summary>
				static Thread GetByID(UInt64 TargetTID);
				///<summary>Вертає ID потоку</summary>
				static ID GetTID();
				///<summary>Призупинити виконання потоку на час ms вказаний в мілісекундах</summary>
				static void Sleep(UInt64 ms);
				///<summary>Призупинення потоку на вічно(використовувати лише якщо потік сон переривається ззовні)</summary>
				static void Sleep();
				///<summary>Очікувати на завершення потоку по ID, таймаут вказуєтья в мілісекундах
				///<para>якщо не вказано то без таймаута</para></summary>
				static void Wait(UInt64 TargetTID, SizeT Timeout = 0);
				///<summary>Отримати список потоків по імені</summary>
				static List<Thread> GetByLabel(const String& Label);
				///<summary>Отримати список всіх потоків</summary>
				static List<Thread> GetAllThreads();
			public:
				///<summary>Конструктор</summary>
				Thread() : thread(new ThreadData(L"unnamed")) {} //-V730
				///<summary>Конструктор з імена потоку</summary>
				Thread(const String& Label) : thread(new ThreadData(L"Label")) {} //-V730
				///<summary>Конструктор з оптимізованою передачею імені</summary>
				Thread(String&& Label) : thread(new ThreadData(MoveRValue(Label))) {} //-V730

				RefCountClass(Thread, thread); //-V730

				///<summary>Зміна імені потоку</summary>
				void ChangeLabel(const String& NewLabel);
				//Вертає ім'я потоку
				const String& GetLabel() const
				{
					return this->thread->Label;
				}

				///<summary>Запуск потоку, аргументом є делегат який містить функцію з її аргументами</summary>
				void ExecDelegate(Delegate&& ThreadMain);

				///<summary>Запуск потоку, аргументами є функція і її аргументи</summary>
				///<param name = "F"> Функція яку виконуватиме потік</param>
				///<param name = "Args">Аргументи функції</param>
				template<typename F, typename ... Args>
				Thread& ExecFunct(F f, Args& ... args)
				{
					ExecDelegate(CreateAndPackFunctionObj(f, args...));
					return *this;
				}

				///<summary>Переривання сну потоку</summary>
				void ExecApc();

				///<summary>Виставлення функції в APC чергу потоку</summary>
				void DispatchAPC(Delegate&& APCDelegate);

				///<summary>Примусове завершення потоку, використовувати в рідкісних випадках
				///<para>бо не відбувається видалення об'єктів створених потоком</summary>
				DeprecatedCall("Can't free resources if thread kill") void Kill();

				///<summary>Провірка чи потік запущений</summary>
				__forceinline bool isWork()
				{
					return thread->ID != -1;
				}

				//Переривання роботи потоку
				Thread& Interupt();

				//Стан об'єкту потоку
				ThreadState State()
				{
					return thread->state;
				}

				//Очікувати на завершення потоку
				Thread& Join(SizeT Timeout = 0);

				//Очікувати на завершення потоку
				const Thread& Join(SizeT Timeout = 0) const;

				//Вертає ID потоку
				__forceinline ID GetThisThreadID() const
				{
					return thread->ID;
				}

				ID Property(GetThisThreadID) TID;

				//Оператор перетворення вертає true яко потік запущений, false якщо ні
				__forceinline operator Boolean() const
				{
					return thread->ID != -1;
				}

				//Вертає результат точки входу потоку потоку
				template<typename T>
				T GetResult() {
					switch (this->thread->state) //-V2520
					{
					case ThreadState::NotStarted:
					case ThreadState::Init:
					case ThreadState::Work:
					case ThreadState::Killed:
					case ThreadState::Terminated:
						Throw(NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
					case ThreadState::ClosedByExeption:
						Throw(NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
					case ThreadState::CorrectlyEnded:
						return this->thread->EntryPoint->GetResult<T>();
					default:
						Throw(NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
					}
				}

				//Функція обміну для об'єктів потоків
				friend void Seed::Swap(Seed::Core::Threading::Thread& a, Seed::Core::Threading::Thread& b) noexcept;
			};
		}
	}
	//Оптимізований обмін об'єктів потоку
	template<>
	__forceinline void Swap(Seed::Core::Threading::Thread & a, Seed::Core::Threading::Thread & b) noexcept
	{
		Swap(a.thread, b.thread);
	}
}
