// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

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
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)
//RtlValidateHeap - if BaseAddr == nullptr then RtlValidateHeap validate all heap chunks.
//But this option not documented and VS warn as 3 param of RtlValidateHeap is invalid
#pragma warning(disable: 6387)
//This file use SAL annotation which invisible for Seed C++ Runtime API
#pragma warning(disable: 28301)

using namespace Seed;
using namespace Seed::Memory;

struct HeapDataT
{
	Handle HeapHandle;
	volatile SizeT AllocatedMemory;
	volatile SizeT AllocatedMemBlksCount;
	volatile SizeT CurrentHeapSize;
};

ForceInline static HeapDataT& GetInternalHeapData() {
	static HeapDataT InternalHeapData;
	return InternalHeapData;
}

ForceInline static HeapDataT& GetHeapData() {
	static HeapDataT HeapData;
	return HeapData;
}

ForceInline static HeapDataT& GetSecureHeapData() {
	static HeapDataT SecureHeapData;
	return SecureHeapData;
}

#define InternalHeapData GetInternalHeapData()
#define HeapData GetHeapData()
#define SecureHeapData GetSecureHeapData()

constexpr Seed::ConstString InternalHeapName = L"внутрішній";
constexpr Seed::ConstString DefaultHeapName = L"загальній";
constexpr Seed::ConstString SecureHeapName = L"захищеній";

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
	ULONG Flags = HEAP_GROWABLE;

	auto& cfg = CurrentSettings().MemoryManager.DefaultHeap;

	if (cfg.HeapArgsCheck)
		Flags |= FLG_HEAP_VALIDATE_PARAMETERS;
	if (cfg.HeapFreeCheck)
		Flags |= FLG_HEAP_ENABLE_FREE_CHECK;
	if (cfg.HeapTailCheck)
		Flags |= FLG_HEAP_ENABLE_TAIL_CHECK;
	if (cfg.HeapValidationChecks)
		Flags |= FLG_HEAP_VALIDATE_ALL;

	HeapData.HeapHandle = RtlCreateHeap(
		Flags,
		NULL,
		cfg.StartSize,
		cfg.MaxSize,
		NULL,
		NULL);
	if (HeapData.HeapHandle == nullptr)
		return false;
	HeapData.AllocatedMemBlksCount = HeapData.AllocatedMemory = HeapData.CurrentHeapSize = 0;
	return true;
}

void Seed::Core::Memory::Heap::Stop() noexcept
{
	if (HeapData.AllocatedMemBlksCount > 0)
		toCoreLog(L"Попередження: виявлено утічку пам'яті в стандартній кучі");
	RtlDestroyHeap(HeapData.HeapHandle);
}

#ifndef ReleaseMode
__declspec(allocator)
#endif // !ReleaseMode
void * Seed::Core::Memory::Heap::Allocate(SizeT Size)
{
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapAllocsLogging) {
		LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
	}
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapValidationChecks)
	{
		RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	}
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, DefaultHeapName, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі
	SizeT* obj = (SizeT*)RtlAllocateHeap(HeapData.HeapHandle, 0, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, DefaultHeapName, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&HeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&HeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

#ifndef ReleaseMode
__declspec(allocator)
#endif // !ReleaseMode
void * Seed::Core::Memory::Heap::AllignedAllocate(SizeT Size, SizeT AllocationReq)
{
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне,
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapAllocsLogging) {
		LogAllocating(Size, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
	}
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapValidationChecks)
	{
		RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	}
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	void* obj = RtlAllocateHeap(HeapData.HeapHandle, 0, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, DefaultHeapName, Size, HeapData.AllocatedMemory, HeapData.AllocatedMemBlksCount));
	//Вираховуєм мінімальний можливий адресс початку даних в ділянці пам'яті
	SizeT Address = ((SizeT)obj) + sizeof(SizeT) + sizeof(SizeT); //-V119
	//Вираховуєм точний адресс початку ділянки
	SizeT AllignedAddress = Math::RoundToUpper(Address, AllocationReq) * AllocationReq;
	//Отримуєм заголовок ділянки пам'яті
	SizeT* RealPtr = (SizeT*)(AllignedAddress - (2 * sizeof(SizeT)));
	//Записуєм реальний початок ділянки пам'яті
	RealPtr[0] = (SizeT)obj;
	//Та розмір даних
	RealPtr[1] = Size;
	//Атомарно оновлюєм статитистику
	InterlockedIncrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&HeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&HeapData.CurrentHeapSize, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT));
	//Вертаєм покажчик на початок даних
	return (void*)AllignedAddress;
}

