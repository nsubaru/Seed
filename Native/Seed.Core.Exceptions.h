#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "typeinfo"

#pragma warning(push)
#pragma warning(disable: 4996)
namespace Seed {
	inline namespace Core {
		namespace Memory {
			class ExceptionHeap : public BasicHeap {
			public:
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Allocate(SizeT Size) final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedAllocate(SizeT Size, SizeT AllocationReq) final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Free(void * MemBlk) final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedFree(void * MemBlk, SizeT AllocationReq) final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() noexcept final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentAllocatedMemory() noexcept final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentHeapUsage() noexcept final;
			};
		}
		///<summary>Підсистема помилок в рантаймі</summary>
		inline namespace Exceptions {
			///<summary>Макрос локалізації помилки</summary>
#define ExceptionPosition __FILE__, __FUNCSIG__, __LINE__

			///<summary>Класс базової помилки</summary>
			class SeedKernelCall Exception : virtual public Memory::Allocator<Memory::ExceptionHeap> {
			public:
				///<sumary>Місце генерації помилки</summary>
				struct ExeptionLocation {
					///<summary>Ім'я файла</summary>
					ConstACharStr File;
					///<summary>Ім'я функції</summary>
					ConstACharStr FunctName;
					///<summary>Номер рядка</summary>
					SizeT Line;
					///<summary>ID потоку</summary>
					ID ThreadID;
				};
			protected:
				///<summary>Місце генерації даної помилки</summary>
				ExeptionLocation location;
				///<summary>Ім'я помилки</summary>
				const Char * exeptionName;
			public:
				//TODO: Modify ctor by receiving size of exceptio name string by template
				//TODO: Set thread id
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Name'>Ім'я помилки</param>
				///<returns>Екземпляр класу Exeption</returns>
				__forceinline Exception(ConstACharStr File, ConstACharStr FunctName, SizeT Line, ConstCharStr ExeptionName) noexcept
					:location({ File,FunctName,Line, 0 }), exeptionName(ExeptionName) {}

				///<summary>Вертає користувальницьку назву помилки</summary>
				__forceinline ConstCharStr GetExceptioName() const
				{
					return exeptionName;
				}

				///<summary>Вертає місце генерації помилки</summary>
				__forceinline const ExeptionLocation & GetLocation() const
				{
					return location;
				}

				///<summary>Користувальницька назва помилки</summary>
				ConstCharStr Property(GetExceptioName) Name;
				///<summary>Місце генерації помилки</summary>
				ExeptionLocation Property(GetLocation) Location;

				///<summary>Деструктор оголошений для правильного знищення об'єктів які наслідуються</summary>
				virtual ~Exception() {}
			};

			///<summary>Ділення на ноль</summary>
			class SeedKernelCall DivideByZero : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				DivideByZero(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Ділення на ноль")
				{}
			};

			///<summary>Помилка доступу до комірки пам'яті
			///<para>Буває трьох варінтів</para>
			///<para> - Read	- спроба прочитати за неіснуючим адресом</para>
			///<para> - Write	- спроба записати за неіснуючим адресом</para>
			///<para> - Execute - спроба виконати команду за неіснуючим адресом</para>
			///<para>Тому помилка містить інформацію про тип доступу та адресс який пробували використати</para>
			///</summary>
			class NullPointException : public Exception {
			public:
				///<summary> Можливі варіанти помилки</summary>
				enum class Types : UInt8 { Read, Write, Execute };
			private:
				///<summary>Адресс який спробували використати</summary>
				SizeT Addr;
				///<summary>Тип доступу який згенерував помилку</summary>
				Types Type;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Addr'>Адрес комірки пам'яті яка згенерувала помилку</param>
				///<param mame = 'Type'>Тип доступу до комірки який згенерував помилку</param>
				NullPointException(ConstACharStr File, ConstACharStr FunctName, SizeT Line, SizeT Addr, Types Type)
					:Exception(File, FunctName, Line, L"Помилка доступу до пам'яті"), Addr(Addr), Type(Type)
				{}

				///<summary>Адрес комірки пам'яті яка згенерувала помилку</summary>
				SizeT GetMemoryAddress() { return Addr; }
				///<summary>Тип доступу до комірки який згенерував помилку</summary>
				Types GetAccessType() { return Type; }
			};

			///<summary>Використання невалідного дескриптора</summary>
			class InvalidHandleUsage : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidHandleUsage(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Використання не валідного дескриптора")
				{}
			};

			///<summary>Неможливо перетворити тип</summary>
			class SeedKernelCall BadCast : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadCast(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо пертворити тип")
				{}
			};

			///<summary>Неможливо викликати віртуальну функцію</summary>
			class SeedKernelCall BadPureCall : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadPureCall(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати віртуальну функцію")
				{}
			};

			///<summary>Некоректний тип помилки</summary>
			class SeedKernelCall BadException : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadException(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Не коректний тип помилки")
				{}
			};

			///<summary>Недостатньо оперативної пам'яті для операції</summary>
			class SeedKernelCall OutOfMemory :public Exception {
			private:
				///<summary>Кількість байт які пробували виділити</summary>
				SizeT TryAllocate;
				///<summary>Кількість байт зайннятих до виділення</summary>
				SizeT AllocatedBefore;
				///<summary>Кількість елементів зайннятих до</summary>
				SizeT MemBlkscountBefore;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'TryAllocate'>Кількість байт які пробували виділити</param>
				///<param name = 'AllocatedBefore'>Кількість байт які були виділені на момент генерації виключення</param>
				///<param name = 'MemBlksCountBefore'>Кількість блоків які були виділені на момент генерації виключення</param>
				OutOfMemory(
					ConstACharStr File,
					ConstACharStr FunctName,
					SizeT Line,
					SizeT TryAllocate,
					SizeT AllocatedBefore,
					SizeT MemBlkscountBefore
				) :
					Exception(File, FunctName, Line, L"Неможливо пертворити тип"),
					TryAllocate(TryAllocate), AllocatedBefore(AllocatedBefore), MemBlkscountBefore(MemBlkscountBefore)
				{}
				///<summary>Вертає кількість байт які пробували виділити</summary>
				SizeT GetRequiredBytes() { return TryAllocate; }
				///<summary>Вертає кількість байт які були виділені на момент виникнення помилки</summary>
				SizeT GetAllocatedBytes() { return AllocatedBefore; }
				///<summary>Вертає кількість блоків пам'яті які були виділенні на момент виникненя помилки</summary>
				SizeT GetAllocatedMemBlksCount() { return MemBlkscountBefore; }
			};

			///<summary>Внутрішня помилка ядра</summary>
			class SeedKernelCall InternalError : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InternalError(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Внутрішня помилка ядра")
				{}
			};

			///<summary>Невірний формат</summary>
			class SeedKernelCall InvalidFormat : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidFormat(ConstACharStr File, ConstACharStr FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Невірний формат")
				{}
			};

			///<summary>Доступ до елемента який знаходится за межею розмірів</summary>
			class SeedKernelCall IndexOutOfRange :public Exception {
			private:
				SizeT Index;
				SizeT MaxIndex;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequiredIndex'>Індекс елемента який пробували отримати</param>
				///<param name = 'MaxAllowedIndex'>Максимальний допустимий індекс</param>
				__forceinline IndexOutOfRange(ConstACharStr File, ConstACharStr FunctName, SizeT Line, SizeT RequiredIndex, SizeT MaxAllowedIndex)
					noexcept : Exception(File, FunctName, Line, L"Вихід за межі"),
					Index(RequiredIndex), MaxIndex(MaxAllowedIndex)
				{}

				///<summary>Запрошуваний індекс</summary>
				__forceinline SizeT GetRequiredIndex() { return Index; }
				///<summary>Максимальний достуаний індекс</summary>
				__forceinline SizeT GetMaxAllowedIndex() { return MaxIndex; }

				///<summary>Запрошуваний індекс</summary>
				SizeT Property(GetRequiredIndex) RequiredIndex;
				///<summary>Максимальний достуаний індекс</summary>
				SizeT Property(GetMaxAllowedIndex) MaxAllowedIndex;
			};

			///<summary>Некоректний тип</summary>
			class SeedKernelCall InvalidTypeException : public Exception {
			private:
				const TypeInfo & requireType;
				const TypeInfo & actuallyType;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequireType'>Інформація про очікуваний тип</param>
				///<param name = 'ActuallyType'>Інформація про фактичний тип</param>
				__forceinline InvalidTypeException(ConstACharStr File, ConstACharStr FunctName, SizeT Line, const TypeInfo & RequireType, const TypeInfo & ActuallyType)
					noexcept : Exception(File, FunctName, Line, L"Invalid type"),
					requireType(RequireType), actuallyType(ActuallyType)
				{
				}

				///<summary>Інформація про очікуваний тип</summary>
				const TypeInfo & GetRequireType() { return requireType; }
				///<summary>Інформація про фактичний тип</summary>
				const TypeInfo & GetActuallyType() { return actuallyType; }

				///<summary>Інформація про очікуваний тип</summary>
				const TypeInfo Property(GetRequireType) RequireType;
				///<summary>Інформація про фактичний тип</summary>
				const TypeInfo Property(GetActuallyType) ActuallyType;
			};

			///<summary>Операцію перервано</summary>
			class SeedKernelCall InteruptException : protected Exception
			{
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				__forceinline InteruptException(ConstACharStr File, ConstACharStr FunctName, SizeT Line) 
					noexcept : Exception(File, FunctName, Line, L"Операція перервана")
				{};

				///<summary>Вертає місце генерації помилки</summary>
				__forceinline const ExeptionLocation & GetLocation() const
				{
					return location;
				}
				
				virtual ~InteruptException() {}
			};
		}
	}
}

