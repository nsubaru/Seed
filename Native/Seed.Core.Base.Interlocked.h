#pragma once
#include "Seed.Core.Base.Types.h"

#pragma region Intrisc definition
extern "C" {
	long _InterlockedAdd(
		long volatile* Addend,
		long Value
	);
	__int64 _InterlockedAdd64(
		__int64 volatile* Addend,
		__int64 Value
	);
	short _InterlockedIncrement16(
		volatile short* lpAddend
	);
	long _InterlockedIncrement(
		volatile long* lpAddend
	);
	__int64 _InterlockedIncrement64(
		volatile __int64* lpAddend
	);
	long _InterlockedDecrement(
		volatile long* lpAddend
	);
	short _InterlockedDecrement16(
		volatile short* lpAddend
	);
	__int64 _InterlockedDecrement64(
		volatile __int64* lpAddend
	);
	short _InterlockedCompareExchange16(
		short volatile* Destination,
		short Exchange,
		short Comperand
	);
	long _InterlockedCompareExchange(
		long volatile* Destination,
		long Exchange,
		long Comperand
	);
	__int64 _InterlockedCompareExchange64(
		__int64 volatile* Destination,
		__int64 Exchange,
		__int64 Comperand
	);
}
#pragma endregion


namespace Seed {
	inline namespace Core {
		///<summary>Набір атомарних InterlockedXXX операцій</summary>
		class Interlocked final {
		public:
#pragma region Add
			///<summary>Атомарне додавання</summary>
			///<param name = "To">volatile зміна до якої додасться значення What</param>
			///<param name = "What">зміна яка додасться</param>
			///<returns>Нове значення зміної To</returns>
			__forceinline static Int32 InterlockedAdd(volatile Int32& To, Int32 What) {
				return _InterlockedAdd(reinterpret_cast<volatile long*>(&To), What);
			}

			///<summary>Атомарне додавання</summary>
			///<param name = "To">volatile зміна до якої додасться значення What</param>
			///<param name = "What">зміна яка додасться</param>
			///<returns>Нове значення зміної To</returns>
			__forceinline static UInt32 InterlockedAdd(volatile UInt32& To, UInt32 What) {
				return _InterlockedAdd(reinterpret_cast<volatile long*>(&To), What);
			}
			///<summary>Атомарне додавання</summary>
			///<param name = "To">volatile зміна до якої додасться значення What</param>
			///<param name = "What">зміна яка додасться</param>
			///<returns>Нове значення зміної To</returns>
			__forceinline static Int64 InterlockedAdd(volatile Int64& To, Int64 What) {
				return _InterlockedAdd64(reinterpret_cast<volatile __int64*>(&To), What);
			}

			///<summary>Атомарне додавання</summary>
			///<param name = "To">volatile зміна до якої додасться значення What</param>
			///<param name = "What">зміна яка додасться</param>
			///<returns>Нове значення зміної To</returns>
			__forceinline static UInt64 InterlockedAdd(volatile UInt64& To, UInt64 What) {
				return _InterlockedAdd64(reinterpret_cast<volatile __int64*>(&To), What);
			}
#pragma endregion
#pragma region Increment
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int16 Increment(volatile Int16& What) {
				return _InterlockedIncrement16(reinterpret_cast<volatile short*>(&What));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt16 Increment(volatile UInt16& What) {
				return _InterlockedIncrement16(reinterpret_cast<volatile short*>(&What));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int32 Increment(volatile Int32& What) {
				return _InterlockedIncrement(reinterpret_cast<volatile long*>(&What));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt32 Increment(volatile UInt32& What) {
				return _InterlockedIncrement(reinterpret_cast<volatile long*>(&What));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int64 Increment(volatile Int64& What) {
				return _InterlockedIncrement64(reinterpret_cast<volatile long long*>(&What));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt64 Increment(volatile UInt64& What) {
				return _InterlockedIncrement64(reinterpret_cast<volatile long long*>(&What));
			}
#pragma endregion
#pragma region Decrement
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int16 Decrement(volatile Int16& What) {
				return _InterlockedDecrement16(reinterpret_cast<volatile short*>(&What));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt16 Decrement(volatile UInt16& What) {
				return _InterlockedDecrement16(reinterpret_cast<volatile short*>(&What));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int32 Decrement(volatile Int32& What) {
				return _InterlockedDecrement(reinterpret_cast<volatile long*>(&What));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt32 Decrement(volatile UInt32& What) {
				return _InterlockedDecrement(reinterpret_cast<volatile long*>(&What));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int64 Decrement(volatile Int64& What) {
				return _InterlockedDecrement64(reinterpret_cast<volatile long long*>(&What));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "What">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt64 Decrement(volatile UInt64& What) {
				return _InterlockedDecrement64(reinterpret_cast<volatile long long*>(&What));
			}
#pragma endregion
#pragma region CompareAndSwap
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int16 CompareAndSwap(volatile Int16& Destination, Int16 Exchange, Int16 Comperand) {
				return _InterlockedCompareExchange16(reinterpret_cast<volatile short*>(&Destination), Exchange, Comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt16 CompareAndSwap(volatile UInt16& Destination, UInt16 Exchange, UInt16 Comperand) {
				return _InterlockedCompareExchange16(reinterpret_cast<volatile short*>(&Destination), Exchange, Comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int32 CompareAndSwap(volatile Int32& Destination, Int32 Exchange, Int32 Comperand) {
				return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&Destination), Exchange, Comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt32 CompareAndSwap(volatile UInt32& Destination, UInt32 Exchange, UInt32 Comperand) {
				return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&Destination), Exchange, Comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int64 CompareAndSwap(volatile Int64& Destination, Int64 Exchange, Int64 Comperand) {
				return _InterlockedCompareExchange64(reinterpret_cast<volatile long long*>(&Destination), Exchange, Comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює Destination з Comperand і обмінює якщо рівне</para></summary>
			///<param name = "Destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "Exchange">Нове значення</param>
			///<param name = "Comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt64 CompareAndSwap(volatile UInt64& Destination, UInt64 Exchange, UInt64 Comperand) {
				return _InterlockedCompareExchange64(reinterpret_cast<volatile long long*>(&Destination), Exchange, Comperand);
			}
#pragma endregion
		};
	}
}

#undef _InterlockedAdd
#undef _InterlockedAdd64
#undef _InterlockedIncrement
#undef _InterlockedIncrement16
#undef _InterlockedIncrement64
#undef _InterlockedDecrement
#undef _InterlockedDecrement16
#undef _InterlockedDecrement64
#undef _InterlockedCompareExchange
#undef _InterlockedCompareExchange16
#undef _InterlockedCompareExchange64