void * Seed::Core::Memory::Heap::Free(void * MemBlk)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapValidationChecks)
	{
		RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	}
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapAllocsLogging) {
		LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
	}
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + DataSize;
	//Пробуєм звільнити пам'ять
	if (RtlFreeHeap(HeapData.HeapHandle, 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&HeapData.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&HeapData.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

void * Seed::Core::Memory::Heap::AllignedFree(void * MemBlk, SizeT AllocationReq)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно перевіряєм цілісність кучі
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapAllocsLogging) { //-V807
		RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	}
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapValidationChecks)
	{
		RtlValidateHeap(HeapData.HeapHandle, 0, nullptr);
	}
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне,
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Free(MemBlk);
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* Header = ((SizeT*)MemBlk) - 2;
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[1];
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.DefaultHeap.HeapAllocsLogging) {
		LogFree(DataSize, HeapData.AllocatedMemBlksCount + 1, L"Загальна");
	}
	//Отрмуєм покажчик на початок ділянки пам'яті
	void* RealPtr = (void*)Header[0];
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + sizeof(SizeT) + AllocationReq + DataSize;
	//Пробуєм звільнити ділянку пам'яті
	if (RtlFreeHeap(HeapData.HeapHandle, 0, RealPtr)) {
		//Якшо успішно то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&HeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&HeapData.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&HeapData.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentMemBlkCount()
{
	return HeapData.AllocatedMemBlksCount;
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentAllocatedMemory()
{
	return HeapData.AllocatedMemory;
}

Seed::SizeT Seed::Core::Memory::Heap::CurrentHeapUsage()
{
	return HeapData.CurrentHeapSize;
}
#pragma endregion
#pragma region Sensitive heap

Seed::Boolean Seed::Core::Memory::SensitiveDataHeap::Init() noexcept
{
	ULONG Flags = HEAP_GROWABLE;

	auto& cfg = CurrentSettings().MemoryManager.SensitiveDataHeap;

	if (cfg.HeapArgsCheck)
		Flags |= FLG_HEAP_VALIDATE_PARAMETERS;
	if (cfg.HeapFreeCheck)
		Flags |= FLG_HEAP_ENABLE_FREE_CHECK;
	if (cfg.HeapTailCheck)
		Flags |= FLG_HEAP_ENABLE_TAIL_CHECK;
	if (cfg.HeapValidationChecks)
		Flags |= FLG_HEAP_VALIDATE_ALL;

	SecureHeapData.HeapHandle = RtlCreateHeap(
		Flags,
		NULL,
		cfg.StartSize,
		cfg.MaxSize,
		NULL,
		NULL);
	if (SecureHeapData.HeapHandle == nullptr)
		return false;
	SecureHeapData.AllocatedMemBlksCount = SecureHeapData.AllocatedMemory = SecureHeapData.CurrentHeapSize = 0;
	return true;
}

void Seed::Core::Memory::SensitiveDataHeap::Stop() noexcept
{
	if (SecureHeapData.AllocatedMemBlksCount > 0)
		toCoreLog(L"Попередження: виявлено утічку пам'яті в захищеній кучі");
	RtlDestroyHeap(SecureHeapData.HeapHandle);
}

#ifndef ReleaseMode
__declspec(allocator)
#endif // !ReleaseMode
DeprecatedCall("Низькорівнева робота з пам'ятю") void* Seed::Core::Memory::SensitiveDataHeap::Allocate(Seed::SizeT Size)
{
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapAllocsLogging) {
		LogAllocating(Size, SecureHeapData.AllocatedMemBlksCount + 1, L"Безпечна");
	}
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapValidationChecks)
	{
		RtlValidateHeap(SecureHeapData.HeapHandle, 0, nullptr);
	}
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, SecureHeapName, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі і вказуєм системі обнулити ділянку пам'яті
	SizeT* obj = (SizeT*)RtlAllocateHeap(SecureHeapData.HeapHandle, HEAP_ZERO_MEMORY, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, SecureHeapName, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&SecureHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&SecureHeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

#ifndef ReleaseMode
__declspec(allocator)
#endif // !ReleaseMode
DeprecatedCall("Низькорівнева робота з пам'ятю") void* Seed::Core::Memory::SensitiveDataHeap::AllignedAllocate(Seed::SizeT Size, Seed::SizeT AllocationReq)
{
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне,
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapAllocsLogging) {
		LogAllocating(Size, SecureHeapData.AllocatedMemBlksCount + 1, L"Безпечна");
	}
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapValidationChecks)
	{
		RtlValidateHeap(SecureHeapData.HeapHandle, 0, nullptr);
	}
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	//Вказуєм системі обнулити ділянку пам'яті пееред виділенням
	void* obj = RtlAllocateHeap(SecureHeapData.HeapHandle, HEAP_ZERO_MEMORY, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, SecureHeapName, Size, SecureHeapData.AllocatedMemory, SecureHeapData.AllocatedMemBlksCount));
	//Вираховуєм мінімальний можливий адресс початку даних в ділянці пам'яті
	SizeT Address = ((SizeT)obj) + sizeof(SizeT) + sizeof(SizeT); //-V119
	//Вираховуєм точний адресс початку ділянки
	SizeT AllignedAddress = Math::RoundToUpper(Address, AllocationReq) * AllocationReq;
	//Отримуєм заголовок ділянки пам'яті
	SizeT* RealPtr = (SizeT*)(AllignedAddress - (2 * sizeof(SizeT)));
	//Записуєм реальний початок ділянки пам'яті
	RealPtr[0] = (SizeT)obj;
	//Та розмір даних
	RealPtr[1] = Size;
	//Атомарно оновлюєм статитистику
	InterlockedIncrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&SecureHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&SecureHeapData.CurrentHeapSize, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT));
	//Вертаєм покажчик на початок даних
	return (void*)AllignedAddress;
}

