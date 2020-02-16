// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Exceptions.h"
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"
#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.Threading.Impl.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)
//RtlValidateHeap param 3 == null then check all heap memory
#pragma warning(disable: 6387)

using namespace Seed;

///<summary>Інформація про кучу помилок</summary>
struct ExceptionsHeapT
{
	///<summary>Дескриптор кучі</summary>
	Seed::Handle HeapHandle;
	///<summary>Поточне використання кучі</summary>
	volatile Seed::SizeT AllocatedMemory;
	///<summary>Поточна кількість виділених блоків</summary>
	volatile Seed::SizeT AllocatedMemBlksCount;
	///<summary>Фактичне використання пам'яті кучею</summary>
	volatile Seed::SizeT CurrentHeapSize;
};

static __forceinline ExceptionsHeapT& GetExceptionsHeap() {
	static ExceptionsHeapT ExceptionsHeap;
	return ExceptionsHeap;
}

#define ExceptionsHeap GetExceptionsHeap()

Boolean Seed::Core::Init::Exceptions() noexcept
{
	ULONG Flags = HEAP_GROWABLE;

	auto& cfg = CurrentSettings().MemoryManager.ExceptionHeap;

	if (cfg.HeapArgsCheck)
		Flags |= FLG_HEAP_VALIDATE_PARAMETERS;
	if (cfg.HeapFreeCheck)
		Flags |= FLG_HEAP_ENABLE_FREE_CHECK;
	if (cfg.HeapTailCheck)
		Flags |= FLG_HEAP_ENABLE_TAIL_CHECK;
	if (cfg.HeapValidationChecks)
		Flags |= FLG_HEAP_VALIDATE_ALL;

	ExceptionsHeap.HeapHandle = RtlCreateHeap(
		Flags,
		NULL,
		cfg.MaxSize,
		cfg.StartSize,
		NULL,
		NULL);

	if (ExceptionsHeap.HeapHandle == nullptr)
		return false;

	ExceptionsHeap.AllocatedMemBlksCount = ExceptionsHeap.AllocatedMemory = 0;

	Threading::InitMinTls();

	return true;
}

void Seed::Core::Stop::Exceptions() noexcept
{
	Threading::StopMinTls();
	RtlDestroyHeap(ExceptionsHeap.HeapHandle);
}

SeedKernelCall void* Seed::Core::Memory::ExceptionHeap::Allocate(SizeT Size)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(ExceptionsHeap.HeapHandle, 0, nullptr);
#endif
	//Якщо розмір задано максимальне можливе значення SizeT то це явно помилка і генеруєм виключення OutOfMemory
	if (Size == Math::Max<SizeT>())
		return nullptr;
	//Обчислюєм повний розмір ділянки пам'яті
	SizeT FullSize = sizeof(SizeT) + Size;
	//Пробуєм виділити ділянку пам'яті з кучі
	SizeT* obj = (SizeT*)RtlAllocateHeap(ExceptionsHeap.HeapHandle, 0, FullSize);
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

SeedKernelCall void* Seed::Core::Memory::ExceptionHeap::AllignedAllocate(SizeT Size, SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(ExceptionsHeap.HeapHandle, 0, nullptr);
#endif
	//Якщо вирівннювання 0 або 1 то це означає що вирівнювання не потрібне,
	//а отже викликаєм просте виділення пам'яті
	if (AllocationReq == 0 || AllocationReq == 1)
		return Allocate(Size);
	//Пробуєм виділити пам'ять з кучі
	//Виділяєм ділянку більшу для того щоб помістити розмір і реальний початок ділянки пам'яті
	//Також враховуєм що ділянка може бути не вирівнена тому додаєм AllocationReq до розміру,
	//щоб гарантувати щоб вирівнений адрес знайдеться в ділянці
	void* obj = RtlAllocateHeap(ExceptionsHeap.HeapHandle, 0, Size + AllocationReq + sizeof(SizeT) + sizeof(SizeT)); //-V119
	//Якщо функція вернула nullptr то це значить що або розмір завеликий або пам'яті немає
	//В обох випадках генеруєм помилку OutOfMemory
	if (obj == nullptr)
		return nullptr;
	//Вираховуєм мінімальний можливий адресс початку даних в ділянці пам'яті
	SizeT Address = ((SizeT)obj) + sizeof(SizeT) + sizeof(SizeT); //-V119
	//Вираховуєм точний адресс початку ділянки
	SizeT AllignedAddress = Math::RoundToUpper(Address, AllocationReq) * AllocationReq;
	//Отримуєм заголовок ділянки пам'яті
	SizeT* RealPtr = (SizeT*)(AllignedAddress - 2);
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

SeedKernelCall void* Seed::Core::Memory::ExceptionHeap::Free(void* MemBlk)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(ExceptionsHeap.HeapHandle, 0, nullptr);
#endif
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
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&ExceptionsHeap.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&ExceptionsHeap.CurrentHeapSize), -static_cast<LONG64>(FullSize));
		return nullptr;
	}
	//Якщо ні то вертаєм об'єкт
	else return MemBlk;
}

