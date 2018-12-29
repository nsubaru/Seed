#include "Seed.Core.Memory.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.Exceptions.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

///<summary>Інформація про кучі</summary>
struct
{
	///<summary>Дескриптор кучі</summary>
	Seed::Handle HeapHandle;
	///<summary>Поточне використання кучі</summary>
	Seed::SizeT AllocatedMemory;
	///<summary>Поточна кількість виділених блоків</summary>
	Seed::SizeT AllocatedMemBlksCount;
	///<summary>Фактичне використання пам'яті кучею</summary>
	Seed::SizeT CurrentHeapSize;
} HeapData, SecureHeapData;

Seed::Boolean Seed::Core::Memory::Heap::Init() noexcept
{
	HeapData.HeapHandle = RtlCreateHeap(
		HEAP_GROWABLE,
		NULL,
		10 * 1024 * 1024,
		0,
		NULL,
		NULL);
	if (HeapData.HeapHandle == nullptr)
		return False;
	HeapData.AllocatedMemBlksCount = HeapData.AllocatedMemory = HeapData.CurrentHeapSize = 0;
	return True;
}

void Seed::Core::Memory::Heap::Stop() noexcept
{
	//if (AllocatedMemBlksCount > 0)
	//	toKernelLog(L"Попередження: виявлено утічку пам'яті в стандартній кучі");
	RtlDestroyHeap(HeapData.HeapHandle);
}

SeedKernelCall void * Seed::Core::Memory::Heap::Allocate(Seed::SizeT Size)
{
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max::SizeT)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі
	SizeT * obj = (SizeT*)RtlAllocateHeap(HeapData.HeapHandle, 0, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&HeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&HeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

SeedKernelCall void * Seed::Core::Memory::Heap::AllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq)
{
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне, 
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	void * obj = RtlAllocateHeap(HeapData.HeapHandle, 0, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
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
	InterlockedIncrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&HeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&HeapData.CurrentHeapSize, Size + Size + sizeof(SizeT) + sizeof(SizeT));
	//Вертаєм покажчик на початок даних
	return (void*)AllignedAddress;
}

