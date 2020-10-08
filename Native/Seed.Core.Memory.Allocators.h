#pragma once
#include "Seed.Core.Memory.Base.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4595)
namespace Seed {
	inline namespace Core {
		///<summary>Простір імен підсистеми пам'яті рантайму Рем</summary>
		namespace Memory {
#pragma region Heaps classes
			///<summary>Базовий клас куч</summary>
			interface BasicHeap {
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Allocate(DataSizeT Size) = 0;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<returns>Виділену ділянку</returns>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedAllocate(DataSizeT Size, DataSizeT AllocationReq) = 0;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* Free(void* MemBlk) = 0;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				virtual DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedFree(void* MemBlk, DataSizeT AllocationReq) = 0;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SeedKernelCall SizeT CurrentMemBlkCount() = 0;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentAllocatedMemory() = 0;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentHeapUsage() = 0;
			};

			///<summary>Загальна куча</summary>
			class Heap final : public BasicHeap {
			public:
				/// <summary>
				/// Стандартний конструктор
				/// </summary>
				ForceInline Heap() = default;
				/// <summary>
				/// Стандартний оператор копіювання
				/// </summary>
				/// <param name="Лівий операнд"></param>
				/// <returns></returns>
				ForceInline Heap& operator=(Heap&&) = default;
				/// <summary>
				/// Стандартний деструктор
				/// </summary>
				ForceInline ~Heap() = default;
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
				Heap& operator=(const Heap&) = delete;
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<returns>Виділену ділянку</returns>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* Allocate(DataSizeT Size) override final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<returns>Виділену ділянку</returns>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedAllocate(DataSizeT Size, DataSizeT AllocationReq) override final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* Free(void* MemBlk) override final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedFree(void* MemBlk, DataSizeT AllocationReq) override final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SeedKernelCall SizeT CurrentMemBlkCount() override final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentAllocatedMemory() override final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentHeapUsage();
			};

			///<summary>Захищена куча</summary>
			///<remarks>Перед виділенням і після звільнення відбувається обнулення пам'яті</remarks>
			class SensitiveDataHeap final : public BasicHeap {
			public:
				/// <summary>
				/// Стандартний конструктор
				/// </summary>
				ForceInline SensitiveDataHeap() = default;
				/// <summary>
				/// Стандартний оператор копіювання
				/// </summary>
				/// <param name="Лівий операнд"></param>
				/// <returns></returns>
				ForceInline SensitiveDataHeap& operator=(SensitiveDataHeap&&) = default;
				/// <summary>
				/// Стандартний деструктор
				/// </summary>
				ForceInline ~SensitiveDataHeap() = default;
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
				SensitiveDataHeap& operator=(const SensitiveDataHeap&) = delete;
				///<summary>Виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<returns>Виділену ділянку</returns>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* Allocate(DataSizeT Size) override final;
				///<summary>Вирівнняне виділення пам'яті</summary>
				///<param name='Size'>Розмір ділянки в байтах</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				///<returns>Виділену ділянку або</returns>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedAllocate(DataSizeT Size, DataSizeT AllocationReq) override final;

				///<summary>Звільнення пам'яті</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* Free(void* MemBlk) override final;
				///<summary>Звільнення пам'яті яка буа виділена по границі</summary>
				///<param name='MemBlk'>Блок пам'яті який звільняється</param>
				///<param name='AllocationReq'>Границя вирівнювання</param>
				virtual SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void* AllignedFree(void* MemBlk, DataSizeT AllocationReq) override final;

				///<summary>Кількість виділених блоків даних</summary>
				virtual SeedKernelCall SizeT CurrentMemBlkCount() override final;
				///<summary>Розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentAllocatedMemory() override final;
				///<summary>Фактичнй розмір виділеної пам'яті в байтах</summary>
				virtual SeedKernelCall DataSizeT CurrentHeapUsage() override final;
			};
#pragma endregion

#pragma region "Allocator class"
			///<summary>Класс алокатор
			///<para>Відповідає за виділення та звільнення пам'яті</para>
			///</summary>
			template<class HeapClass>
			class Allocator {
				static_assert(TypeTraits::IsBasedOn<Memory::BasicHeap, HeapClass>(), L"HeapClass in not heap");
			private:
				///<value>Куча з якої відбуватиметься виділення</value>
				inline static HeapClass heap;
			public:
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				ForceInline Allocator() {};
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				ForceInline Allocator(Allocator&) {};
				///<summary>Дані алокатора статичні тому конструктори,
				///оператори копіювання і переміщення та деструктор не потрібен
				///</summary>
				ForceInline Allocator(Allocator&&) {};
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
				ForceInline ~Allocator() {};

				///<summary>Оператор виділення пам'яті</summary>
				///<param name = "Size">Розмір ділянки пам'яті</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static ForceInline void* operator new(SizeT Size) {
					return heap.Allocate(DataSizeT(Size));
				}

				///<summary>Оператор виділення масиву об'єктів</summary>
				///<param name = "Size">Розмір масиву об'єктів</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static ForceInline void* operator new[](SizeT Size) {
					return heap.Allocate(DataSizeT(Size));
				}

				///<summary>Оператор виділення одиночного об'єкта з специфічним вирівнюванням</summary>
				///<param name = "Size">Розмір ділянки пам'яті</param>
				///<param name = "Allign">Границя вирівнювання</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static ForceInline void* operator new(Seed::SizeT Size, Seed::AllignRequirements Allign) {
					return heap.AllignedAllocate(DataSizeT(Size), DataSizeT((Seed::SizeT)Allign));
				}

