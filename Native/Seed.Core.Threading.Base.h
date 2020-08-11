#pragma once
#include "Seed.Core.Containers.Concurency.Array.h"
#include "Seed.Core.Containers.Concurency.List.h"
#include "Seed.Core.Containers.SmartPtr.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Exceptions.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			enum class EThreadableTypes : UInt8 {
				Thread, JobTask, ScheduledTask, Unknown = 255
			};
			enum class EThreadabledStates : UInt8 {
				NotStarted, Starting, Executing, Paused, ExceptionProcessing, Interupted, Ended, Unknown = 255
			};
			enum class EThreadableCodeType : UInt8 {
				Native, Managed, Unknown = 255
			};

			class IThreadable {
			public:
				virtual const String& GetName() const = 0;
				virtual void SetName(const String& NewName) = 0;

				virtual UInt64 GetID() const = 0;

				virtual EThreadableTypes GetThreadableType() const = 0;
				virtual EThreadabledStates GetThreadabledState() const = 0;

				virtual Containers::Array<String> GetCallStack() const = 0;

				virtual IThreadable& GetParrentThreadable() const = 0;
				virtual Boolean IsHaveParrentThreadable() const = 0;

				virtual IThreadable& Suspend() = 0;
				virtual IThreadable& Interupt() = 0;
				virtual IThreadable& Join() = 0;
				virtual IThreadable& Join(DateTime timeOut) = 0;
				virtual const IThreadable& Join() const = 0;
				virtual const IThreadable& Join(DateTime timeOut) const = 0;
				virtual ~IThreadable() = 0 {}
			};

			SeedKernelCall SizeT GetThreadableCount(Boolean WithPseudoThreadable = false);
			SeedKernelCall Containers::ConcurencyList<SmartPtr<IThreadable>> GetAllThreadable();

			SeedKernelCall IThreadable& GetThis();
			SeedKernelCall ID GetTID();

			void Sleep(DateTime time, Boolean IsRelative);
			inline void Sleep(UInt64 ms) { Sleep(DateTime((DateTime::SecondInNtTime / 1000) * ms), true); }

			enum class EJobStates :UInt8 {
				NotStarted, Executing, Finished, ExceptionCatched, Unknown = 255
			};

			interface IJob
			{
				EJobStates GetCurrentState();
				void Cancel();
				IJob& Interupt();
				IJob& Join();
				IJob& Join(DateTime timeOut);
			};

			///<summary>Неіснуючий потік або він не є керований через Core</summary>
			class InvalidThread final : public Exception //-V1052
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
				ForceInline InvalidThread(const ConstAString& File, const ConstAString& FunctName, SizeT Line, ID TID) //-V730
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
						ExceptionString(L". Не знайдено потоку з ID "), ExceptionString(tid), ExceptionString(L". "),
						Location.toString(), ExceptionString(L".")
					});
				}

				ForceInline virtual ~InvalidThread() override final {};
			};

			///<summary>Не валідний об'єкт потоку</summary>
			class InvalidThreadObj final : public Exception
			{
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
                        Location.toString(), ExceptionString(L".")
                    });
				}

				ForceInline virtual ~InvalidThreadObj() override final {}
			};
			///<summary>Запуск запущеного об'єкта</summary>
			class ThreadableAlredyStarted final : public Exception //-V1052
			{
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
                        Location.toString(), ExceptionString(L".")
                    });
				}

				ForceInline virtual ~ThreadableAlredyStarted() override final {}
			};

			///<summary>Взаємне блокування</summary>
			class DedlockExpected final : public Exception //-V1052
			{
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
                        Location.toString(), ExceptionString(L".")
                    });
				}

				ForceInline virtual ~DedlockExpected() override final {}
			};
		}
	}
}