SeedKernelCall void * Seed::Core::Memory::Heap::Free(void * MemBlk)
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
	if (RtlFreeHeap(HeapData.HeapHandle, 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&HeapData.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&HeapData.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else Throw(NullPointExeption(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointExeption::Types::Read));
}

SeedKernelCall void * Seed::Core::Memory::Heap::AllignedFree(void * MemBlk, Seed::SizeT AllocationReq)
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
	if (RtlFreeHeap(HeapData.HeapHandle, 0, RealPtr)) {
		//Якшо успішно то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&HeapData.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&HeapData.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else Throw(NullPointExeption(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointExeption::Types::Read));
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentMemBlkCount() noexcept
{
	return HeapData.AllocatedMemBlksCount;
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentAllocatedMemory() noexcept
{
	return HeapData.AllocatedMemory;
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentHeapUsage() noexcept
{
	return HeapData.CurrentHeapSize;
}

Seed::Boolean Seed::Core::Memory::SecuredHeap::Init() noexcept
{
	SecureHeapData.HeapHandle = RtlCreateHeap(
		HEAP_GROWABLE | HEAP_ZERO_MEMORY,
		NULL,
		2 * 1024 * 1024,
		0,
		NULL,
		NULL);
	if (SecureHeapData.HeapHandle == nullptr)
		return False;
	SecureHeapData.AllocatedMemBlksCount = SecureHeapData.AllocatedMemory = SecureHeapData.CurrentHeapSize = 0;
	return True;
}

void Seed::Core::Memory::SecuredHeap::Stop() noexcept
{
	//if (AllocatedMemBlksCount > 0)
	//	toKernelLog(L"Попередження: виявлено утічку пам'яті в захищеній кучі");
	RtlDestroyHeap(SecureHeapData.HeapHandle);
}

DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Seed::Core::Memory::SecuredHeap::Allocate(Seed::SizeT Size)
{
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max::SizeT)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі і вказуєм системі обнулити ділянку пам'яті
	SizeT * obj = (SizeT*)RtlAllocateHeap(SecureHeapData.HeapHandle, HEAP_ZERO_MEMORY, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&SecureHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&SecureHeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Seed::Core::Memory::SecuredHeap::AllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq)
{
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне, 
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	//Вказуєм системі обнулити ділянку пам'яті пееред виділенням
	void * obj = RtlAllocateHeap(SecureHeapData.HeapHandle, HEAP_ZERO_MEMORY, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Вираховуєм мінімальний можливий адресс початку даних в ділянці пам'яті
	SizeT Address = ((SizeT)obj) + sizeof(SizeT) + sizeof(SizeT); //-V119
	//Вираховуєм точний адресс початку ділянки
	SizeT AllignedAddress = Math::RoundToUpper(Address, AllocationReq) * AllocationReq;
	//Отримуєм заголовок ділянки пам'яті
	SizeT * RealPtr = (SizeT*)(AllignedAddress)-2;
	//Записуєм реальний початок ділянки пам'яті
	RealPtr[0] = (SizeT)obj;
	//Та розмір даних
	RealPtr[1] = Size;
	//Атомарно оновлюєм статитистику
	InterlockedIncrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&SecureHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&SecureHeapData.CurrentHeapSize, Size + Size + sizeof(SizeT) + sizeof(SizeT));
	//Вертаєм покажчик на початок даних
	return (void*)AllignedAddress;
}

DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Seed::Core::Memory::SecuredHeap::Free(void * MemBlk)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + DataSize;
	//Обнулюєм ділянку пам'яті
	RtlSecureZeroMemory(MemBlk, DataSize);
	//Пробуєм звільнити пам'ять
	if (RtlFreeHeap(SecureHeapData.HeapHandle, 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&SecureHeapData.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&SecureHeapData.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else Throw(NullPointExeption(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointExeption::Types::Read));
}

DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void * Seed::Core::Memory::SecuredHeap::AllignedFree(void * MemBlk, Seed::SizeT AllocationReq)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT * Header = ((SizeT*)MemBlk) - 2;
	//Отрмуєм покажчик на початок ділянки пам'яті
	void* RealPtr = (void*)Header[0];
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[1];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + sizeof(SizeT) + AllocationReq + DataSize;
	//Обнулюєм ділянку пам'яті
	RtlSecureZeroMemory(MemBlk, DataSize);
	//Пробуєм звільнити ділянку пам'яті
	if (RtlFreeHeap(SecureHeapData.HeapHandle, 0, RealPtr)) {
		//Якшо успішно то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)&SecureHeapData.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)&SecureHeapData.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointExeption
	else Throw(NullPointExeption(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointExeption::Types::Read));

}

Seed::SizeT Seed::Core::Memory::SecuredHeap::CurrentMemBlkCount() noexcept
{
	return SecureHeapData.AllocatedMemBlksCount;
}

Seed::SizeT Seed::Core::Memory::SecuredHeap::CurrentAllocatedMemory() noexcept
{
	return SecureHeapData.AllocatedMemory;
}

Seed::SizeT Seed::Core::Memory::SecuredHeap::CurrentHeapUsage() noexcept
{
	return SecureHeapData.CurrentHeapSize;
}

#pragma intrinsic(memcpy)
DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void Seed::Core::Memory::Copy(
	_In_reads_(Size) const void * Source,
	_Out_writes_all_(Size) void * Destination,
	Seed::SizeT Size)
{
	memcpy(Destination, Source, Size);
}

#pragma intrinsic(memset)
DeprecatedCall("Низькорівнева робота з пам'ятю") SeedKernelCall void Seed::Core::Memory::MemBlkFill(
	_Out_writes_bytes_all_(Size) void * MemBlk,
	Seed::SizeT Size,
	_In_ const Seed::Byte Filler)
{
	memset(MemBlk, Filler, Size);
}

Seed::Boolean Seed::Core::Init::MemoryManagment() noexcept {
	return Seed::Core::Memory::Init();
}

Seed::Boolean Seed::Core::Memory::Init()
{
	
	//toKernelLog(L"Підсистема оперативної пам'яті ініціалізована");
	return true;
}

void Seed::Core::Memory::Stop()
{
	Memory::SecuredHeap::Stop();
	Memory::Heap::Stop();
	///toKernelLog(L"Підсистема оперативної пам'яті зупинена успішно");
}

void Seed::Core::Stop::MemoryManagment() noexcept
{
	Memory::Stop();
}

#pragma warning(pop)