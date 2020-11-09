#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Time.Base.h"
#include "Seed.Core.Memory.h"
#include "Seed.Core.ConstString.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Containers.SmartPtr.h"
#include "typeinfo"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
namespace Seed {
	inline namespace Core {
		namespace Memory {
			class ExceptionHeap final : public BasicHeap {
			public:
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Allocate(DataSizeT Size) final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedAllocate(DataSizeT Size, DataSizeT AllocationReq) final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Free(void* MemBlk) final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedFree(void* MemBlk, DataSizeT AllocationReq) final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual DataSizeT CurrentAllocatedMemory() final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual DataSizeT CurrentHeapUsage() final;

				ExceptionHeap& operator=(const ExceptionHeap&) = delete;
			};
		}

		///<summary>Підсистема помилок в рантаймі</summary>
		inline namespace Exceptions
		{
			///<summary>Класс базової помилки</summary>
			class Exception;

			///<summary>Рядок для збереження тексту помилок</summary>
			namespace { using ExceptionString = BaseString<Memory::ExceptionHeap>; }
			/// <summary> Об'єкт для збереження помилки  </summary>
			namespace { using ExceptionHolderType = BaseSmartPtr<Memory::ExceptionHeap, Exception>; }
		}

		///<summary>Підсистема помилок в рантаймі</summary>
		inline namespace Exceptions {
			///<summary>Класс базової помилки</summary>
			class Exception : public Memory::Allocator<Memory::ExceptionHeap>, public IClonable<ExceptionHolderType> {
			public:
				/// <summary>
				/// Сховище стеку викликів
				/// </summary>
				struct StackTraceRecord {
					/// <summary>
					/// Максимальна кількість адресів функцій які можуть зберігатись в сховищі
					/// </summary>
					static inline constexpr SizeT MaxStackTraceItemCount = (8Kb).Bytes / sizeof(SizeT);

					/// <summary>
					/// Реальна кількість адресів функцій
					/// </summary>
					SizeT Count;
					/// <summary>
					/// Адреса функцій
					/// </summary>
					SizeT FunctionAddress[MaxStackTraceItemCount];	
				};

				///<summary>Місце генерації помилки</summary>
				struct ExeptionLocation {
					///<summary>Ім'я файла</summary>
					ConstAString File;
					///<summary>Ім'я функції</summary>
					ConstAString FunctName;
					///<summary>Номер рядка</summary>
					SizeT Line;
					///<summary>ID потоку</summary>
					ID ThreadID;
					///<summary>Текстове представлення</summary>
					ExceptionString toString() const {
						return ExceptionString::Combine({
							ExceptionString(L"Згенеровано у рядку ") , ExceptionString::toString(Line), 
							ExceptionString(L" у функції ") , ExceptionString(FunctName), 
							ExceptionString(L" у файлі ") , ExceptionString(File), 
							ExceptionString(L" в потоці з ID ") , ExceptionString::toString(ThreadID)
						});
					}
				};
			protected:
				///<summary>Місце генерації даної помилки</summary>
				ExeptionLocation location;
				///<summary>Ім'я помилки</summary>
				ConstString exceptionName;
				///<summary>Стек помилки</summary>
				StackTraceRecord stackTrace;
				///<summary>Внутрішня помилка</summary>
				ExceptionHolderType innerException;
			private:
				static ID GetTargetThreadID();
				friend void GetStackBackTrace(Seed::Exceptions::Exception::StackTraceRecord& stackTrace);
				friend ExceptionHolderType* GetInnerExceptionObjectPtr(Exception* exception);
			public:
				/// <summary>
				/// Копіювальний конструктор 
				/// </summary>
				/// <param name="from"></param>
				Exception(const Exception& from) : 
					location(from.location), 
					exceptionName(from.exceptionName), 
					stackTrace(from.stackTrace), 
					innerException(from.innerException) {}

				/// <summary>
				/// Конструктор переміщення конструктор 
				/// </summary>
				/// <param name="from"></param>
				Exception(Exception&& from) : 
					location(from.location), 
					exceptionName(MoveRValue(from.exceptionName)), 
					stackTrace(from.stackTrace), 
					innerException(MoveRValue(from.innerException)) {}

				/// <summary>
				/// Оператор копіювання
				/// </summary>
				/// <param name="ex">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				Exception& operator=(const Exception& ex) {
					this->~Exception();
					this->Exception::Exception(ex);
					return *this;
				}

				/// <summary>
				/// Оператор переміщення
				/// </summary>
				/// <param name="ex">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				Exception& operator=(Exception&& ex) {
					this->~Exception();
					this->Exception::Exception(MoveRValue(ex));
					return *this;
				}
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Name'>Ім'я помилки</param>
				///<returns>Екземпляр класу Exeption</returns>
				ForceInline Exception(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ConstString& ExeptionName) noexcept
					:location({ File, FunctName, Line, GetTargetThreadID() }), exceptionName(ExeptionName), stackTrace({ 0 }), innerException(nullptr)
				{
					GetStackBackTrace((Seed::Exceptions::Exception::StackTraceRecord&)this->stackTrace);
				}

				///<summary>Вертає користувальницьку назву помилки</summary>
				ForceInline const ConstString& GetExceptioName() const
				{
					return exceptionName;
				}

				///<summary>Вертає місце генерації помилки</summary>
				ForceInline const ExeptionLocation& GetLocation() const
				{
					return location;
				}

				/// <summary>
				/// Вертає стек викликів на момент генерації помилки
				/// </summary>
				/// <returns></returns>
				ForceInline const StackTraceRecord& GetStackTrace() const {
					return stackTrace;
				}

				/// <summary>
				/// Вертає об'єкт який містить внутрішню помилку(об'єкт може бути пустий)
				/// </summary>
				/// <returns></returns>
				ForceInline const ExceptionHolderType& GetInnerException() const {
					return innerException;
				}

				/// <summary>
				/// Встановлює об'єкт який містить внутрішню помилку(об'єкт може бути пустий)
				/// </summary>
				/// <param name="newInnerException">Об'єкт який містить нову помилку</param>
				ForceInline void SetInnerException(const ExceptionHolderType& newInnerException) {
					innerException = newInnerException;
				}

				/// <summary>
				/// Встановлює об'єкт який містить внутрішню помилку(об'єкт може бути пустий)
				/// </summary>
				/// <param name="newInnerException">Об'єкт який містить нову помилку</param>
				ForceInline void SetInnerException(ExceptionHolderType&& newInnerException) {
					innerException = MoveRValue(newInnerException);
				}

				/// <summary>
				/// Встановлює об'єкт який містить внутрішню помилку(об'єкт може бути пустий)
				/// </summary>
				/// <param name="newInnerException">Покажчик на внутрішню помилку</param>
				ForceInline void SetInnerException(Exception*&& newInnerException) {
					innerException = ExceptionHolderType(MoveRValue(newInnerException));
				}

				///<summary>Користувальницька назва помилки</summary>
				ConstString Property(GetExceptioName) Name;
				///<summary>Місце генерації помилки</summary>
				ExeptionLocation Property(GetLocation) Location;
				///<summary> Стек викликів на момент генерації помилки </summary>
				StackTraceRecord Property(GetStackTrace) StackTrace;
				/// <summary> Внутрішня помилка якщо вона є </summary>
				ExceptionHolderType Property(GetInnerException, SetInnerException) InnerException;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const {
					return ExceptionString::Combine({
						ExceptionString(L"Невідома помилка з повідомлення "), ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				///<summary>Деструктор оголошений для правильного знищення об'єктів які наслідуються</summary>
				virtual ~Exception() {}
			public:
				/// <summary>
				/// Не оброблена помилка
				/// </summary>
				class RawException final : IDisposable {
				private:
					struct Data : Memory::Allocator<Memory::ExceptionHeap> {
						void* ptr;
						RefCountT RefCount;

						Data(void* ptr) : ptr(ptr), RefCount(1) {}
					};
				private:
					Data * This;
				private:
					RawException(void* ptr) : This(new Data(ptr)) {};
				protected:
					virtual void Dispose() override final;
				public:
					/// <summary>
					/// Пустий конструктор
					/// </summary>
					RawException() :This(new Data(nullptr)) {};

					/// <summary>
					/// Генерує збережену помилку
					/// </summary>
					void ThrowThis() {
						ThrowRawException(*this);
					}

					RefCountClass(RawException, This);
				public:
					friend Exception;
				};
			public:
				/// <summary>
				/// Зберігає поточний об'єкт помилкт
				/// </summary>
				/// <returns></returns>
				static RawException StoreCurrentException();
				/// <summary>
				/// Генерує збережену помилку
				/// </summary>
				/// <param name="exception">Збережена помилка</param>
				static void ThrowRawException(RawException& exception);
			public:
				/// <summary>
				/// Клонує помилку
				/// </summary>
				/// <returns></returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new Exception(*this));
				}
			};

			///<summary>Ділення на ноль</summary>
			class DivideByZero final : public Exception {
			public:
				DivideByZero(const DivideByZero& from) :Exception(from) {}
				DivideByZero(DivideByZero&& from) :Exception(MoveRValue(from)) {}
				DivideByZero& operator=(const DivideByZero& from) { this->~DivideByZero(); this->DivideByZero::DivideByZero(from); return *this; }
				DivideByZero& operator=(DivideByZero&& from) { this->~DivideByZero(); this->DivideByZero::DivideByZero(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				DivideByZero(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Ділення на ноль")
				{}

				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				virtual ~DivideByZero() {};
			public:
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new DivideByZero(*this));
				}
			};

			///<summary>Помилка доступу до комірки пам'яті
			///<para>Буває трьох варінтів</para>
			///<para> - Read	- спроба прочитати за неіснуючим адресом</para>
			///<para> - Write	- спроба записати за неіснуючим адресом</para>
			///<para> - Execute - спроба виконати команду за неіснуючим адресом</para>
			///<para>Тому помилка містить інформацію про тип доступу та адресс який пробували використати</para>
			///</summary>
			class NullPointException final : public Exception {
			public:
				///<summary> Можливі варіанти помилки</summary>
				enum class Types : UInt8 { Read, Write, Execute };
			private:
				///<summary>Адресс який спробували використати</summary>
				SizeT Addr;
				///<summary>Тип доступу який згенерував помилку</summary>
				Types Type;
			public:
				NullPointException(const NullPointException& from) : Exception(from), Addr(from.Addr), Type(from.Type) {}
				NullPointException(NullPointException&& from) :Exception(MoveRValue(from)), Addr(from.Addr), Type(from.Type) {}
				NullPointException& operator=(const NullPointException& from) { this->~NullPointException(); this->NullPointException::NullPointException(from); return *this; }
				NullPointException& operator=(NullPointException&& from) { this->~NullPointException(); this->NullPointException::NullPointException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Addr'>Адрес комірки пам'яті яка згенерувала помилку</param>
				///<param mame = 'Type'>Тип доступу до комірки який згенерував помилку</param>
				NullPointException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT Addr, Types Type)
					:Exception(File, FunctName, Line, L"Помилка доступу до пам'яті"), Addr(Addr), Type(Type)
				{}

				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Addr'>Адрес комірки пам'яті яка згенерувала помилку</param>
				///<param mame = 'Type'>Тип доступу до комірки який згенерував помилку</param>
				NullPointException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Помилка доступу до пам'яті"), Addr(0x0), Type(Types::Read)
				{}

				///<summary>Адрес комірки пам'яті яка згенерувала помилку</summary>
				SizeT GetMemoryAddress() const { return Addr; }
				///<summary>Тип доступу до комірки який згенерував помилку</summary>
				Types GetAccessType() const { return Type; }

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L" за адресом "), ExceptionString::UInt64asHex(GetMemoryAddress()),
						ExceptionString(L" при спробі "),
						((GetAccessType() == Types::Read) ? ExceptionString(L"читання. ") :
						(GetAccessType() == Types::Write) ? ExceptionString(L"запису. ")
															: ExceptionString(L"виконання. ")),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L". ") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}
			public:
				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NullPointException(*this));
				}

				virtual ~NullPointException() {}
			};

			///<summary>Використання невалідного дескриптора</summary>
			class InvalidHandleUsage final : public Exception {
			public:
				InvalidHandleUsage(const InvalidHandleUsage& orig) : Exception(orig) {}
				InvalidHandleUsage(InvalidHandleUsage&& from) :Exception(MoveRValue(from)) {}
				InvalidHandleUsage& operator=(const InvalidHandleUsage& from) { this->~InvalidHandleUsage(); this->InvalidHandleUsage::InvalidHandleUsage(from); return *this; }
				InvalidHandleUsage& operator=(InvalidHandleUsage&& from) { this->~InvalidHandleUsage(); this->InvalidHandleUsage::InvalidHandleUsage(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidHandleUsage(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Використання не валідного дескриптора")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidHandleUsage(*this));
				}

				virtual ~InvalidHandleUsage() {}
			};

			/// <summary>
			/// Не знайдено 
			/// </summary>
			class NotFoundException : public Exception {
			public:
				NotFoundException(const NotFoundException& orig) : Exception(orig) {}
				NotFoundException(NotFoundException&& from) :Exception(MoveRValue(from)) {}
				NotFoundException& operator=(const NotFoundException& from) { this->~NotFoundException(); this->NotFoundException::NotFoundException(from); return *this; }
				NotFoundException& operator=(NotFoundException&& from) { this->~NotFoundException(); this->NotFoundException::NotFoundException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				NotFoundException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Не знайдено")
				{}

				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = "Msg">Повідомлення</param>
				NotFoundException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ConstString& Msg)
					:Exception(File, FunctName, Line, Msg)
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NotFoundException(*this));
				}

				virtual ~NotFoundException() {}
			};

			///<summary>Неможливо перетворити тип</summary>
			class BadCast final : public Exception {
			public:
				BadCast(const BadCast& orig) : Exception(orig) {}
				BadCast(BadCast&& from) :Exception(MoveRValue(from)) {}
				BadCast& operator=(const BadCast& from) { this->~BadCast(); this->BadCast::BadCast(from); return *this; }
				BadCast& operator=(BadCast&& from) { this->~BadCast(); this->BadCast::BadCast(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadCast(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо пертворити тип")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), ExceptionString(L". ")
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new BadCast(*this));
				}

				virtual ~BadCast() {}
			};

			///<summary>Неможливо викликати віртуальну функцію</summary>
			class BadPureCall final : public Exception {
			public:
				BadPureCall(const BadPureCall& orig) : Exception(orig) {}
				BadPureCall(BadPureCall&& from) :Exception(MoveRValue(from)) {}
				BadPureCall& operator=(const BadPureCall& from) { this->~BadPureCall(); this->BadPureCall::BadPureCall(from); return *this; }
				BadPureCall& operator=(BadPureCall&& from) { this->~BadPureCall(); this->BadPureCall::BadPureCall(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadPureCall(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати віртуальну функцію")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new BadPureCall(*this));
				}

				virtual ~BadPureCall() {}
			};

			///<summary>Вже існує</summary>
			class AllredyExistException : public Exception {
			public:
				AllredyExistException(const AllredyExistException& orig) : Exception(orig) {}
				AllredyExistException(AllredyExistException&& from) : Exception(MoveRValue(from)) {}
				AllredyExistException& operator=(const AllredyExistException& from) { this->~AllredyExistException(); this->AllredyExistException::AllredyExistException(from); return *this; }
				AllredyExistException& operator=(AllredyExistException&& from) { this->~AllredyExistException(); this->AllredyExistException::AllredyExistException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				AllredyExistException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Вже існує")
				{}

				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				AllredyExistException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ConstString& Msg)
					:Exception(File, FunctName, Line, Msg)
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new AllredyExistException(*this));
				}

				virtual ~AllredyExistException() {}
			};

			///<summary>Ключ вжк існує</summary>
			class KeyAllredyExistException : public AllredyExistException {
			public:
				KeyAllredyExistException(const KeyAllredyExistException& orig) : AllredyExistException(orig) {}
				KeyAllredyExistException(KeyAllredyExistException&& from) :AllredyExistException(MoveRValue(from)) {}
				KeyAllredyExistException& operator=(const KeyAllredyExistException& from) { this->~KeyAllredyExistException(); this->KeyAllredyExistException::KeyAllredyExistException(from); return *this; }
				KeyAllredyExistException& operator=(KeyAllredyExistException&& from) { this->~KeyAllredyExistException(); this->KeyAllredyExistException::KeyAllredyExistException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				KeyAllredyExistException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:AllredyExistException(File, FunctName, Line, L"Ключ вже існує")
				{}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new KeyAllredyExistException(*this));
				}

				virtual ~KeyAllredyExistException() {}
			};

			///<summary>Некоректний тип помилки</summary>
			class BadException final : public Exception {
			public:
				BadException(const BadException& orig) : Exception(orig) {}
				BadException(BadException&& from) :Exception(MoveRValue(from)) {}
				BadException& operator=(const BadException& from) { this->~BadException(); this->BadException::BadException(from); return *this; }
				BadException& operator=(BadException&& from) { this->~BadException(); this->BadException::BadException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Не коректний тип помилки")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new BadException(*this));
				}

				virtual ~BadException() {}
			};

			///<summary>Недостатньо оперативної пам'яті для операції</summary>
			class OutOfMemory final : public Exception {
			private:
				///<summary>Ім'я кучі</summary>
				ConstString HeapName;
				///<summary>Кількість байт які пробували виділити</summary>
				SizeT TryAllocate;
				///<summary>Кількість байт зайннятих до виділення</summary>
				SizeT AllocatedBefore;
				///<summary>Кількість елементів зайннятих до</summary>
				SizeT MemBlkscountBefore;
			public:
				OutOfMemory(const OutOfMemory& orig) :
					Exception(orig),
					HeapName(orig.HeapName),
					TryAllocate(orig.TryAllocate),
					AllocatedBefore(orig.AllocatedBefore),
					MemBlkscountBefore(orig.MemBlkscountBefore) {}

				OutOfMemory(OutOfMemory&& orig) : 
					Exception(MoveRValue(orig)),
					HeapName(orig.HeapName),
					TryAllocate(orig.TryAllocate),
					AllocatedBefore(orig.AllocatedBefore),
					MemBlkscountBefore(orig.MemBlkscountBefore) {}

				OutOfMemory& operator=(const OutOfMemory& from) { this->~OutOfMemory(); this->OutOfMemory::OutOfMemory(from); return *this; }
				OutOfMemory& operator=(OutOfMemory&& from) { this->~OutOfMemory(); this->OutOfMemory::OutOfMemory(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'HeapName'>Ім'я кучі з маленької літери</param>
				///<param name = 'TryAllocate'>Кількість байт які пробували виділити</param>
				///<param name = 'AllocatedBefore'>Кількість байт які були виділені на момент генерації виключення</param>
				///<param name = 'MemBlksCountBefore'>Кількість блоків які були виділені на момент генерації виключення</param>
				OutOfMemory(
					const ConstAString& File,
					const ConstAString& FunctName,
					SizeT Line,
					const ConstString& HeapName,
					SizeT TryAllocate,
					SizeT AllocatedBefore,
					SizeT MemBlkscountBefore
				) :
					Exception(File, FunctName, Line, L"Недостатньо оперативної пам'яті для операції"),
					TryAllocate(TryAllocate), AllocatedBefore(AllocatedBefore), MemBlkscountBefore(MemBlkscountBefore), HeapName(HeapName)
				{}

				///<summary>Вертає ім'я кучі в якій пробували виділити пам'ять</summary>
				const ConstString& GetHeapName() const { return HeapName; }
				///<summary>Вертає кількість байт які пробували виділити</summary>
				SizeT GetRequiredBytes() const { return TryAllocate; }
				///<summary>Вертає кількість байт які були виділені на момент виникнення помилки</summary>
				SizeT GetAllocatedBytes() const { return AllocatedBefore; }
				///<summary>Вертає кількість блоків пам'яті які були виділенні на момент виникненя помилки</summary>
				SizeT GetAllocatedMemBlksCount() const { return MemBlkscountBefore; }

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L" в "), ExceptionString(HeapName), ExceptionString(L" кучі "),
						ExceptionString(L" при виділені "), ExceptionString::toString(TryAllocate),
						ExceptionString(L" байт, на момент виникнення помилки було виділено "),
						ExceptionString::toString(AllocatedBefore), ExceptionString(L" байт в "),
						ExceptionString::toString(MemBlkscountBefore),
						ExceptionString((MemBlkscountBefore == 1) ? ExceptionString(L" блоку пам'яті. ") : (L"блоках пам'яті. ")),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new OutOfMemory(*this));
				}

				virtual ~OutOfMemory() {}
			};

			///<summary>Внутрішня помилка ядра</summary>
			class InternalError final : public Exception {
			public:
				InternalError(const InternalError& orig) : Exception(orig) {}
				InternalError(InternalError&& from) :Exception(MoveRValue(from)) {}
				InternalError& operator=(const InternalError& from) { this->~InternalError(); this->InternalError::InternalError(from); return *this; }
				InternalError& operator=(InternalError&& from) { this->~InternalError(); this->InternalError::InternalError(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InternalError(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Внутрішня помилка ядра")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), ExceptionString(L". ")
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InternalError(*this));
				}

				virtual ~InternalError() {}
			};

			///<summary>Невірний формат</summary>
			class InvalidFormatException : public Exception {
			public:
				InvalidFormatException(const InvalidFormatException& orig) : Exception(orig) {}
				InvalidFormatException(InvalidFormatException&& from) :Exception(MoveRValue(from)) {}
				InvalidFormatException& operator=(const InvalidFormatException& from) { this->~InvalidFormatException(); this->InvalidFormatException::InvalidFormatException(from); return *this; }
				InvalidFormatException& operator=(InvalidFormatException&& from) { this->~InvalidFormatException(); this->InvalidFormatException::InvalidFormatException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidFormatException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Невірний формат")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidFormatException(*this));
				}

				virtual ~InvalidFormatException() {}
			};

			///<summary>Невірний формат</summary>
			class NotImplementedException final : public Exception {
			public:
				NotImplementedException(const NotImplementedException& orig) : Exception(orig) {}
				NotImplementedException(NotImplementedException&& from) :Exception(MoveRValue(from)) {}
				NotImplementedException& operator=(const NotImplementedException& from) { this->~NotImplementedException(); this->NotImplementedException::NotImplementedException(from); return *this; }
				NotImplementedException& operator=(NotImplementedException&& from) { this->~NotImplementedException(); this->NotImplementedException::NotImplementedException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				NotImplementedException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Функція\\метод не реалізовані")
				{}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NotImplementedException(*this));
				}

				virtual ~NotImplementedException() {}
			};

			///<summary>Доступ до елемента який знаходится за межею розмірів</summary>
			class IndexOutOfRange : public Exception {
			private:
				SizeT Index;
				SizeT MaxIndex;
			public:
				IndexOutOfRange(const IndexOutOfRange& orig) : Exception(orig), Index(orig.Index), MaxIndex(orig.MaxIndex) {}
				IndexOutOfRange(IndexOutOfRange&& from) :Exception(MoveRValue(from)), Index(from.Index), MaxIndex(from.MaxIndex) {}
				IndexOutOfRange& operator=(const IndexOutOfRange& from) { this->~IndexOutOfRange(); this->IndexOutOfRange::IndexOutOfRange(from); return *this; }
				IndexOutOfRange& operator=(IndexOutOfRange&& from) { this->~IndexOutOfRange(); this->IndexOutOfRange::IndexOutOfRange(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequiredIndex'>Індекс елемента який пробували отримати</param>
				///<param name = 'MaxAllowedIndex'>Максимальний допустимий індекс</param>
				ForceInline IndexOutOfRange(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT RequiredIndex, SizeT MaxAllowedIndex)
					: Exception(File, FunctName, Line, L"Вихід за межі"),
					Index(RequiredIndex), MaxIndex(MaxAllowedIndex)
				{}

				///<summary>Запрошуваний індекс</summary>
				ForceInline SizeT GetRequiredIndex() const { return Index; }
				///<summary>Максимальний достуаний індекс</summary>
				ForceInline SizeT GetMaxAllowedIndex() const { return MaxIndex; }

				///<summary>Запрошуваний індекс</summary>
				SizeT Property(GetRequiredIndex) RequiredIndex;
				///<summary>Максимальний достуаний індекс</summary>
				SizeT Property(GetMaxAllowedIndex) MaxAllowedIndex;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". При спробі отримати доступ до елемента за індексом "), ExceptionString::toString(RequiredIndex),
						ExceptionString(L" якщо максимальний дозволений індекс "), ExceptionString::toString(MaxAllowedIndex),
						ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new IndexOutOfRange(*this));
				}

				virtual ~IndexOutOfRange() {}
			};

			///<summary>Некоректний тип</summary>
			class InvalidTypeException final : public Exception {
			private:
				const TypeInfo& requiredType;
				const TypeInfo& actuallyType;
			public:
				InvalidTypeException(const InvalidTypeException& orig) : Exception(orig), requiredType(orig.requiredType), actuallyType(orig.actuallyType) {}
				InvalidTypeException(InvalidTypeException&& from) :Exception(MoveRValue(from)), requiredType(from.requiredType), actuallyType(from.actuallyType) {}
				InvalidTypeException& operator=(const InvalidTypeException& from) { this->~InvalidTypeException(); this->InvalidTypeException::InvalidTypeException(from); return *this; }
				InvalidTypeException& operator=(InvalidTypeException&& from) { this->~InvalidTypeException(); this->InvalidTypeException::InvalidTypeException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequireType'>Інформація про очікуваний тип</param>
				///<param name = 'ActuallyType'>Інформація про фактичний тип</param>
				ForceInline InvalidTypeException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const TypeInfo& RequiredType, const TypeInfo& ActuallyType)
					: Exception(File, FunctName, Line, L"Не вірний тип"),
					requiredType(RequiredType), actuallyType(ActuallyType)
				{
				}

				///<summary>Інформація про очікуваний тип</summary>
				const TypeInfo& GetRequiredType() const { return requiredType; }
				///<summary>Інформація про фактичний тип</summary>
				const TypeInfo& GetActuallyType() const { return actuallyType; }

				///<summary>Інформація про очікуваний тип</summary>
				const TypeInfo Property(GetRequiredType) RequiredType;
				///<summary>Інформація про фактичний тип</summary>
				const TypeInfo Property(GetActuallyType) ActuallyType;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Очікуваний тип: "), ExceptionString((Char*) RequiredType.GetCxxName()),
						ExceptionString(L". Фактичний тип: "), ExceptionString((Char*) ActuallyType.GetCxxName()),
						ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidTypeException(*this));
				}

				virtual ~InvalidTypeException() {}
			};

			///<summary>Невірний аргумент</summary>
			class InvalidArgumentException final : public Exception {
			private:
				///<summary>Ім'я аргумента</summary>
				ConstString ArgNameValue;
				///<summary>Додаткова інформація</summary>
				ConstString DescriptionValue;
			public:
				InvalidArgumentException(const InvalidArgumentException& orig) :
					Exception(orig),
					ArgNameValue(orig.ArgNameValue),
					DescriptionValue(orig.DescriptionValue) {}

				InvalidArgumentException(InvalidArgumentException&& from) :
					Exception(from),
					ArgNameValue(from.ArgNameValue),
					DescriptionValue(from.DescriptionValue) {}

				InvalidArgumentException& operator=(const InvalidArgumentException& from) { this->~InvalidArgumentException(); this->InvalidArgumentException::InvalidArgumentException(from); return *this; }
				InvalidArgumentException& operator=(InvalidArgumentException&& from) { this->~InvalidArgumentException(); this->InvalidArgumentException::InvalidArgumentException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'ArgName'>Ім'я аргумента</param>
				///<param name = 'Description'>Додаткова інформація</param>
				InvalidArgumentException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ConstString& ArgName, const ConstString& Description) :
					Exception(File, FunctName, Line, L"Невірний аргумент"), ArgNameValue(ArgName), DescriptionValue(Description)
				{}

				///<summary>Вертає ім'я аргумента</summary>
				const ConstString& GetArgName() { return ArgNameValue; }
				///<summary>Вертає додаткову інформацію</summary>
				const ConstString& GetDescription() { return DescriptionValue; }

				///<summary>Ім'я аргумента</summary>
				ConstString Property(ArgNameValue) ArgumentPosition;
				///<summary>Додаткова інформація</summary>
				ConstString Property(GetDescription) Description;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Невірний "), ArgNameValue,
						ExceptionString(L" аргумент. Додатковий опис: "),
						ExceptionString(DescriptionValue), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InvalidArgumentException(*this));
				}

				virtual ~InvalidArgumentException() {}
			};

			///<summary>Операцію перервано</summary>
			class InteruptException final : protected Exception {
			public:
				InteruptException(const InteruptException& orig) : Exception(orig) {}
				InteruptException(InteruptException&& from) : Exception(MoveRValue(from)) {}
				InteruptException& operator=(const InteruptException& from) { this->~InteruptException(); this->InteruptException::InteruptException(from); return *this; }
				InteruptException& operator=(InteruptException&& from) { this->~InteruptException(); this->InteruptException::InteruptException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline InteruptException(const ConstAString & File, const ConstAString & FunctName, SizeT Line)
					: Exception(File, FunctName, Line, L"Операція перервана")
				{};

				///<summary>Вертає місце генерації помилки</summary>
				ForceInline const ExeptionLocation& GetLocation() const
				{
					return location;
				}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new InteruptException(*this));
				}

				virtual ~InteruptException() {}
			};

			///<summary>Рядок має завеликий розмір</summary>
			class StringToLong final : public Exception {
			private:
				///<summary>Запрошений розмір</summary>
				SizeT requiredSize;
			public:
				StringToLong(const StringToLong& orig) : Exception(orig), requiredSize(orig.requiredSize) {}
				StringToLong(StringToLong&& from) : Exception(MoveRValue(from)), requiredSize(from.requiredSize) {}
				StringToLong& operator=(const StringToLong& from) { this->~StringToLong(); this->StringToLong::StringToLong(from); return *this; }
				StringToLong& operator=(StringToLong&& from) { this->~StringToLong(); this->StringToLong::StringToLong(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequiredSize'>Запрошений розмір</param>
				StringToLong(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT RequiredSize) :
					Exception(File, FunctName, Line, L"Рядок має завеликий розмір"), requiredSize(RequiredSize) {}

				///<summary>Вертає запрошений розмір</summary>
				SizeT GetRequiredSize() { return requiredSize; }
				///<summary>Запрошений розмір</summary>
				SizeT Property(GetRequiredSize) RequiredSize;

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Запрошений розмір "), ExceptionString::toString(requiredSize),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new StringToLong(*this));
				}
			};

			///<summary>Цілочисельне переповнення</summary>
			class IntegerOverflowException : public Exception {
			public:
				IntegerOverflowException(const IntegerOverflowException& orig) : Exception(orig) {}
				IntegerOverflowException(IntegerOverflowException&& from) : Exception(MoveRValue(from)) {}
				IntegerOverflowException& operator=(const IntegerOverflowException& from) { this->~IntegerOverflowException(); this->IntegerOverflowException::IntegerOverflowException(from); return *this; }
				IntegerOverflowException& operator=(IntegerOverflowException&& from) { this->~IntegerOverflowException(); this->IntegerOverflowException::IntegerOverflowException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				IntegerOverflowException(const ConstAString& File, const ConstAString& FunctName, SizeT Line) :
					Exception(File, FunctName, Line, L"Цілочисельне переповнення") {}

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new IntegerOverflowException(*this));
				}

				virtual ~IntegerOverflowException() {}
			};

			///<summary>Ітератор на кінець не є визначеним</summary>
			class EndNodeNotDeferencable final : public Exception {
			public:
				EndNodeNotDeferencable(const EndNodeNotDeferencable& orig) : Exception(orig) {}
				EndNodeNotDeferencable(EndNodeNotDeferencable&& from) : Exception(MoveRValue(from)) {}
				EndNodeNotDeferencable& operator=(const EndNodeNotDeferencable& from) { this->~EndNodeNotDeferencable(); this->EndNodeNotDeferencable::EndNodeNotDeferencable(from); return *this; }
				EndNodeNotDeferencable& operator=(EndNodeNotDeferencable&& from) { this->~EndNodeNotDeferencable(); this->EndNodeNotDeferencable::EndNodeNotDeferencable(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				ForceInline EndNodeNotDeferencable(const ConstAString& File, const ConstAString& FunctName, SizeT Line) noexcept
					: Exception(File, FunctName, Line, L"Ітератор на кінець не є визначеним")
				{};

				///<summary>Текстове представлення інформації про помилки помилки</summary>
				virtual ExceptionString toString() const override final {
					return ExceptionString::Combine({
						ExceptionString(exceptionName), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new EndNodeNotDeferencable(*this));
				}

				virtual ~EndNodeNotDeferencable() {}
			};

			/// <summary>
			/// Виключення в NT функції
			/// </summary>
			class NtException : public Exception {
			private:
				///<summary>NtStatus code операції</summary>
				Int32 statusCode;
			public:
				NtException(const NtException& orig) : Exception(orig), statusCode(orig.statusCode) {}
				NtException(NtException&& from) : Exception(MoveRValue(from)), statusCode(from.statusCode) {}
				NtException& operator=(const NtException& from) { this->~NtException(); this->NtException::NtException(from); return *this; }
				NtException& operator=(NtException&& from) { this->~NtException(); this->NtException::NtException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'StatusCode'>Код помилки</param>
				NtException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, Int32 StatusCode)
					: Exception(File, FunctName, Line, L"Помилка системного виклику NT"), statusCode(StatusCode)
				{}
			public:
				/// <summary>
				/// Повертає код помилки
				/// </summary>
				/// <returns>Код помилки</returns>
				Int32 GetCode() const { return statusCode; }

				/// <summary>
				/// Код помилки
				/// </summary>
				Int32 Property(GetCode) Code;
			public:
				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Системний виклик вернув код "),
						BaseString<Memory::ExceptionHeap>::Int32asHex(statusCode), ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new NtException(*this));
				}

				virtual ~NtException() {}
			};

			/// <summary>
			/// Переповнення стеку
			/// </summary>
			class FullStackException : public Exception {
			private:
				///<summary>Максимальний допустимий розмір стеку</summary>
				SizeT maxSize;
			public:
				FullStackException(const FullStackException& orig) : Exception(orig), maxSize(orig.maxSize) {}
				FullStackException(FullStackException&& from) : Exception(MoveRValue(from)), maxSize(from.maxSize) {}
				FullStackException& operator=(const FullStackException& from) { this->~FullStackException(); this->FullStackException::FullStackException(from); return *this; }
				FullStackException& operator=(FullStackException&& from) { this->~FullStackException(); this->FullStackException::FullStackException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'maxSize'>Максимальний допустимий розмір стеку</param>
				FullStackException(const ConstAString& File, const ConstAString& FuncName, SizeT Line, SizeT maxSize)
					: Exception(File, FuncName, Line, L"Перевищено максимальний розмір стеку"), maxSize(maxSize)
				{}

				/// <summary>
				/// Getter для максимального розміру стеку
				/// </summary>
				SizeT GetMaxSize() const {
					return maxSize;
				}

				///<summary>Максимальний допустимий розмір стеку</summary>
				SizeT Property(GetMaxSize) MaxSize;

				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Максимальний допустимий розмір - "),
						ExceptionString::toString(maxSize),
						ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
					});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new FullStackException(*this));
				}

				virtual ~FullStackException() {}
			};

			/// <summary>
			/// Порожній стек
			/// </summary>
			class EmptyStackException : public Exception {
			public:
				EmptyStackException(const EmptyStackException& orig) : Exception(orig) {}
				EmptyStackException(EmptyStackException&& from) : Exception(MoveRValue(from)) {}
				EmptyStackException& operator=(const EmptyStackException& from) { this->~EmptyStackException(); this->EmptyStackException::EmptyStackException(from); return *this; }
				EmptyStackException& operator=(EmptyStackException&& from) { this->~EmptyStackException(); this->EmptyStackException::EmptyStackException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				EmptyStackException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
					: Exception(File, FuncName, Line, L"Спроба вийняти елементи з пустого стеку.")
				{}

				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new EmptyStackException(*this));
				}

				virtual ~EmptyStackException() {}
			};

			/// <summary>
			/// Переповнення черги
			/// </summary>
			class FullQueueException : public Exception {
			private:
				///<summary>Максимальний допустимий розмір черги</summary>
				SizeT maxSize;
			public:
				FullQueueException(const FullQueueException& orig) : Exception(orig), maxSize(orig.maxSize) {}
				FullQueueException(FullQueueException&& from) : Exception(MoveRValue(from)), maxSize(from.maxSize) {}
				FullQueueException& operator=(const FullQueueException& from) { this->~FullQueueException(); this->FullQueueException::FullQueueException(from); return *this; }
				FullQueueException& operator=(FullQueueException&& from) { this->~FullQueueException(); this->FullQueueException::FullQueueException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'maxSize'>Максимальний допустимий розмір черги</param>
				FullQueueException(const ConstAString& File, const ConstAString& FuncName, SizeT Line, SizeT maxSize)
					: Exception(File, FuncName, Line, L"Перевищено максимальний розмір черги"), maxSize(maxSize)
				{}

				/// <summary>
				/// Getter для максимального розміру розміру черги
				/// </summary>
				SizeT GetMaxSize() const {
					return maxSize;
				}

				///<summary>Максимальний допустимий розмір стеку</summary>
				SizeT Property(GetMaxSize) MaxSize;

				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						ExceptionString(L". Максимальний допустимий розмір - "),
						ExceptionString::toString(maxSize),
						ExceptionString(L". "),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new FullQueueException(*this));
				}

				virtual ~FullQueueException() {}
			};

			/// <summary>
			/// Порожня черга
			/// </summary>
			class EmptyQueueException : public Exception {
			public:
				EmptyQueueException(const EmptyQueueException& orig) : Exception(orig) {}
				EmptyQueueException(EmptyQueueException&& from) : Exception(MoveRValue(from)) {}
				EmptyQueueException& operator=(const EmptyQueueException& from) { this->~EmptyQueueException(); this->EmptyQueueException::EmptyQueueException(from); return *this; }
				EmptyQueueException& operator=(EmptyQueueException&& from) { this->~EmptyQueueException(); this->EmptyQueueException::EmptyQueueException(MoveRValue(from)); return *this; }
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				EmptyQueueException(const ConstAString& File, const ConstAString& FuncName, SizeT Line)
					: Exception(File, FuncName, Line, L"Спроба вийняти елементи з пустої черги.")
				{}

				///<summary>Текстове представлення інформації про помилку</summary>
				virtual ExceptionString toString() const override {
					return ExceptionString::Combine({
						ExceptionString(exceptionName),
						location.toString(), 
						(innerException == nullptr) ? ExceptionString(L".") : (L". Також є вкладена помилка: " + InnerException->toString())
						});
				}

				/// <summary>
				/// Вертає копію об'єкта помилки
				/// </summary>
				/// <returns>Розумний покажчик на об'єкт помилки</returns>
				virtual ExceptionHolderType Clone() const override {
					return ExceptionHolderType(new EmptyQueueException(*this));
				}

				virtual ~EmptyQueueException() {}
			};
		}

		namespace Concepts
		{
			/// <summary>
			/// Перевірка чи заданий тип є типом помилки(Seed::Core::Exception або похідний від нього клас)
			/// </summary>
			template<typename T>
			concept IsException = IsSame<T, Exceptions::Exception> || IsBasedOn<Exceptions::Exception, T> || IsSame<T, InteruptException>;
		}
	}
}