template<class Type>
using ThrowableObj = typename Seed::Core::StaticAsserts::ConditionT <
	Seed::Core::StaticAsserts::IsFunction<Type>::State,
	typename Seed::Core::StaticAsserts::ResultOf<Type>::Type,
	typename Type
>::Result;

template<typename Type>
struct IsThrowable :
	Seed::Core::StaticAsserts::IsBasedOn<
	Seed::Core::Exceptions::Exception,
	Seed::Core::StaticAsserts::RemoveRefT<ThrowableObj<Type>>
	>
{};

///<summary>Внутрішня функція генерації помилки</summary>
///<param name = 'ex'>Об'єкт помилки</param>
template<typename E> InternalApiCall __forceinline void __declspec(noreturn) ___cxxthrow(E && ex) { throw ex; }

///<summary>Генерація помилки з провіркою типу</summary>
///<param name = 'ExceptionObj'>Об'єкт помилки або функція яка вертає такий об'єкт</param>
#define Throw(ExceptionObj) \
do{ \
	static_assert(IsThrowable<decltype(ExceptionObj)>(), \
	"Only Seed::Exception and derived classes cant be throw as exception"); \
	__assume(IsThrowable<decltype(ExceptionObj)>()); \
	__pragma(warning(push)); \
	__pragma(warning(disable: 4996)); \
	___cxxthrow(ExceptionObj);\
	__pragma(warning(pop)); \
 }while(false)

#define throw static_cast("Use Throw insted")

#pragma warning(pop)