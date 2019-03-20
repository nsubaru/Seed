#pragma once
#include "Seed.Core.Base.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4595)
namespace Seed {
	inline namespace Core {
		///<summary>Простір імен підсистеми пам'яті рантайму Рем</summary>
		namespace Memory {
			///<summary>Базовий клас куч</summary>
			interface BasicHeap {
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Allocate(SizeT Size) = 0;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedAllocate(SizeT Size, SizeT AllocationReq) = 0;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Free(void * MemBlk) = 0;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedFree(void * MemBlk, SizeT AllocationReq) = 0;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() noexcept = 0;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentAllocatedMemory() noexcept = 0;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentHeapUsage() noexcept = 0;
			};

			///<summary>Загальна куча</summary>
			class Heap : public BasicHeap {
			private:
				///<summary>Ініціалізація кучі</summary>
				///<returns>True якщо ініціалізація вдало проведена</returns>
				static Boolean Init() noexcept;
				///<summary>Зупинка кучі</summary>
				static void Stop() noexcept;
				///<summary>Ініціалізація кучі</summary>
				///<returns>True якщо ініціалізація проведена успішно</returns>
				friend extern Boolean Init();
				///<summary>Зупинка куч</summary>
				friend extern void Stop();
			public:
				Heap& operator=(Heap&) = delete;
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Allocate(SizeT Size) override final {
					extern void * GenericAllocate(SizeT Size);
					return GenericAllocate(Size);
				}
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedAllocate(SizeT Size, SizeT AllocationReq) override final {
					extern void * GenericAllignedAllocate(SizeT Size, SizeT AllocationReq);
					return GenericAllignedAllocate(Size, AllocationReq);
				}

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Free(void * MemBlk) override final {
					extern void * GenericFree(void * MemBlk);
					return GenericFree(MemBlk);
				}
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * AllignedFree(void * MemBlk, SizeT AllocationReq) override final {
					extern void * GenericAllignedFree(void * MemBlk, SizeT AllocationReq);
					return GenericAllignedFree(MemBlk, AllocationReq);
				}

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() noexcept override final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentAllocatedMemory() noexcept override final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentHeapUsage() noexcept;
			};

			///<summary>Захищена куча</summary>
			///<remarks>Перед виділенням і після звільнення відбувається обнулення пам'яті</remarks>
			class SeedKernelCall SensitiveDataHeap : public BasicHeap {
			private:
				///<summary>Ініціалізація кучі</summary>
				///<returns>True якщо ініціалізація вдало проведена</returns>
				static Boolean Init() noexcept;
				///<summary>Зупинка кучі</summary>
				static void Stop() noexcept;
				///<summary>Ініціалізація кучі</summary>
				///<returns>True якщо ініціалізація проведена успішно</returns>
				friend extern Boolean Init();
				///<summary>Зупинка куч</summary>
				friend extern void Stop();
			public:
				SensitiveDataHeap & operator=(SensitiveDataHeap&) = delete;
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void * Allocate(SizeT Size) override final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Виділену ділянку або</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void * AllignedAllocate(SizeT Size, SizeT AllocationReq) override final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void * Free(void * MemBlk) override final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void * AllignedFree(void * MemBlk, SizeT AllocationReq) override final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SizeT CurrentMemBlkCount() noexcept override final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentAllocatedMemory() noexcept override final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SizeT CurrentHeapUsage() noexcept override final;
			};

			///<summary>Копіювання ділянки пам'яті</summary>
			///<param name='Source'>Ділянка пам'яті яка копіюється</param>
			///<param name='Destination'>Ділянка пам'яті куда копіюється</param>
			///<param name='Size'>Кількість байт як потрібно скопіювати</param>
			DeprecatedCall("Низькорівнева робота з пам'ятю") void Copy(
				const void * Source,
				void * Destination,
				SizeT Size);

			///<summary>Заповнює блок пам'яті певним значенням</summary>
			///<param name='MemBlk'>Блок пам'яті</param>
			///<param name='Size'>Розмір в байтах ділянки пам'яті</param>
			///<param name='Filler'>Значення яким заповнить ділянку пам'яті</param>
			DeprecatedCall("Низькорівнева робота з пам'ятю") void MemBlkFill(
				void * MemBlk,
				SizeT Size,
				const Byte Filler);

			///<summary>Загальне кількість виділених блоків пам'яті у обох кучами</summary>
			__forceinline SizeT CurrentTotalAllocatedMemBlks() noexcept
			{
				return Heap().CurrentMemBlkCount() + SensitiveDataHeap().CurrentMemBlkCount();
			}

			///<summary>Загальний розмір виділеної пам'яті з обох куч</summary>
			__forceinline SizeT CurrentTotalAllocatedMemory() noexcept
			{
				return Heap().CurrentAllocatedMemory() + SensitiveDataHeap().CurrentAllocatedMemory();
			}

			///<summary>Загальне використання пам'яті кучами</summary>
			__forceinline SizeT CurrentTotalHeapsUsage() noexcept
			{
				return Heap().CurrentHeapUsage() + SensitiveDataHeap().CurrentHeapUsage();
			}

			///<summary>Класс алокатор
			///<para>Відповідає за виділення та звільнення пам'яті</para>
			///</summary>
			template<class HeapClass>
			class Allocator {
				static_assert(StaticAsserts::IsBasedOn<Memory::BasicHeap, HeapClass>(), L"Класс Heap не є кучею");
			private:
				///<value>Куча з якої відбуватиметься виділення</value>
				inline static HeapClass heap;
			public:
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				Allocator() {};
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				Allocator(Allocator&) {};
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				Allocator(Allocator&&) {};
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				Allocator& operator=(Allocator&) = delete;
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				Allocator& operator=(Allocator&&) = delete;
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				~Allocator() {};

				///<summary>Оператор виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Вертає ділянку пам'яті</returns>
				static __forceinline void * operator new(SizeT Size) {
					return heap.Allocate(Size);
				}
				///<summary>Оператор виділення масиву об'єктів</summary>
				///<param name='Size'>Розмір масиву об'єктів</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Вертає ділянку пам'яті</returns>
				static __forceinline void * operator new[](SizeT Size) {
					return heap.Allocate(Size);
				}
				///<summary>Оператор виділення одиночного об'єкта з специфічним вирівнюванням</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Вертає ділянку пам'яті</returns>
				static __forceinline void* operator new(Seed::SizeT Size, Seed::AllignRequirements Allign) {
				return heap.AllignedAllocate(Size, (Seed::SizeT)Allign);
				}
				///<summary>Оператор виділення масиву об'єктів з специфічним вирівнюванням</summary>
				///<param name='Size'>Розмір ділянки пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
				///<returns>Вертає ділянку пам'яті</returns>
				static __forceinline void* operator new[](Seed::SizeT Size, Seed::AllignRequirements Allign) {
					return heap.AllignedAllocate(Size, (Seed::SizeT)Allign);
				}
				///<summary>Оператор видалення одиночного об'єкту</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				static __forceinline void operator delete(void * MemBlk) {
					heap.Free(MemBlk);
				}
				///<summary>Оператор видалення масиву об'єктів</summary>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				static __forceinline void operator delete[](void * MemBlk) {
					heap.Free(MemBlk);
				}
				///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				static __forceinline void __cdecl operator delete(void * MemBlk, Seed::AllignRequirements Allign) {
					heap.AllignedFree(MemBlk, (SizeT)Allign);
				}
				///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням</summary>
				///<param name='MemBlk'>Ділянка пам'яті</param>
				///<param name='Allign'>Границя вирівнювання</param>
				///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
				static __forceinline void __cdecl operator delete[](void * MemBlk, Seed::AllignRequirements Allign) {
					heap.AllignedFree(MemBlk, (SizeT)Allign);
				}
			};
		}
	}
}

