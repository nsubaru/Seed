#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Memory.h"
#include "typeinfo"

#pragma warning(push)
#pragma warning(disable: 4996)
namespace Seed {
	inline namespace Core {
		namespace Memory {
			class SeedKernelCall ExceptionHeap : public BasicHeap {
			public:
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<exeption cref='Seed::Core::Exceptions::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Allocate(SizeT Size) final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::Core::Exceptions::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedAllocate(SizeT Size, SizeT AllocationReq) final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<exeption cref='Seed::Core::Exceptions::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Free(void* MemBlk) final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::Core::Exceptions::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedFree(void* MemBlk, SizeT AllocationReq) final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() noexcept final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentAllocatedMemory() noexcept final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentHeapUsage() noexcept final;

				ExceptionHeap& operator=(const ExceptionHeap&) = delete;
			};
		}

		///<summary>Константний ASCII рядок який пам'ятає свій розмір</summary>
		class SeedKernelCall ConstAString {
		private:
			///<summary>Покажчик на текст</summary>
			const AChar* str;
			///<summary>Його розмір</summary>
			SizeT size;
		public:
			///<summary>Конструктор константного ASCII рядка</summary>
			///<param name = "Text">Рядковий літерал ASCII символів</param>
			///<param name = "Size">Його розмір</param>
			template<SizeT Size>
			constexpr ConstAString(const char(&Text)[Size]) : str(Text), size(Size) {}

			///<summary>Копіювальний конструктор</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			constexpr ConstAString(const ConstAString& orig) : str(orig.str), size(orig.size) {}

			///<summary>Оператор копіювання</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			///<returns>Силку на поточний об'єкт</returns>
			constexpr ConstAString& operator=(const ConstAString& orig) = default;

			///<summary>Оператор доступу до символів константного ASCII рядка</summary>
			///<param name = "Index>Індекс символа в рядку</param>
			///<returns>Константна силка на літеру</returns>
			const AChar& operator[](SizeT Index) const {
				extern __declspec(noreturn) void ThrowIndexOutOfRange(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Required,
					const Seed::SizeT MaxAllowed);
				if (Index >= size)
					ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, size);
				return str[Index];
			}

			///<summary>Вертає розмір константного ASCII рядка</summary>
			constexpr SizeT GetSize() const { return this->size; }

			///<summary>Розмір константного ASCII рядка</summary>
			SizeT Property(GetSize) Size;

			///<summary>Вертає константний масив символів цього константного ASCII рядка</summary>
			constexpr ConstACharStr GetString() const { return this->str; }

			///<summary>Константний масив символів цього константного ASCII рядка</summary>
			ConstACharStr Property(GetString) Str;
		};

		///<summary>Константний рядок який пам'ятає свій розмір</summary>
		class SeedKernelCall ConstString {
		private:
			///<summary>Покажчик на текст</summary>
			const Char* str;
			///<summary>Його розмір</summary>
			SizeT size;
		public:
			///<summary>Конструктор константного рядка</summary>
			///<param name = "Text">Рядковий літерал</param>
			///<param name = "Size">Його розмір</param>
			template<SizeT Size>
			constexpr ConstString(const wchar_t(&Text)[Size]) : str(Text), size(Size) {}

			///<summary>Копіювальний конструктор</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			constexpr ConstString(const ConstString& orig) : str(orig.str), size(orig.size) {}

			///<summary>Оператор копіювання</summary>
			///<param name = "orig">Оригинал константного рядка</param>
			///<returns>Силку на поточний об'єкт</returns>
			ConstString& operator=(const ConstString& orig) = default;

			///<summary>Оператор доступу до символів константного рядка</summary>
			///<param name = "Index>Індекс символа в рядку</param>
			///<returns>Константна силка на літеру</returns>
			const Char& operator[](SizeT Index) const {
				extern __declspec(noreturn) void ThrowIndexOutOfRange(
					const Seed::ConstAString & File,
					const Seed::ConstAString & Function,
					const Seed::SizeT Required,
					const Seed::SizeT MaxAllowed);
				if (Index >= size)
					ThrowIndexOutOfRange(__FILE__, __FUNCSIG__, __LINE__, size);
				return str[Index];
			}

			///<summary>Вертає розмір константного рядка</summary>
			constexpr SizeT GetSize() const { return this->size; }

			///<summary>Розмір константного рядка</summary>
			SizeT Property(GetSize) Size;

			///<summary>Вертає константний масив символів цього константного рядка</summary>
			constexpr ConstCharStr GetString() const { return this->str; }

			///<summary>Константний масив символів цього константного рядка</summary>
			ConstCharStr Property(GetString) Str;
		};

