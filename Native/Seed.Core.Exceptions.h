#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		///<summary>Підсистема помилок в рантаймі</summary>
		inline namespace Exceptions {
			///<summary>Макрос локалізації помилки</summary>
#define ExeptionPosition __FILE__, __FUNCSIG__, __LINE__

			///<summary>Класс базової помилки</summary>
			class Exception {
			private:
				///<sumary>Місце генерації помилки</summary>
				struct ExeptionLocation {
					///<summary>Ім'я файла</summary>
					const AChar * File;
					///<summary>Ім'я функції</summary>
					const AChar * FunctName;
					///<summary>Номер рядка</summary>
					SizeT Line;
				};
				///<summary>Місце генерації даної помилки</summary>
				ExeptionLocation Location;
				///<summary>Ім'я помилки</summary>
				const Char * ExeptionName;

				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<returns>Виділену ділянку або <c>nullptr</c></returns>
				static DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall _Ret_notnull_ _Post_writable_byte_size_(Size) void * Allocate(SizeT Size) noexcept;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<returns>Виділену ділянку або <c>nullptr</c></returns>
				static DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall _Ret_notnull_ _Post_writable_byte_size_(Size) void * AllignedAllocate(SizeT Size, SizeT AllocationReq) noexcept;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				static DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Free(void * MemBlk);
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				static DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedFree(void * MemBlk, Seed::SizeT AllocationReq);
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Name'>Ім'я помилки</param>
				///<returns>Екземпляр класу Exeption</returns>
				__forceinline Exception(const AChar * File, const AChar * FunctName, SizeT Line, const Char * ExeptionName) noexcept
					:Location({ File,FunctName,Line }), ExeptionName(ExeptionName) {}

				///<summary>Деструктор оголошений для правильного знищення об'єктів які наслідуються</summary>
				virtual ~Exception() = 0 {}

#pragma warning(push)
#pragma warning(disable: 4996)
				///<summary>Оператор виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<returns>Вертає ділянку пам'яті</returns>
				inline  void * operator new(SizeT Size) {
					return Allocate(Size);
				}
				///<summary>Оператор виділення масиву об'єктів</summary>
				///<param name='Size'>Розмір масиву об'єктів</param>
				///<returns>Вертає ділянку пам'яті</returns>
				inline _Ret_notnull_ _Post_writable_byte_size_(Size) void * operator new[](SizeT Size) {
					return Allocate(Size);
				}
				///<summary>Оператор виділення одиночного об'єкта з специфічним вирівнюванням</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static inline _Ret_notnull_ _Post_writable_byte_size_(Size) void* operator new(Seed::SizeT Size, Seed::AllignRequirements al) {
					return AllignedAllocate(Size, (Seed::SizeT)al);
				}
				///<summary>Оператор виділення масиву об'єктів з специфічним вирівнюванням</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static inline _Ret_notnull_ _Post_writable_byte_size_(Size) void* operator new[](Seed::SizeT Size, Seed::AllignRequirements Allign) {
					return AllignedAllocate(Size, (Seed::SizeT)Allign);
				}
				///<summary>Оператор видалення одиночного об'єкту</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				static inline void operator delete(void * MemBlk) {
					Free(MemBlk);
				}
				///<summary>Оператор видалення масиву об'єктів</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				static inline void operator delete[](void * MemBlk) {
					Free(MemBlk);
				}
				///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				static inline void __cdecl operator delete(void * MemBlk, Seed::AllignRequirements Allign) {
					AllignedFree(MemBlk, (SizeT)Allign);
				}
				///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				static inline void __cdecl operator delete[](void * MemBlk, Seed::AllignRequirements Allign) {
					AllignedFree(MemBlk, (SizeT)Allign);
				}
#pragma warning(pop)

				///<summary>Кількість виділених блоків даних</summary>
				///<returns>Кількість виділених блоків даних</returns>
				SeedKernelCall SizeT CurrentMemBlkCount() noexcept;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				///<returns>Розмір виділеної пам'яті в байтах</returns>
				SeedKernelCall SizeT CurrentAllocatedMemory() noexcept;
			};

			///<summary>Ділення на ноль</summary>
			class DivideByZero : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				DivideByZero(const AChar * File, const AChar * FunctName, SizeT Line)
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
			class NullPointExeption : public Exception {
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
				NullPointExeption(const AChar * File, const AChar * FunctName, SizeT Line, SizeT Addr, Types Type)
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
				InvalidHandleUsage(const AChar * File, const AChar * FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Використання не валідного дескриптора")
				{}
			};

			///<summary>Неможливо перетворити тип</summary>
			class BadCast : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadCast(const AChar * File, const AChar * FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо пертворити тип")
				{}
			};

			///<summary>Неможливо викликати віртуальну функцію</summary>
			class BadPureCall : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadPureCall(const AChar * File, const AChar * FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати віртуальну функцію")
				{}
			};

			///<summary>Некоректний тип помилки</summary>
			class BadException : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadException(const AChar * File, const AChar * FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Не коректний тип помилки")
				{}
			};

			///<summary>Недостатньо оперативної пам'яті для операції</summary>
			class OutOfMemory :public Exception {
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
					const AChar * File,
					const AChar * FunctName,
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
			class InternalError : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InternalError(const AChar * File, const AChar * FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Внутрішня помилка ядра")
				{}
			};

			///<summary>Доступ до елемента який знаходится за межею розмірів</summary>
			class IndexOutOfRange :public Exception {
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
				__forceinline IndexOutOfRange(const AChar * File, const AChar * FunctName, SizeT Line, SizeT RequiredIndex, SizeT MaxAllowedIndex)
					noexcept : Exception(File, FunctName, Line, L"Index out of range") {
					this->Index = RequiredIndex;
					this->MaxIndex = MaxAllowedIndex;
				}

				///<summary>Запрошуваний індекс</summary>
				__forceinline SizeT RequiredIndex() { return Index; }
				///<summary>Максимальний достуаний індекс</summary>
				__forceinline SizeT MaxAllowedIndex() { return MaxIndex; }
			};
		}
	}
}

template<typename E> __forceinline InternalApiCall __declspec(noreturn) void __throw(E && ex) { throw ex; }

#define Throw(ExceptionObj)\
{ \
	static_assert(Seed::Core::StaticAsserts::IsBasedOn<Seed::Core::Exceptions::Exception, decltype(ExceptionObj)>(), \
	"Only Seed::Exception and derived classes cant be throw as exception"); \
	__assume(Seed::Core::StaticAsserts::IsBasedOn<Seed::Core::Exceptions::Exception, decltype(ExceptionObj)>()); \
	__pragma(warning(push)) \
	__pragma(warning(disable: 4996)) \
	__throw(ExceptionObj);\
	__pragma(warning(pop)) \
 }


#define throw static_assert(false, "Use Throw insteed")