				///<summary>Оператор виділення масиву об'єктів з специфічним вирівнюванням</summary>
				///<param name = "Size">Розмір ділянки пам'яті</param>
				///<param name = "Allign">Границя вирівнювання</param>
				///<returns>Вертає ділянку пам'яті</returns>
				static ForceInline void* operator new[](Seed::SizeT Size, Seed::AllignRequirements Allign) {
					return heap.AllignedAllocate(DataSizeT(Size), DataSizeT((Seed::SizeT)Allign));
				}

				///<summary>Оператор видалення одиночного об'єкту</summary>
				///<param name = "MemBlk">Ділянка пам'яті</param>
				static ForceInline void operator delete(void* MemBlk) {
					heap.Free(MemBlk);
				}
				///<summary>Оператор видалення масиву об'єктів</summary>
				///<param name = "MemBlk">Ділянка пам'яті</param>
				static ForceInline void operator delete[](void* MemBlk) {
					heap.Free(MemBlk);
				}

				///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням</summary>
				///<param name = "MemBlk">Ділянка пам'яті</param>
				///<param name = "Allign">Границя вирівнювання</param>
				static ForceInline void __cdecl operator delete(void* MemBlk, Seed::AllignRequirements Allign) {
					heap.AllignedFree(MemBlk, DataSizeT((SizeT)Allign));
				}
				///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням</summary>
				///<param name = "MemBlk">Ділянка пам'яті</param>
				///<param name = "Allign">Границя вирівнювання</param>
				static ForceInline void __cdecl operator delete[](void* MemBlk, Seed::AllignRequirements Allign) {
					heap.AllignedFree(MemBlk, DataSizeT((SizeT)Allign));
				}
			};
#pragma endregion

#pragma region Memory statistic
			///<summary>Загальне кількість виділених блоків пам'яті у внутрішній кучі рантайму</summary>
			SizeT CurrentInternalHeapAllocatedMemBlks();

			///<summary>Загальний розмір виділеної пам'яті у внутрішній кучі рантайму</summary>
			DataSizeT CurrentInternalHeapAllocatedMemory();

			///<summary>Загальне використання пам'яті у внутрішній кучі рантайму</summary>
			DataSizeT CurrentInternalHeapHeapsUsage();

			///<summary>Загальне кількість виділених блоків пам'яті у всіх керованих кучах кучах</summary>
			SizeT CurrentTotalAllocatedMemBlks();

			///<summary>Загальний розмір виділеної пам'яті у всіх керованих кучах кучах</summary>
			DataSizeT CurrentTotalAllocatedMemory();

			///<summary>Загальне використання пам'яті у всіх керованих кучах кучах</summary>
			DataSizeT CurrentTotalHeapsUsage();
#pragma endregion