		///<summary>Підсистема помилок в рантаймі</summary>
		inline namespace Exceptions {
			///<summary>Класс базової помилки</summary>
			class SeedKernelCall Exception : virtual public Memory::Allocator<Memory::ExceptionHeap> {
			public:
				///<summary>Місце генерації помилки</summary>
				struct SeedKernelCall ExeptionLocation {
					///<summary>Ім'я файла</summary>
					ConstAString File;
					///<summary>Ім'я функції</summary>
					ConstAString FunctName;
					///<summary>Номер рядка</summary>
					SizeT Line;
					///<summary>ID потоку</summary>
					ID ThreadID;
					/////<summary>Текстове представлення</summary>
					//ExceptionString toString() const {
					//	return ExceptionString(L"Згенеровано у рядку ") + BaseString<Memory::ExceptionHeap>::toString(Line)
					//		+ ExceptionString(L" у функції ") + ExceptionString(File)
					//		+ ExceptionString(L" у файлі ") + ExceptionString(FunctName)
					//		+ ExceptionString(L" в потоці з ID ") + BaseString<Memory::ExceptionHeap>::toString(ThreadID);
					//}
				};
			protected:
				///<summary>Місце генерації даної помилки</summary>
				ExeptionLocation location;
				///<summary>Ім'я помилки</summary>
				ConstString exeptionName;
			public:
				//TODO: Modify ctor by receiving size of exception name string by template
				//TODO: Set thread id
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'Name'>Ім'я помилки</param>
				///<returns>Екземпляр класу Exeption</returns>
				__forceinline Exception(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const ConstString& ExeptionName) noexcept
					:location({ File, FunctName, Line, 0 }), exeptionName(ExeptionName) {}

				///<summary>Вертає користувальницьку назву помилки</summary>
				__forceinline const ConstString & GetExceptioName() const
				{
					return exeptionName;
				}

				///<summary>Вертає місце генерації помилки</summary>
				__forceinline const ExeptionLocation& GetLocation() const
				{
					return location;
				}

				///<summary>Користувальницька назва помилки</summary>
				ConstString Property(GetExceptioName) Name;
				///<summary>Місце генерації помилки</summary>
				ExeptionLocation Property(GetLocation) Location;

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const = 0 {
				//	return ExceptionString(L"Невідома помилка з повідомлення ") + ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString();
				//}

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
				DivideByZero(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Ділення на ноль")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Помилка доступу до комірки пам'яті
			///<para>Буває трьох варінтів</para>
			///<para> - Read	- спроба прочитати за неіснуючим адресом</para>
			///<para> - Write	- спроба записати за неіснуючим адресом</para>
			///<para> - Execute - спроба виконати команду за неіснуючим адресом</para>
			///<para>Тому помилка містить інформацію про тип доступу та адресс який пробували використати</para>
			///</summary>
			class SeedKernelCall NullPointException : public Exception {
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
				NullPointException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT Addr, Types Type)
					:Exception(File, FunctName, Line, L"Помилка доступу до пам'яті"), Addr(Addr), Type(Type)
				{}

				///<summary>Адрес комірки пам'яті яка згенерувала помилку</summary>
				SizeT GetMemoryAddress() const { return Addr; }
				///<summary>Тип доступу до комірки який згенерував помилку</summary>
				Types GetAccessType() const { return Type; }

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L" за адресом ") + BaseString<Memory::ExceptionHeap>::toString(GetMemoryAddress())
				//		+ ExceptionString(L" при спробі ") +
				//		((GetAccessType() == Types::Read) ? ExceptionString(L"читання. ") :
				//		(GetAccessType() == Types::Write) ? ExceptionString(L"запису. ") : ExceptionString(L"виконання. "))
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Використання невалідного дескриптора</summary>
			class SeedKernelCall InvalidHandleUsage : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidHandleUsage(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Використання не валідного дескриптора")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Неможливо перетворити тип</summary>
			class SeedKernelCall BadCast : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadCast(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо пертворити тип")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Неможливо викликати віртуальну функцію</summary>
			class SeedKernelCall BadPureCall : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadPureCall(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Неможливо викликати віртуальну функцію")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Некоректний тип помилки</summary>
			class SeedKernelCall BadException : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				BadException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Не коректний тип помилки")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Недостатньо оперативної пам'яті для операції</summary>
			class SeedKernelCall OutOfMemory :public Exception {
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
				

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L" в ") + ExceptionString(HeapName) + ExceptionString(L" кучі ")
				//		+ ExceptionString(L" при виділені ") + BaseString<Memory::ExceptionHeap>::toString(TryAllocate)
				//		+ ExceptionString(L" байт, на момент виникнення помилки було виділено ") 
				//		+ BaseString<Memory::ExceptionHeap>::toString(AllocatedBefore) + ExceptionString(L" байт в ")
				//		+ BaseString<Memory::ExceptionHeap>::toString(MemBlkscountBefore)
				//		+ ExceptionString((MemBlkscountBefore == 1) ? ExceptionString(L" блоку пам'яті. ") : (L"блоках пам'яті. "))
				//		+ location.toString();
				//}
			};

			///<summary>Внутрішня помилка ядра</summary>
			class SeedKernelCall InternalError : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InternalError(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Внутрішня помилка ядра")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Невірний формат</summary>
			class SeedKernelCall InvalidFormatException : public Exception {
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				InvalidFormatException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					:Exception(File, FunctName, Line, L"Невірний формат")
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
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
				__forceinline IndexOutOfRange(const ConstAString& File, const ConstAString& FunctName, SizeT Line, SizeT RequiredIndex, SizeT MaxAllowedIndex)
					noexcept : Exception(File, FunctName, Line, L"Вихід за межі"),
					Index(RequiredIndex), MaxIndex(MaxAllowedIndex)
				{}

				///<summary>Запрошуваний індекс</summary>
				__forceinline SizeT GetRequiredIndex() const { return Index; }
				///<summary>Максимальний достуаний індекс</summary>
				__forceinline SizeT GetMaxAllowedIndex() const { return MaxIndex; }

				///<summary>Запрошуваний індекс</summary>
				SizeT Property(GetRequiredIndex) RequiredIndex;
				///<summary>Максимальний достуаний індекс</summary>
				SizeT Property(GetMaxAllowedIndex) MaxAllowedIndex;

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". При спробі отримати доступ до елемента за індексом ") + BaseString<Memory::ExceptionHeap>::toString(RequiredIndex)
				//		+ ExceptionString(L" якщо максимальний дозволений індекс ") + BaseString<Memory::ExceptionHeap>::toString(MaxAllowedIndex)
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Некоректний тип</summary>
			class SeedKernelCall InvalidTypeException : public Exception {
			private:
				const TypeInfo& requiredType;
				const TypeInfo& actuallyType;
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				///<param name = 'RequireType'>Інформація про очікуваний тип</param>
				///<param name = 'ActuallyType'>Інформація про фактичний тип</param>
				__forceinline InvalidTypeException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, const TypeInfo& RequiredType, const TypeInfo& ActuallyType)
					noexcept : Exception(File, FunctName, Line, L"Не вірний тип"),
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

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". Очікуваний тип: ") + ExceptionString(RequiredType.name)
				//		+ ExceptionString(L". Фактичний тип: ") + ExceptionString(ActuallyType.name)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};

			///<summary>Операцію перервано</summary>
			class SeedKernelCall InteruptException : protected Exception
			{
			public:
				///<summary>Конструктор помилки</summary>
				///<param name = 'File'>Файл в якому згенерована помилка</param>
				///<param name = 'FunctName'>Ім'я функції</param>
				///<param name = 'Line'>Номер рядка де була згенерована помилка</param>
				__forceinline InteruptException(const ConstAString& File, const ConstAString& FunctName, SizeT Line)
					noexcept : Exception(File, FunctName, Line, L"Операція перервана")
				{};

				///<summary>Вертає місце генерації помилки</summary>
				__forceinline const ExeptionLocation& GetLocation() const
				{
					return location;
				}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override final {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}

				virtual ~InteruptException() {}
			};

			class NtException : public Exception {
			private:
				///<summary>NtStatus code операції</summary>
				Int32 statusCode;
			public:
				NtException(const ConstAString& File, const ConstAString& FunctName, SizeT Line, Int32 StatusCode)
					noexcept : Exception(File, FunctName, Line, L"Помилка системного виклику NT"), statusCode(StatusCode)
				{}

				/////<summary>Текстове представлення інформації про помилки помилки</summary>
				//virtual ExceptionString toString() const override {
				//	return ExceptionString(exeptionName)
				//		+ ExceptionString(L". Системний виклик вернув код ") 
				//		+ BaseString<Memory::ExceptionHeap>::UInt64asHex(statusCode) + ExceptionString(L". ")
				//		+ location.toString()
				//		+ ExceptionString(L". ");
				//}
			};
		}
	}
}

#pragma region Throw
///<summary>Вертає тип або тип результату функції\методу<\summary>
template<class Type>
using ThrowableObj = typename Seed::Core::StaticAsserts::ConditionT <
	Seed::Core::StaticAsserts::IsFunction<Type>::State,
	typename Seed::Core::StaticAsserts::ResultOf<Type>::Type,
	typename Type
>::Result;

///<summary>Вертає <c>True</c> якщо цей об'єкт ж помилкою чи вертає в якості результату об'єкт помилки</summary>
template<typename Type>
struct IsThrowable :
	Seed::Core::StaticAsserts::IsBasedOn<
	Seed::Core::Exceptions::Exception,
	Seed::Core::StaticAsserts::RemoveRefT<ThrowableObj<Type>>
	>
{};

///<summary>Внутрішня функція генерації помилки</summary>
///<param name = 'ex'>Об'єкт помилки</param>
template<typename E> InternalApiCall __forceinline void __declspec(noreturn) ___cxxthrow(E&& ex) { throw ex; }

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

///<summary>Заборона використання <c>throw</c> без провірок на тип об'єкта помилки</summary>
#define throw static_cast("Use Throw insted")

///<summary>Макрос локалізації помилки</summary>
#define ExceptionPosition __FILE__, __FUNCSIG__, __LINE__
#pragma endregion

#pragma warning(pop)