SeedKernelCall void* Seed::Core::Memory::ExceptionHeap::AllignedFree(void* MemBlk, SizeT AllocationReq)
{
#if PreReleaseMode || DebugMode
	RtlValidateHeap(ExceptionsHeap.HeapHandle, 0, nullptr);
#endif
	//Якщо покажчик nullptr то нічого не потрібно робити бо він і так вільний
	if (MemBlk == nullptr) return nullptr;
	//Отримуєм покажчик на заголовок ділянки пам'яті
	SizeT* Header = ((SizeT*)MemBlk) - 2;
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
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&ExceptionsHeap.AllocatedMemory), -static_cast<LONG64>(DataSize));
		InterlockedAddNoFence64(reinterpret_cast<volatile LONG64*>(&ExceptionsHeap.CurrentHeapSize), -static_cast<LONG64>(FullSize));
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

#pragma warning(pop)

inline ID Seed::Core::Exceptions::Exception::GetTargetThreadID()
{
	return reinterpret_cast<ID>(NtCurrentThreadId());
}

#include "Seed.Core.Exceptions.ABI.Public.h"

__declspec(noreturn)
void ThrowBadException()
{
	Throw(Seed::Exceptions::BadException(ExceptionPosition));
}

decltype(typeid(int)) GetBadExceptionTypeId()
{
	return typeid(Seed::Exceptions::BadException);
}

void SehTranslatorFunction(unsigned int exCode, _EXCEPTION_POINTERS * exData)
{
	
    switch (exCode) {
#if !(PreReleaseMode || DebugMode)
    case STATUS_BREAKPOINT:
        Seed::toCoreLog(L"Критична помилка: Відбувається відлагодження");
        break;
#endif
    case STATUS_HEAP_CORRUPTION:
        Seed::toCoreLog(L"Критична помилка: Куча пам'яті пошкоджена");
#if (PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        NtTerminateProcess(NtCurrentProcess(), STATUS_HEAP_CORRUPTION);
        break;
    case STATUS_ACCESS_VIOLATION:
    {
        Seed::toCoreLog(L"Критична помилка: Спроба використати не існуючу ділянку пам'яті");
#if (PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        EXCEPTION_RECORD* sehException = (EXCEPTION_RECORD*)exData->ExceptionRecord; //-V1027
        NullPointException::Types type;
        switch (sehException->ExceptionInformation[0])
        {
        case 0:
            type = NullPointException::Types::Read;
            break;
        case 1:
            type = NullPointException::Types::Write;
            break;
        case 8:
            type = NullPointException::Types::Execute;
            break;
        default:
            type = NullPointException::Types::Read;
            break;
        }
        Throw(Seed::NullPointException(ExceptionPosition, sehException->ExceptionInformation[1], type));
    }
    break;
    default:
        Seed::BaseString<Seed::Core::Memory::ExceptionHeap> str(L"Критична помилка: Невідома помилка. Код: ");
        str +=
            Seed::BaseString<Seed::Core::Memory::ExceptionHeap>::UInt64asHex(exCode);
        Seed::toCoreLog(str);
#if !(PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        NtTerminateProcess(NtCurrentProcess(), exCode);
        break;
    }
}

[[noreturn]] InternalApiCall void EmergencyShutdown(const ConstAString& File, const ConstAString& Function, UInt64 Line, const ConstString& Reason) {
    using SafeString = BaseString<Memory::ExceptionHeap>;
    Exception::ExeptionLocation location{ File,Function,Line };
    toCoreLog(SafeString(L"Аварійне завершення ядра. Причина ") + SafeString(Reason) + SafeString(L" Розташування виклику: ") + location.toString());
    NtTerminateProcess(NtCurrentProcess(), STATUS_INTERNAL_ERROR);
}