#pragma region Throw
///<summary>Вертає тип або тип результату функції\методу<\summary>
template<typename Type>
using ThrowableObj = typename Seed::Core::TypeTraits::ConditionT <
	Seed::Core::TypeTraits::IsFunction<Type>::State,
	typename Seed::Core::TypeTraits::ResultOfT<Type>::Type,
	Type
>::Result;

///<summary>Вертає <c>True</c> якщо цей об'єкт ж помилкою чи вертає в якості результату об'єкт помилки</summary>
template<typename Type>
struct IsThrowable :
	Seed::Core::TypeTraits::IsBasedOn<
	Seed::Core::Exceptions::Exception,
	Seed::Core::TypeTraits::RemoveRefV<ThrowableObj<Type>>
	>
{};

///<summary>Внутрішня функція генерації помилки</summary>
///<param name = 'ex'>Об'єкт помилки</param>
template<typename E> InternalApiCall ForceInline void __declspec(noreturn) ___cxxthrow(E&& ex) { throw static_cast<E&>(ex); }

///<summary>Внутрішня функція пркидування поточної помилки</summary>
InternalApiCall ForceInline void __declspec(noreturn) __cxxrethrow() { throw; } //-V667

#ifndef CXXTHROWAPI
	///<summary>Генерація помилки з провіркою типу</summary>
	///<param name = 'ExceptionObj'>Об'єкт помилки або функція яка вертає такий об'єкт</param>
	#define Throw(ExceptionObj) \
	do{ \
		static_assert(IsThrowable<decltype(ExceptionObj)>::State || \
			Seed::Core::TypeTraits::IsSameT<Seed::Exceptions::InteruptException, decltype(ExceptionObj)>::State, \
		"Only Seed::Exception and derived classes cant be throw as exception"); \
		__assume(IsThrowable<decltype(ExceptionObj)>()); \
		__pragma(warning(push)); \
		__pragma(warning(disable: 4996)); \
		___cxxthrow(ExceptionObj);\
		__pragma(warning(pop)); \
	 }while(false)

	#define ReThrow \
	do{\
	__pragma(warning(push)); \
	__pragma(warning(disable: 4996)); \
	__cxxrethrow(); \
	__pragma(warning(pop)); \
	} while(false)

	///<summary>Заборона використання <c>throw</c> без провірок на тип об'єкта помилки</summary>
	#define throw static_cast(false, "Use Throw insted")

	///<summary>Макрос локалізації помилки</summary>
	#define ExceptionPosition __FILE__, __FUNCSIG__, __LINE__
#endif // !CXXTHROWAPI

#pragma endregion

#pragma warning(pop)