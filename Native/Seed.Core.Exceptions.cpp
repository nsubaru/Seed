#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

using namespace Seed;

struct
{
	Handle HeapHandle;
	SizeT AllocatedMemory;
	SizeT AllocatedMemBlksCount;
	SizeT CurrentHeapSize;
} ExceptionsHeap;

Boolean Seed::Core::Init::Exceptions() noexcept
{
	ExceptionsHeap.HeapHandle =
		RtlCreateHeap(
			HEAP_GROWABLE,
			NULL,
			10485760ull,
			0,
			NULL,
			NULL
		);
	ExceptionsHeap.AllocatedMemBlksCount = ExceptionsHeap.AllocatedMemory = 0;

	return Boolean();
}

void Seed::Core::Stop::Exceptions() noexcept
{
	RtlDestroyHeap(ExceptionsHeap.HeapHandle);
}

SeedKernelCall void * Seed::Core::Exceptions::Exception::Allocate(SizeT Size) noexcept
{
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max::SizeT)
		return nullptr;
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі
	SizeT * obj = (SizeT*)RtlAllocateHeap(ExceptionsHeap.HeapHandle, 0, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		return nullptr;
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&ExceptionsHeap.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&ExceptionsHeap.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&ExceptionsHeap.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

SeedKernelCall void * Seed::Core::Exceptions::Exception::AllignedAllocate(SizeT Size, SizeT AllocationReq) noexcept
{
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне, 
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	void * obj = RtlAllocateHeap(ExceptionsHeap.HeapHandle, 0, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		return nullptr;
	//Вираховуєм мінімальний можливий адресс початку даних в ділянці пам'яті
	SizeT Address = ((SizeT)obj) + sizeof(SizeT) + sizeof(SizeT); //-V119
	//Вираховуєм точний адресс початку ділянки
	SizeT AllignedAddress = Math::RoundToUpper(Address, AllocationReq) * AllocationReq;
	//Отримуєм заголовок ділянки пам'яті
	SizeT * RealPtr = (SizeT*)(AllignedAddress - 2);
	//Записуєм реальний початок ділянки пам'яті
	RealPtr[0] = (SizeT)obj;
	//Та розмір даних
	RealPtr[1] = Size;
	//Атомарно оновлюєм статитистику
	InterlockedIncrementSizeTNoFence(&ExceptionsHeap.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&ExceptionsHeap.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&ExceptionsHeap.CurrentHeapSize, Size + Size + sizeof(SizeT) + sizeof(SizeT));
	//Вертаєм покажчик на початок даних
	return (void*)AllignedAddress;
}

SeedKernelCall void * Seed::Core::Exceptions::Exception::Free(void * MemBlk)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + DataSize;
	//Пробуєм звільнити пам'ять
	if (RtlFreeHeap(ExceptionsHeap.HeapHandle, 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&ExceptionsHeap.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&ExceptionsHeap.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&ExceptionsHeap.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else return MemBlk;
}

SeedKernelCall void * Seed::Core::Exceptions::Exception::AllignedFree(void * MemBlk, Seed::SizeT AllocationReq)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT * Header = ((SizeT*)MemBlk) - 2;
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[0];
	//Отрмуєм покажчик на початок ділянки пам'яті
	void* RealPtr = (void*)Header[1];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + sizeof(SizeT) + AllocationReq + DataSize;
	//Пробуєм звільнити ділянку пам'яті
	if (RtlFreeHeap(ExceptionsHeap.HeapHandle, 0, RealPtr)) {
		//Якшо успішно то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&ExceptionsHeap.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&ExceptionsHeap.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&ExceptionsHeap.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else return MemBlk;
}

SeedKernelCall SizeT Seed::Core::Exceptions::Exception::CurrentMemBlkCount() noexcept
{
	return ExceptionsHeap.AllocatedMemBlksCount;
}

SeedKernelCall SizeT Seed::Core::Exceptions::Exception::CurrentAllocatedMemory() noexcept
{
	return ExceptionsHeap.AllocatedMemory;
}

#pragma warning(pop)