#include "Seed.Core.Exceptions.ABI.SEH.h"
#include "Seed.Core.Exceptions.ABI.Defines.h"
#include "Seed.Core.Exceptions.ABI.Types.h"

#pragma warning(push)
#pragma warning(disable: 4996)

int ArrayUnwindFilter(EXCEPTION_POINTERS* pExPtrs)
{
    EHExceptionRecord* pExcept = (EHExceptionRecord*)pExPtrs->ExceptionRecord;

    switch (pExcept->ExceptionCode) //-V2520
    {
    case EH_EXCEPTION_NUMBER:
        EmergencyShutdown(ExceptionPosition, L"Dtors can't throw exceptions");
        //terminate();
#ifdef ALLOW_UNWIND_ABORT
    case EH_ABORT_FRAME_UNWIND_PART:
        return EXCEPTION_EXECUTE_HANDLER;
#endif
    default:
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

void __stdcall __ArrayUnwind(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    int         count,              // Number of elements in the array
    void(__stdcall* pDtor)(void*)    // The destructor to call
)
{
    // 'unwind' rest of array

    __try
    {
        while (--count >= 0)
        {
            ptr = (char*)ptr - size;
            (*pDtor)(ptr);
        }
    }
    __except (ArrayUnwindFilter(exception_info()))
    {
        ; // Deliberately do nothing
    }
}

void __stdcall __ehvec_ctor(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    size_t      count,              // Number of elements in the array
    void(__stdcall* pCtor)(void*),   // Constructor to call
    void(__stdcall* pDtor)(void*)    // Destructor to call should exception be thrown
)
{
    size_t i = 0;      // Count of elements constructed
    int success = 0;

    __try
    {
        // Construct the elements of the array
        for (; i < count; i++)
        {
            (*pCtor)(ptr);
            ptr = (char*)ptr + size;
        }
        success = 1;
    }
    __finally
    {
        if (!success)
            __ArrayUnwind(ptr, size, (int)i, pDtor);
    }
}

void __stdcall __ehvec_dtor(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    //  int         count,              // Number of elements in the array
    size_t      count,              // Number of elements in the array
    void(__stdcall* pDtor)(void*)    // The destructor to call
)
{
    _Analysis_assume_(count > 0);

    int success = 0;

    // Advance pointer past end of array
    ptr = (char*)ptr + size * count;

    __try
    {
        // Destruct elements
        while (count-- > 0)
        {
            ptr = (char*)ptr - size;
            (*pDtor)(ptr);
        }
        success = 1;
    }
    __finally
    {
        if (!success)
            __ArrayUnwind(ptr, size, (int)count, pDtor);
    }
}

#pragma warning(pop)


Seed::Exception::RawException Seed::Core::Exceptions::Exception::StoreCurrentException()
{
    //if (GetThreadExceptionData()->_isCatchBlockProceding) {
    //    RawExceptionInfo* current = static_cast<RawExceptionInfo*>(ThreadExceptionInfo->_curexception);
    //    RawExceptionInfo* record = static_cast<RawExceptionInfo*>(InternalHeapAllocate(sizeof(RawExceptionInfo)));
    //    RtlSecureZeroMemory(record, sizeof(RawExceptionInfo));
    //    _CopyException(current, record);
    //    return RawException(record);
    //}
    //___cxxthrow(Exceptions::NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
    return RawException(nullptr);
}

void ThrowExceptionFromExceptionStorage(EXCEPTION_RECORD record) {
    //RtlRaiseException(&record);
}

void Seed::Core::Exceptions::Exception::ThrowRawException(RawException&& exception)
{
//    if (exception.ptr == nullptr)
//        ___cxxthrow(Exceptions::NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
//    EXCEPTION_RECORD record = { 0 };
//    EXCEPTION_RECORD* ex = (EXCEPTION_RECORD*)exception.ptr;
//    auto copy = (RawExceptionInfo*)&record;
//    auto orig = (RawExceptionInfo*)ex;
//
//    //copying the _EXCEPTION_RECORD (for SEH exceptions)
//    copy->NtExceptionInfo.ExceptionCode = orig->NtExceptionInfo.ExceptionCode;
//    copy->NtExceptionInfo.ExceptionFlags = orig->NtExceptionInfo.ExceptionFlags;
//    copy->NtExceptionInfo.ExceptionRecord = NULL; // We don't chain SEH exceptions
//    copy->NtExceptionInfo.ExceptionAddress = NULL; // Useless field to copy->  It will be overwritten by RaiseException()
//    copy->NtExceptionInfo.NumberParameters = orig->NtExceptionInfo.NumberParameters;
//
//    // copying any addition parameters
//    for (ULONG i = 0; i < copy->NtExceptionInfo.NumberParameters && i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
//    {
//        copy->NtExceptionInfo.ExceptionInformation[i] = orig->NtExceptionInfo.ExceptionInformation[i];
//    }
//    // NULLing out any un-used parameters
//    for (ULONG i = copy->NtExceptionInfo.NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
//    {
//        copy->NtExceptionInfo.ExceptionInformation[i] = 0;
//    }
//
//    if (orig->CxxExceptionInfo.ExceptionCode == EH_EXCEPTION_NUMBER &&
//        orig->CxxExceptionInfo.NumberParameters == EH_EXCEPTION_PARAMETERS && (
//            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER1 ||
//            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER2 ||
//            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER3))
//    {
//        // this is a C++ exception
//        // we need to copy the C++ exception object
//
//        // nulling out the Exception object pointer, because we haven't copied it yet
//        copy->CxxExceptionInfo.params.pExceptionObject = NULL;
//
//        ThrowInfo* pThrow = orig->CxxExceptionInfo.params.pThrowInfo;
//        if (orig->CxxExceptionInfo.params.pExceptionObject == NULL ||
//            pThrow == NULL ||
//            pThrow->pCatchableTypeArray == NULL
//            )
//        {
//            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
//            //terminate();
//            EmergencyShutdown(ExceptionPosition, L"Throw info empty");
//        }
//
//        // we finally got the type info we want
//        //auto typeInfo = GetByOffset<CatchableType>((SizeT)orig->CxxExceptionInfo.params.pThrowImageBase, pThrow->pCatchableTypeArray);
//#define THROW_CTLIST_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->arrayOfCatchableTypes)
//#define THROW_CTARRAY_IB(t,ib)			((CatchableTypeArray*)((ib) + (t).pCatchableTypeArray))
//#define _GetThrowImageBase() (SizeT)orig->CxxExceptionInfo.params.pThrowImageBase
//        const auto typeInfo = (CatchableType*)
//            (*(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase()); //-V1004
//        //		const auto typeInfo2 = ;
//        //		const auto typeInfo = (CatchableType*)(*(int*)
//        //			GetByOffset<CatchableTypeArray>(
//        //			(SizeT)orig->CxxExceptionInfo.params.pThrowImageBase,
//        //				pThrow->pCatchableTypeArray
//        //				) + (SizeT)orig->CxxExceptionInfo.params.pThrowImageBase);
//        void* pExceptionBuffer = NULL;
//
//        InternalHeapFree(ThreadExceptionInfo->throwObjStorageForStoredException);
//
//        if (sizeof(NtException) != typeInfo->sizeOrOffset)
//            toCoreLog(L"Попередження: розмір типу не збігається з інформацією з метаданих виконавчого файла");
//
//        ThreadExceptionInfo->throwObjStorageForStoredException = InternalHeapAllocate(typeInfo->sizeOrOffset);
//
//        pExceptionBuffer = ThreadExceptionInfo->throwObjStorageForStoredException;
//
//        _CallCopyCtor(pExceptionBuffer, orig->CxxExceptionInfo.params.pExceptionObject, typeInfo->sizeOrOffset, typeInfo, orig->CxxExceptionInfo.params.pThrowImageBase);
//
//        copy->CxxExceptionInfo.params.pExceptionObject = pExceptionBuffer;
//    }
//    ThrowExceptionFromExceptionStorage(record);
}

Seed::Core::Exceptions::Exception::RawException::~RawException()
{
    //if (this->ptr != nullptr) {
    //    RawExceptionInfo* data = (RawExceptionInfo*)this->ptr;
    //    __DestructExceptionObject(&data->CxxExceptionInfo, TRUE, 0);
    //    data->CxxExceptionInfo.params.pExceptionObject = InternalHeapFree(data->CxxExceptionInfo.params.pExceptionObject);
    //    this->ptr = InternalHeapFree(this->ptr);
    //}
}