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
		short volatile* destination,
		short exchange,
		short comperand
	);
	long _InterlockedCompareExchange(
		long volatile* destination,
		long exchange,
		long comperand
	);
	__int64 _InterlockedCompareExchange64(
		__int64 volatile* destination,
		__int64 exchange,
		__int64 comperand
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
			///<param name = "to">volatile зміна до якої додасться значення what</param>
			///<param name = "what">зміна яка додасться</param>
			///<returns>Нове значення змінної to</returns>
			__forceinline static Int32 InterlockedAdd(volatile Int32& to, Int32 what) {
				return _InterlockedAdd(reinterpret_cast<volatile long*>(&to), what);
			}

			///<summary>Атомарне додавання</summary>
			///<param name = "to">volatile зміна до якої додасться значення what</param>
			///<param name = "what">зміна яка додасться</param>
			///<returns>Нове значення змінної to</returns>
			__forceinline static UInt32 InterlockedAdd(volatile UInt32& to, UInt32 what) {
				return _InterlockedAdd(reinterpret_cast<volatile long*>(&to), what);
			}
			///<summary>Атомарне додавання</summary>
			///<param name = "to">volatile зміна до якої додасться значення what</param>
			///<param name = "what">зміна яка додасться</param>
			///<returns>Нове значення змінної to</returns>
			__forceinline static Int64 InterlockedAdd(volatile Int64& to, Int64 what) {
				return _InterlockedAdd64(reinterpret_cast<volatile __int64*>(&to), what);
			}

			///<summary>Атомарне додавання</summary>
			///<param name = "to">volatile зміна до якої додасться значення what</param>
			///<param name = "what">зміна яка додасться</param>
			///<returns>Нове значення змінної to</returns>
			__forceinline static UInt64 InterlockedAdd(volatile UInt64& to, UInt64 what) {
				return _InterlockedAdd64(reinterpret_cast<volatile __int64*>(&to), what);
			}
#pragma endregion
#pragma region Increment
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int16 Increment(volatile Int16& what) {
				return _InterlockedIncrement16(reinterpret_cast<volatile short*>(&what));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt16 Increment(volatile UInt16& what) {
				return _InterlockedIncrement16(reinterpret_cast<volatile short*>(&what));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int32 Increment(volatile Int32& what) {
				return _InterlockedIncrement(reinterpret_cast<volatile long*>(&what));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt32 Increment(volatile UInt32& what) {
				return _InterlockedIncrement(reinterpret_cast<volatile long*>(&what));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static Int64 Increment(volatile Int64& what) {
				return _InterlockedIncrement64(reinterpret_cast<volatile long long*>(&what));
			}
			///<summary>Атомарне інкрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно інкрементнути</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt64 Increment(volatile UInt64& what) {
				return _InterlockedIncrement64(reinterpret_cast<volatile long long*>(&what));
			}
#pragma endregion
#pragma region Decrement
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static Int16 Decrement(volatile Int16& what) {
				return _InterlockedDecrement16(reinterpret_cast<volatile short*>(&what));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt16 Decrement(volatile UInt16& what) {
				return _InterlockedDecrement16(reinterpret_cast<volatile short*>(&what));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static Int32 Decrement(volatile Int32& what) {
				return _InterlockedDecrement(reinterpret_cast<volatile long*>(&what));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt32 Decrement(volatile UInt32& what) {
				return _InterlockedDecrement(reinterpret_cast<volatile long*>(&what));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static Int64 Decrement(volatile Int64& what) {
				return _InterlockedDecrement64(reinterpret_cast<volatile long long*>(&what));
			}
			///<summary>Атомарне декрементування змінної</summary>
			///<param name = "what">Зміна яку потрібно декрементувати</param>
			///<returns>Нове значення</returns>
			__forceinline static UInt64 Decrement(volatile UInt64& what) {
				return _InterlockedDecrement64(reinterpret_cast<volatile long long*>(&what));
			}
#pragma endregion
#pragma region CompareAndSwap
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int16 CompareAndSwap(volatile Int16& destination, Int16 exchange, Int16 comperand) {
				return _InterlockedCompareExchange16(reinterpret_cast<volatile short*>(&destination), exchange, comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt16 CompareAndSwap(volatile UInt16& destination, UInt16 exchange, UInt16 comperand) {
				return _InterlockedCompareExchange16(reinterpret_cast<volatile short*>(&destination), exchange, comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int32 CompareAndSwap(volatile Int32& destination, Int32 exchange, Int32 comperand) {
				return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&destination), exchange, comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt32 CompareAndSwap(volatile UInt32& destination, UInt32 exchange, UInt32 comperand) {
				return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&destination), exchange, comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static Int64 CompareAndSwap(volatile Int64& destination, Int64 exchange, Int64 comperand) {
				return _InterlockedCompareExchange64(reinterpret_cast<volatile long long*>(&destination), exchange, comperand);
			}
			///<summary>Атомарне порівняння і обмін
			///<para> Порівнює destination з comperand і обмінює якщо рівне</para></summary>
			///<param name = "destination">Силка на змінну в яку запишеться результат вразі успіху</param>
			///<param name = "exchange">Нове значення</param>
			///<param name = "comperand">Старе значення</param>
			///<returns>Початкове значення</returns>
			__forceinline static UInt64 CompareAndSwap(volatile UInt64& destination, UInt64 exchange, UInt64 comperand) {
				return _InterlockedCompareExchange64(reinterpret_cast<volatile long long*>(&destination), exchange, comperand);
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