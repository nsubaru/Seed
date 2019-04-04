#include "Seed.Core.Memory.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.Exceptions.h"

#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

using namespace Seed;
using namespace Seed::Memory;

///<summary>Інформація про кучі</summary>
static struct
{
	///<summary>Дескриптор кучі</summary>
	Handle HeapHandle = nullptr;
	///<summary>Поточне використання кучі</summary>
	SizeT AllocatedMemory = 0;
	///<summary>Поточна кількість виділених блоків</summary>
	SizeT AllocatedMemBlksCount = 0;
	///<summary>Фактичне використання пам'яті кучею</summary>
	SizeT CurrentHeapSize = 0;
} InternalHeapData, HeapData, SecureHeapData;


#pragma region Loging
template<SizeT HeapNameSize>
void LogAllocating(SizeT MemBlkSize, SizeT MemBlkIndex, const wchar_t(&HeapName)[HeapNameSize]) {
	using SafeString = BaseString<Memory::ExceptionHeap>;
	SafeString str(100);
	str += L"Відлагодження: виділення ";
	str += SafeString::toString(MemBlkIndex);
	str += L" розміром ";
	str += SafeString::toString(MemBlkSize);
	str += L" байт з кучі "; 
	str += HeapName; 
	str += L".";
	toCoreLog(str);
}

template<SizeT HeapNameSize>
void LogFree(SizeT MemBlkSize, SizeT MemBlkIndex, const wchar_t(&HeapName)[HeapNameSize]) {
	using SafeString = BaseString<Memory::ExceptionHeap>;
	SafeString str(100);
	str += L"Відлагодження: звільнення ";
	str += SafeString::toString(MemBlkIndex);
	str += L" розміром ";
	str += SafeString::toString(MemBlkSize);
	str += L" байт з кучі ";
	str += HeapName;
	str += L".";
	toCoreLog(str);
}
#pragma endregion

#pragma region Default heap
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
	if (HeapData.AllocatedMemBlksCount > 0)
		toCoreLog(L"Попередження: виявлено утічку пам'яті в стандартній кучі");
	RtlDestroyHeap(HeapData.HeapHandle);
}

void * GenericAllocate(Seed::SizeT Size)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
#endif
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
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

void * GenericAllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
#endif
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне,
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return GenericAllocate(Size);
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

void * GenericFree(void * MemBlk)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
#if PreReleaseMode || DebugMode
	LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
#endif
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
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

