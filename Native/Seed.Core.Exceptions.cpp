#include "Seed.Core.Exceptions.h"
#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

using namespace Seed;

///<summary>Інформація про кучу помилок</summary>
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
} ExceptionsHeap;

Boolean Seed::Core::Init::Exceptions() noexcept
{
	ULONG Flags = HEAP_GROWABLE;
	auto configs = CurrentSettings::Memory::ExceptionHeap::GetConfigs();

	if (configs.HEAP_ARGS_CHECK)
		Flags |= FLG_HEAP_VALIDATE_PARAMETERS;
	if (configs.HEAP_FREE_CHECK)
		Flags |= FLG_HEAP_ENABLE_FREE_CHECK;
	if (configs.HEAP_TAIL_CHECK)
		Flags |= FLG_HEAP_ENABLE_FREE_CHECK;

	ExceptionsHeap.HeapHandle = RtlCreateHeap(
		Flags,
		NULL,
		CurrentSettings::Memory::ExceptionHeap::GetMaxSize(),
		CurrentSettings::Memory::ExceptionHeap::GetStartSize(),
		NULL,
		NULL);

	if (ExceptionsHeap.HeapHandle == nullptr)
		return false;

	ExceptionsHeap.AllocatedMemBlksCount = ExceptionsHeap.AllocatedMemory = 0;

	return true;
}

void Seed::Core::Stop::Exceptions() noexcept
{
	RtlDestroyHeap(ExceptionsHeap.HeapHandle);
}

SeedKernelCall void * Seed::Core::Memory::ExceptionHeap::Allocate(SizeT Size)
{
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
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

SeedKernelCall void * Seed::Core::Memory::ExceptionHeap::AllignedAllocate(SizeT Size, SizeT AllocationReq)
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

SeedKernelCall void * Seed::Core::Memory::ExceptionHeap::Free(void * MemBlk)
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
	//Якщо ні то вертаєм об'єкт
	else return MemBlk;
}

SeedKernelCall void * Seed::Core::Memory::ExceptionHeap::AllignedFree(void * MemBlk, SizeT AllocationReq)
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
	//Якщо ні то вертаєм об'єкт
	else return MemBlk;
}

SizeT Seed::Core::Memory::ExceptionHeap::CurrentMemBlkCount() noexcept
{
	return ExceptionsHeap.AllocatedMemBlksCount;
}

SizeT Seed::Core::Memory::ExceptionHeap::CurrentAllocatedMemory() noexcept
{
	return ExceptionsHeap.AllocatedMemory;
}

SizeT Seed::Core::Memory::ExceptionHeap::CurrentHeapUsage() noexcept
{
	return ExceptionsHeap.CurrentHeapSize;
}

__declspec(noreturn) void ThrowIndexOutOfRange(
	const Seed::ConstAString & File, 
	const Seed::ConstAString & Function, 
	const Seed::SizeT Line,
	const Seed::SizeT Required,
	const Seed::SizeT MaxAllowed) {
	Throw(IndexOutOfRange(File, Function, Line, Required, MaxAllowed));
}

__declspec(noreturn) void ThrowEndNodeNotDeferencable(
	const Seed::ConstAString& File,
	const Seed::ConstAString& Function,
	const Seed::SizeT Line) {
	Throw(EndNodeNotDeferencable(File, Function, Line));
}

#pragma warning(pop)

inline ID Seed::Core::Exceptions::Exception::GetTargetThreadID()
{
	return reinterpret_cast<ID>(NtCurrentThreadId());
}