DeprecatedCall("Низькорівнева робота з пам'ятю") void* Seed::Core::Memory::SensitiveDataHeap::Free(void* MemBlk)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapValidationChecks)
	{
		RtlValidateHeap(SecureHeapData.HeapHandle, 0, nullptr);
	}
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* RealPtr = ((SizeT*)MemBlk) - 1;
	//Отримуєм розмір ділянки пам'яті
	SizeT DataSize = RealPtr[0];
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapAllocsLogging) {
		LogFree(DataSize, SecureHeapData.AllocatedMemBlksCount + 1, L"Безпечна");
	}
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + DataSize;
	//Обнулюєм ділянку пам'яті
	RtlSecureZeroMemory(MemBlk, DataSize);
	//Пробуєм звільнити пам'ять
	if (RtlFreeHeap(SecureHeapData.HeapHandle, 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&SecureHeapData.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&SecureHeapData.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

DeprecatedCall("Низькорівнева робота з пам'ятю") void* Seed::Core::Memory::SensitiveDataHeap::AllignedFree(void* MemBlk, Seed::SizeT AllocationReq)
{
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Якщо валідація пам'яті включена то виконуєм додаткову перевірку
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapValidationChecks)
	{
		RtlValidateHeap(SecureHeapData.HeapHandle, 0, nullptr);
	}
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* Header = ((SizeT*)MemBlk) - 2;
	//Отрмуєм покажчик на початок ділянки пам'яті
	void* RealPtr = (void*)Header[0];
	//Отримуєм розмір даних в ділянці пам'яті
	SizeT DataSize = Header[1];
	//Якщо включено логування виділеннь\звільненнь пам'ять то відповідно логуєм дані
	if (CurrentSettings().MemoryManager.SensitiveDataHeap.HeapAllocsLogging) {
		LogFree(DataSize, SecureHeapData.AllocatedMemBlksCount + 1, L"Безпечна");
	}
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + sizeof(SizeT) + AllocationReq + DataSize;
	//Обнулюєм ділянку пам'яті
	RtlSecureZeroMemory(MemBlk, DataSize);
	//Пробуєм звільнити ділянку пам'яті
	if (RtlFreeHeap(SecureHeapData.HeapHandle, 0, RealPtr)) {
		//Якшо успішно то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&SecureHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&SecureHeapData.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&SecureHeapData.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentMemBlkCount()
{
	return SecureHeapData.AllocatedMemBlksCount;
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentAllocatedMemory()
{
	return SecureHeapData.AllocatedMemory;
}

Seed::SizeT Seed::Core::Memory::SensitiveDataHeap::CurrentHeapUsage()
{
	return SecureHeapData.CurrentHeapSize;
}
#pragma endregion

#pragma region Internal heap allocate and free
DeprecatedCall("Низькорівнева робота з пам'ятю") void* InternalHeapFree(void* MemBlk)
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
	if (RtlFreeHeap(RtlProcessHeap(), 0, RealPtr)) {
		//Якщо вдало то оновлюєм статистику
		InterlockedDecrementSizeTNoFence(&InternalHeapData.AllocatedMemBlksCount);
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&InternalHeapData.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&InternalHeapData.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то генеруєм помилку NullPointException
	else Throw(NullPointException(__FILE__, __FUNCSIG__, __LINE__, (SizeT)MemBlk, NullPointException::Types::Read));
}

#ifndef ReleaseMode
__declspec(allocator)
#endif // !ReleaseMode
DeprecatedCall("Низькорівнева робота з пам'ятю") void* InternalHeapAllocate(Seed::SizeT Size)
{
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, InternalHeapName, Size, InternalHeapData.AllocatedMemory, InternalHeapData.AllocatedMemBlksCount));
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі і вказуєм системі обнулити ділянку пам'яті
	SizeT* obj = (SizeT*)RtlAllocateHeap(RtlProcessHeap(), 0, FullSize);
	//Якщо покажчик рівний nullptr то це значить що не можливо виділити пам'ять, а це значить що не достатньо пам'яті в системі
	//Відповідно генеруєм виключення OutOfMemory
	if (obj == nullptr)
		Throw(OutOfMemory(__FILE__, __FUNCSIG__, __LINE__, InternalHeapName, Size, InternalHeapData.AllocatedMemory, InternalHeapData.AllocatedMemBlksCount));
	//Записуєм розмір ділянки в заголовок
	obj[0] = Size;
	//Атомарно оновлюєм статистику
	InterlockedIncrementSizeTNoFence(&InternalHeapData.AllocatedMemBlksCount);
	InterlockedExchangeAddSizeT(&InternalHeapData.AllocatedMemory, Size);
	InterlockedExchangeAddSizeT(&InternalHeapData.CurrentHeapSize, FullSize);
	//Вертаєм покажчик на ділянку після заголовку
	return obj + 1;
}

SizeT Seed::Core::Memory::CurrentInternalHeapAllocatedMemBlks()
{
	return InternalHeapData.AllocatedMemBlksCount;
}

SizeT Seed::Core::Memory::CurrentInternalHeapAllocatedMemory()
{
	return InternalHeapData.AllocatedMemory;
}

SizeT Seed::Core::Memory::CurrentInternalHeapHeapsUsage()
{
	return InternalHeapData.CurrentHeapSize;
}
#pragma endregion

#pragma region Statistic
SizeT Seed::Core::Memory::CurrentTotalAllocatedMemBlks()
{
	return InternalHeapData.AllocatedMemBlksCount + HeapData.AllocatedMemBlksCount + SecureHeapData.AllocatedMemBlksCount;
}

SizeT Seed::Core::Memory::CurrentTotalAllocatedMemory()
{
	return InternalHeapData.AllocatedMemory + HeapData.AllocatedMemory + SecureHeapData.CurrentHeapSize;
}

SizeT Seed::Core::Memory::CurrentTotalHeapsUsage()
{
	return InternalHeapData.CurrentHeapSize + HeapData.CurrentHeapSize + SecureHeapData.CurrentHeapSize;
}
#pragma endregion
#pragma region Miscelenious functions
#pragma intrinsic(memcpy)
DeprecatedCall("Низькорівнева робота з пам'ятю") void Seed::Core::Memory::Copy(
	_In_reads_(Size) const void* Source,
	_Out_writes_all_(Size) void* Destination,
	Seed::SizeT Size)
{
	memcpy(Destination, Source, Size);
}

#pragma intrinsic(memset)
DeprecatedCall("Низькорівнева робота з пам'ятю") void Seed::Core::Memory::MemBlkFill(
	_Out_writes_bytes_all_(Size) void* MemBlk,
	Seed::SizeT Size,
	_In_ const Seed::Byte Filler)
{
	memset(MemBlk, Filler, Size);
}

#pragma endregion
#pragma region Init and stop

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

Seed::Boolean Seed::Core::Init::MemoryManagment() noexcept {
	return Seed::Core::Memory::Init();
}

void Seed::Core::Stop::MemoryManagment() noexcept
{
	Memory::Stop();
}
#pragma endregion

#pragma warning(pop)