///<summary>Оператор виділення пам'яті</summary>
///<param name='Size'>Розмір ділянки пам'яті</param>
///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void * __cdecl operator new(Seed::SizeT Size) {
	extern void * GenericAllocate(Seed::SizeT Size);
	return GenericAllocate(Size);
}
///<summary>Оператор виділення масиву об'єктів</summary>
///<param name='Size'>Розмір масиву об'єктів</param>
///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void * __cdecl operator new[](Seed::SizeT Size) {
	extern void * GenericAllocate(Seed::SizeT Size);
	return GenericAllocate(Size);
}
///<summary>Оператор виділення одиночного об'єкта з специфічним вирівнюванням</summary>
///<param name='Size'>Розмір ділянки пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void * __cdecl operator new(Seed::SizeT Size, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq);
	return GenericAllignedAllocate(Size, (Seed::SizeT)Allign);
}
///<summary>Оператор виділення масиву об'єктів з специфічним вирівнюванням</summary>
///<param name='Size'>Розмір ділянки пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<exeption cref='Seed::OutOfMemory'>Якщо неможливо виділити пам'ять</exeption>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void * __cdecl operator new[](Seed::SizeT Size, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq);
	return GenericAllignedAllocate(Size, (Seed::SizeT)Allign);
}
///<summary>Оператор видалення одиночного об'єкту</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
__forceinline void __cdecl operator delete(void * MemBlk) {
	extern void * GenericFree(void * MemBlk);
	GenericFree(MemBlk);
}
///<summary>Оператор видалення масиву об'єктів</summary>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
///<param name='MemBlk'>Ділянка пам'яті</param>
__forceinline void __cdecl operator delete[](void * MemBlk) {
	extern void * GenericFree(void * MemBlk);
	GenericFree(MemBlk);
}
///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
__forceinline void __cdecl operator delete(void * MemBlk, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedFree(void * MemBlk, Seed::SizeT AllocationReq);
	GenericAllignedFree(MemBlk, (Seed::SizeT)Allign);
}
///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
__forceinline void __cdecl operator delete[](void * MemBlk, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedFree(void * MemBlk, Seed::SizeT AllocationReq);
	GenericAllignedFree(MemBlk, (Seed::SizeT)Allign);
}
///<summary>Оператор видалення одиночного об'єкту з передачею розміру об'єкта</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Size'>Розмір ділянки пам'яті</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
__forceinline void __cdecl operator delete(void * MemBlk, Seed::SizeT Size) {
	extern void * GenericFree(void * MemBlk);
	GenericFree(MemBlk);
}
///<summary>Оператор видалення масиву об'єктів з передачею розміру об'єкта</summary>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Size'>Розмір ділянки пам'яті</param>
__forceinline void __cdecl operator delete[](void * MemBlk, Seed::SizeT Size) {
	extern void * GenericFree(void * MemBlk);
	GenericFree(MemBlk);
}
///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням та розміром</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<param name='Size'>Розмір ділянки пам'яті</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
__forceinline void __cdecl operator delete(void * MemBlk, Seed::SizeT Size, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedFree(void * MemBlk, Seed::SizeT AllocationReq);
	GenericAllignedFree(MemBlk, (Seed::SizeT)Allign);
}
///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням та розміром</summary>
///<param name='MemBlk'>Ділянка пам'яті</param>
///<param name='Allign'>Границя вирівнювання</param>
///<exeption cref='Seed::NullPointException'>Якщо передана не існуюча ділянка пам'яті</exeption>
///<param name='Size'>Розмір ділянки пам'яті</param>
__forceinline void __cdecl operator delete[](void * MemBlk, Seed::SizeT Size, Seed::AllignRequirements Allign) {
	extern void * GenericAllignedFree(void * MemBlk, Seed::SizeT AllocationReq);
	GenericAllignedFree(MemBlk, (Seed::SizeT)Allign);
}
#pragma warning(pop)