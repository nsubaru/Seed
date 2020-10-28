#pragma once
#include "Seed.Core.Time.Base.h"
#include "Seed.Core.Containers.Concurency.Array.h"
#include "Seed.Core.Containers.Concurency.List.h"
#include "Seed.Core.Containers.SmartPtr.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			/// <summary>
			/// Типи потокових сутностей
			/// </summary>
			enum class EThreadableTypes : UInt8 {
				Thread, JobTask, ScheduledTask, IoTask, Unknown = 255
			};

			/// <summary>
			/// Стани потокових сутностей
			/// </summary>
			enum class EThreadabledStates : UInt8 {
				NotStarted, Starting, Executing, Paused, ExceptionProcessing, Interupted, Ended, Unknown = 255
			};

			/// <summary>
			/// Тип коду потокових сутностей
			/// </summary>
			enum class EThreadableCodeType : UInt8 {
				Native, Managed, Unknown = 255
			};

			/// <summary>
			/// Інтерфейс потокових сутностей
			/// </summary>
			class IThreadable {
			public:
				/// <summary>
				/// Вертає новий референс
				/// </summary>
				/// <returns></returns>
				virtual SmartPtr<IThreadable> MakeRef() const = 0;
			public:
				/// <summary>
				/// Отримання імені сутності
				/// </summary>
				/// <returns>Рядкове представлення імені</returns>
				virtual const String& GetName() const = 0;
				/// <summary>
				/// Встановлення імені сутності
				/// </summary>
				/// <param name="NewName">Нове ім'я сутності</param>
				virtual void SetName(const String& NewName) = 0;
				
				/// <summary>
				/// Ідентифікатор сутності
				/// </summary>
				/// <returns></returns>
				virtual ID GetID() const = 0;

				/// <summary>
				/// Тип потокової сутності
				/// </summary>
				/// <returns>Тип потокової сутності</returns>
				virtual EThreadableTypes GetThreadableType() const = 0;

				/// <summary>
				/// Стан потокової сутності
				/// </summary>
				/// <returns>Стан потокової сутності</returns>
				virtual EThreadabledStates GetThreadabledState() const = 0;

				/// <summary>
				/// Заморозка виконання потокової сутності
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				virtual IThreadable& Suspend() = 0;
				/// <summary>
				/// Розмороження виконання потокової сутності
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				virtual IThreadable& Resume() = 0;
				/// <summary>
				/// Переривання виконання потокової сутності
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				virtual IThreadable& Interupt() = 0;
				/// <summary>
				/// Очікувння завершення потокової сутності
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				virtual IThreadable& Join() = 0;
				/// <summary>
				/// Очікувння завершення потокової сутності з таймаутом
				/// </summary>
				/// <param name="timeOut">Максимальний час очікування</param>
				/// <returns>Посилання на об'єкт</returns>
				virtual IThreadable& Join(Time::TimeSpan timeOut) = 0;
				/// <summary>
				/// Очікувння завершення потокової сутності
				/// </summary>
				/// <returns>Посилання на об'єкт</returns>
				virtual const IThreadable& Join() const = 0;
				/// <summary>
				/// Очікувння завершення потокової сутності з таймаутом
				/// </summary>
				/// <param name="timeOut">Максимальний час очікування</param>
				/// <returns>Посилання на об'єкт</returns>
				virtual const IThreadable& Join(Time::TimeSpan timeOut) const = 0;

				virtual ~IThreadable() = 0 {}
			};

			namespace Threadables {
				/// <summary>
				/// Посилання на потік
				/// </summary>
				namespace {
					using ThreadableRef = Seed::Core::Containers::ConcurencySensitiveList<Seed::Core::SmartPtr<Seed::Core::Threading::IThreadable>>::Iterator;
				}
			}

			/// <summary>
			/// Вертає кількість активних потокових сутностей
			/// </summary>
			/// <param name="WithPseudoThreadable">якщо true то окрім потоків будуть рахуватись і інші потокові сутності</param>
			SeedKernelCall SizeT GetThreadableCount(Boolean WithPseudoThreadable = false);

			/// <summary>
			/// Вертає список всіх потокових сутностей
			/// </summary>
			SeedKernelCall Containers::ConcurencyList<SmartPtr<IThreadable>> GetAllThreadable();

			/// <summary>
			/// Вертає посилання на потокову сутність в межає якої виконується поточний код
			/// </summary>
			SeedKernelCall IThreadable& GetThis();
			/// <summary>
			/// Вертає ідентифікатор потокової сутності в межає якої виконується поточний код
			/// </summary>
			SeedKernelCall ID GetTID();

			/// <summary>
			/// Призупиняє виконання потоку на задану кількість часу
			/// </summary>
			/// <param name="time">Час</param>
			/// <param name="IsRelative">якщо true то час береться як відносне значення відносно часу призупинення</param>
			void Sleep(Time::DateTime time, Boolean IsRelative);

			/// <summary>
			/// Призупиняє виконання потоку на задану кількість мілісекунд
			/// </summary>
			/// <param name="ms">Кількість мілісекунд</param>
			ForceInline void Sleep(UInt64 ms) { Sleep(Time::DateTime((Time::SecondInNtTime / 1000) * ms), true); }

			/// <summary>
			/// Призупиняє виконання потоку на заданий проміжок часу
			/// </summary>
			/// <param name="time">Заданий проміжок часу</param>
			ForceInline void Sleep(Time::TimeSpan time) { Sleep(Time::DateTime(time.Ticks), true); }

			/// <summary>
			/// Стани задач
			/// </summary>
			enum class EJobStates :UInt8 {
				NotStarted, Executing, Finished, ExceptionCatched, Unknown = 255
			};


			///<summary>Неіснуючий потік або він не є керований через Core</summary>
			class InvalidThread final : public Exception {
			private:
				///<summary>ID не існуючого потоку</summary>
				ID tid;
			public:
				InvalidThread(const InvalidThread& orig) : Exception(orig), tid(orig.tid) {}
				InvalidThread(InvalidThread&& from) : Exception(MoveRValue(from)), tid(from.tid) {}
				InvalidThread& operator=(const InvalidThread& from) { this->~InvalidThread(); this->InvalidThread::InvalidThread(from); return *this; }
				InvalidThread& operator=(InvalidThread&& from) { this->~InvalidThread(); this->InvalidThread::InvalidThread(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'TID'>Id не існуючого потоку</param>
				ForceInline InvalidThread(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ID TID)
					noexcept : Exception(File, FunctName, Line, L"Не можна працювати з незареєстрованим в рантаймі потоком"), tid(TID)
				{};

				///<summary>Вертаж ID неіснуючого потку</summary>
				ID GetTID() { return tid; }

				///<summary>ID не існуючого потоку</summary>
				ID Property(GetTID) TID;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(this->exceptionName),
						ExceptionString(L". Не знайдено потоку з ID "), ExceptionString::toString(tid), ExceptionString(L". "),
						Location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidThread(*this));
				}

				ForceInline virtual ~InvalidThread() override final {};
			};

			///<summary>Не валідний об'єкт потоку</summary>
			class InvalidThreadObj final : public Exception {
			public:
				InvalidThreadObj(const InvalidThreadObj& orig) : Exception(orig) {}
				InvalidThreadObj(InvalidThreadObj&& from) : Exception(MoveRValue(from)) {}
				InvalidThreadObj& operator=(const InvalidThreadObj& from) { this->~InvalidThreadObj(); this->InvalidThreadObj::InvalidThreadObj(from); return *this; }
				InvalidThreadObj& operator=(InvalidThreadObj&& from) { this->~InvalidThreadObj(); this->InvalidThreadObj::InvalidThreadObj(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline InvalidThreadObj(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Не валідний об'єкт потоку")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
                    });
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidThreadObj(*this));
				}

				ForceInline virtual ~InvalidThreadObj() override final {}
			};
			///<summary>Запуск запущеного об'єкта</summary>
			class ThreadableAlredyStarted final : public Exception {
			public:
				ThreadableAlredyStarted(const ThreadableAlredyStarted& orig) : Exception(orig) {}
				ThreadableAlredyStarted(ThreadableAlredyStarted&& from) : Exception(MoveRValue(from)) {}
				ThreadableAlredyStarted& operator=(const ThreadableAlredyStarted& from) { this->~ThreadableAlredyStarted(); this->ThreadableAlredyStarted::ThreadableAlredyStarted(from); return *this; }
				ThreadableAlredyStarted& operator=(ThreadableAlredyStarted&& from) { this->~ThreadableAlredyStarted(); this->ThreadableAlredyStarted::ThreadableAlredyStarted(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline ThreadableAlredyStarted(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Запуск запущеного об'єкта")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
                    });
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new ThreadableAlredyStarted(*this));
				}

				ForceInline virtual ~ThreadableAlredyStarted() override final {}
			};

			///<summary>Взаємне блокування</summary>
			class DedlockExpected final : public Exception {
			public:
				DedlockExpected(const DedlockExpected& orig) : Exception(orig) {}
				DedlockExpected(DedlockExpected&& from) : Exception(MoveRValue(from)) {}
				DedlockExpected& operator=(const DedlockExpected& from) { this->~DedlockExpected(); this->DedlockExpected::DedlockExpected(from); return *this; }
				DedlockExpected& operator=(DedlockExpected&& from) { this->~DedlockExpected(); this->DedlockExpected::DedlockExpected(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline DedlockExpected(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Взаємне блокування")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
                        ExceptionString(exceptionName), ExceptionString(L". "),
                        Location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
                    });
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new DedlockExpected(*this));
				}

				ForceInline virtual ~DedlockExpected() override final {}
			};
		}
	}
}