void * GenericAllignedFree(void * MemBlk, Seed::SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT * Header = ((SizeT*)MemBlk) - 2;
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[0];
#if PreReleaseMode || DebugMode
	LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
#endif
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
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
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
#pragma endregion
#pragma region Sensitive heap

Seed::Boolean Seed::Core::Memory::SensitiveDataHeap::Init() noexcept
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

void Seed::Core::Memory::SensitiveDataHeap::Stop() noexcept
{
	if (SecureHeapData.AllocatedMemBlksCount > 0)
		toCoreLog(L"Попередження: виявлено утічку пам'яті в захищеній кучі");
	RtlDestroyHeap(SecureHeapData.HeapHandle);
}

DeprecatedCall("Низькорівнева робота з пам'ятю") void * Seed::Core::Memory::SensitiveDataHeap::Allocate(Seed::SizeT Size)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Безпечна");
#endif
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
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

DeprecatedCall("Низькорівнева робота з пам'ятю") void * Seed::Core::Memory::SensitiveDataHeap::AllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Безпечна");
#endif
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

DeprecatedCall("Низькорівнева робота з пам'ятю") void * Seed::Core::Memory::SensitiveDataHeap::Free(void * MemBlk)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
#if PreReleaseMode || DebugMode
	LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Безпечна");
#endif
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
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

DeprecatedCall("Низькорівнева робота з пам'ятю") void * Seed::Core::Memory::SensitiveDataHeap::AllignedFree(void * MemBlk, Seed::SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT * Header = ((SizeT*)MemBlk) - 2;
	//Отрмуєм покажчик на початок ділянки пам'яті
	void* RealPtr = (void*)Header[0];
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[1];
#if PreReleaseMode || DebugMode
	LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Безпечна");
#endif
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
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentMemBlkCount() noexcept
{
	return SecureHeapData.AllocatedMemBlksCount;
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentAllocatedMemory() noexcept
{
	return SecureHeapData.AllocatedMemory;
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentHeapUsage() noexcept
{
	return SecureHeapData.CurrentHeapSize;
}
#pragma endregion

#pragma region Internal heap allocate and free
DeprecatedCall("Низькорівнева робота з пам'ятю") void* InternalHeapFree(void* MemBlk)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(RtlProcessHeap(), 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT * RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + DataSize;
	//Обнулюєм ділянку пам'яті
	RtlSecureZeroMemory(MemBlk, DataSize);
	//Пробуєм звільнити пам'ять
	if (RtlFreeHeap(RtlProcessHeap(), 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&InternalHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64((LONG64*)& InternalHeapData.AllocatedMemory, -(LONG64)DataSize);
		InterlockedAddNoFence64((LONG64*)& InternalHeapData.CurrentHeapSize, -(LONG64)FullSize);
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

DeprecatedCall("Низькорівнева робота з пам'ятю") void* InternalHeapAllocate(Seed::SizeT Size)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(RtlProcessHeap(), 0, nullptr);
#endif
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, InternalHeapData.AllocatedMemory, InternalHeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі і вказуєм системі обнулити ділянку пам'яті
	SizeT * obj = (SizeT*)RtlAllocateHeap(RtlProcessHeap(), 0, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, Size, InternalHeapData.AllocatedMemory, InternalHeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&InternalHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&InternalHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&InternalHeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

SizeT Seed::Core::Memory::CurrentInternalHeapAllocatedMemBlks() noexcept
{
	return InternalHeapData.AllocatedMemBlksCount;
}

SizeT Seed::Core::Memory::CurrentInternalHeapAllocatedMemory() noexcept
{
	return InternalHeapData.AllocatedMemory;
}

SizeT Seed::Core::Memory::CurrentInternalHeapHeapsUsage() noexcept
{
	return InternalHeapData.CurrentHeapSize;
}
#pragma endregion

#pragma region Statistic
SizeT Seed::Core::Memory::CurrentTotalAllocatedMemBlks() noexcept
{
	return InternalHeapData.AllocatedMemBlksCount + HeapData.AllocatedMemBlksCount + SecureHeapData.AllocatedMemBlksCount;
}

SizeT Seed::Core::Memory::CurrentTotalAllocatedMemory() noexcept
{
	return InternalHeapData.AllocatedMemory + HeapData.AllocatedMemory + SecureHeapData.CurrentHeapSize;
}

SizeT Seed::Core::Memory::CurrentTotalHeapsUsage() noexcept
{
	return InternalHeapData.CurrentHeapSize + HeapData.CurrentHeapSize + SecureHeapData.CurrentHeapSize;
}
#pragma endregion
#pragma region Miscelenious functions
#pragma intrinsic(memcpy)
DeprecatedCall("Низькорівнева робота з пам'ятю") void Seed::Core::Memory::Copy(
	_In_reads_(Size) const void * Source,
	_Out_writes_all_(Size) void * Destination,
	Seed::SizeT Size)
{
	memcpy(Destination, Source, Size);
}

#pragma intrinsic(memset)
DeprecatedCall("Низькорівнева робота з пам'ятю") void Seed::Core::Memory::MemBlkFill(
	_Out_writes_bytes_all_(Size) void * MemBlk,
	Seed::SizeT Size,
	_In_ const Seed::Byte Filler)
{
	memset(MemBlk, Filler, Size);
}

#pragma endregion
#pragma region Init and stop

Seed::Boolean Seed::Core::Init::MemoryManagment() noexcept {
	return Seed::Core::Memory::Init();
}

Seed::Boolean Seed::Core::Memory::Init()
{
	Memory::Heap::Init();
	Memory::SensitiveDataHeap::Init();
	toCoreLog(L"Підсистема оперативної пам'яті ініціалізована");
	return true;
}

void Seed::Core::Memory::Stop()
{
	Memory::SensitiveDataHeap::Stop();
	Memory::Heap::Stop();
	toCoreLog(L"Підсистема оперативної пам'яті зупинена успішно");
}

void Seed::Core::Stop::MemoryManagment() noexcept
{
	Memory::Stop();
}
#pragma endregion

#pragma warning(pop)