#pragma region Miscenelious memory function
			///<summary>Копіювання ділянки пам'яті</summary>
			///<param name='Source'>Ділянка пам'яті яка копіюється</param>
			///<param name='Destination'>Ділянка пам'яті куда копіюється</param>
			///<param name='Size'>Кількість байт як потрібно скопіювати</param>
			SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void Copy(
				const void* Source,
				void* Destination,
				SizeT Size);

			///<summary>Заповнює блок пам'яті певним значенням</summary>
			///<param name='MemBlk'>Блок пам'яті</param>
			///<param name='Size'>Розмір в байтах ділянки пам'яті</param>
			///<param name='Filler'>Значення яким заповнить ділянку пам'яті</param>
			SeedKernelCall DeprecatedCall("Низькорівнева робота з пам'ятю") void MemBlkFill(
				void* MemBlk,
				SizeT Size,
				const Byte Filler);
#pragma endregion
		}
	}
}

#pragma region Default new and delete operators
#pragma warning(push)
#pragma warning(disable: 5043)
///<summary>Оператор виділення пам'яті</summary>
///<param name = "Size">Розмір ділянки пам'яті</param>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void* __cdecl operator new(Seed::SizeT Size) {
	return Seed::Memory::Allocator<Seed::Memory::Heap>::operator new(Size);
}
///<summary>Оператор виділення масиву об'єктів</summary>
///<param name = "Size">Розмір масиву об'єктів</param>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void* __cdecl operator new[](Seed::SizeT Size) {
	return Seed::Memory::Allocator<Seed::Memory::Heap>::operator new[](Size);
}
///<summary>Оператор виділення одиночного об'єкта з специфічним вирівнюванням</summary>
///<param name = "Size">Розмір ділянки пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void* __cdecl operator new(Seed::SizeT Size, Seed::AllignRequirements Allign) {
	return Seed::Memory::Allocator<Seed::Memory::Heap>::operator new(Size, Allign);
}
///<summary>Оператор виділення масиву об'єктів з специфічним вирівнюванням</summary>
///<param name = "Size">Розмір ділянки пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
///<returns>Вертає ділянку пам'яті</returns>
[[nodiscard]]
inline void* __cdecl operator new[](Seed::SizeT Size, Seed::AllignRequirements Allign) {
	return Seed::Memory::Allocator<Seed::Memory::Heap>::operator new[](Size, Allign);
}
///<summary>Оператор видалення одиночного об'єкту</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
inline void __cdecl operator delete(void* MemBlk) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete(MemBlk);
}
///<summary>Оператор видалення масиву об'єктів</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
inline void __cdecl operator delete[](void* MemBlk) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete[](MemBlk);
}
///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
inline void __cdecl operator delete(void* MemBlk, Seed::AllignRequirements Allign) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete(MemBlk, Allign);
}
///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
inline void __cdecl operator delete[](void* MemBlk, Seed::AllignRequirements Allign) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete[](MemBlk, Allign);
}
///<summary>Оператор видалення одиночного об'єкту з передачею розміру об'єкта</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Size">Розмір ділянки пам'яті</param>
inline void __cdecl operator delete(void* MemBlk, [[maybe_unused]] Seed::SizeT Size) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete(MemBlk);
}
///<summary>Оператор видалення масиву об'єктів з передачею розміру об'єкта</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Size">Розмір ділянки пам'яті</param>
inline void __cdecl operator delete[](void* MemBlk, [[maybe_unused]] Seed::SizeT Size) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete[](MemBlk);
}
///<summary>Оператор видалення одиночного об'єкту з специфічним вирівннюванням та розміром</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
///<param name = "Size">Розмір ділянки пам'яті</param>
inline void __cdecl operator delete(void* MemBlk, [[maybe_unused]] Seed::SizeT Size, Seed::AllignRequirements Allign) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete(MemBlk, Allign);
}
///<summary>Оператор видалення масиву об'єктів з специфічним вирівннюванням та розміром</summary>
///<param name = "MemBlk">Ділянка пам'яті</param>
///<param name = "Allign">Границя вирівнювання</param>
///<param name = "Size">Розмір ділянки пам'яті</param>
inline void __cdecl operator delete[](void* MemBlk, [[maybe_unused]] Seed::SizeT Size, Seed::AllignRequirements Allign) {
	Seed::Memory::Allocator<Seed::Memory::Heap>::operator delete[](MemBlk, Allign);
}
#pragma warning(pop)
#pragma endregion
#pragma warning(pop)