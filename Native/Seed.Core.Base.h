#pragma once
#include "interlocked.h"

#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Math.h"
#include "Seed.Core.Base.BitOperations.h"
#include "Seed.Core.Base.CStr.h"
#include "Seed.Core.Base.Swap.h"
#include "Seed.Core.Base.StaticAsserts.h"

///<summary>Якщо визначено SeedKernel то всі виклики 
///позначенні SeedKernelCall будуть експортуватись з виконавчого файла
///інакше будуть імпортуватись</summary>
#ifdef SeedKernel
///<summary>Системний виклик який експортується з ядра Рем</summary>
#define SeedKernelCall __declspec(dllexport)

typedef void(__cdecl *PF)(void);

extern int __cdecl Internal::DestroyStaticVariables(PF pf);

#pragma warning(push)
#pragma warning(disable: 4075)
#pragma init_seg("INIT$b", DestroyStaticVariables)  
#define INIT
#pragma warning(pop)

#else
///<summary>Системний виклик який імпортується з ядра Рем</summary>
#define SeedKernelCall __declspec(dllimport)
#endif
///<summary>Не рекомендована функція</summary>
///<param name = 'Why'>Причина</param>
#define DeprecatedCall(Why) __declspec(deprecated(Why))
///<summary>Внутрішній апі, використовувати напряму заборонено</summary>
#define InternalApiCall __declspec(deprecated("Internal API"))

///<summary>Якщо <code>RefCountClass</code> вставлено в тіло класу в .h файлі то даних клас автоматично отримує підрахунок силок</summary>
///<param name = 'ClassName'>назва класу</param> 
///<param name = 'This'>покажчик в який поміщено дані екземаляра, має містити зміну RefCount типу RefCountT</param> 
///<param name = 'OnDestroy'>код(функція) який викличеться в деструкторі при знищені останього екземпляра
///<para>параметр не обов'язковий</para>
///</param> 
///<remarks>
///Дефайн додає конструктор та оператор копіювання і переміщення та деструктор
///</remarks>
#define RefCountClass(ClassName, This, OnDestroy) \
ClassName(const ClassName & Orig) :This(Orig.This) { ++This->RefCount; }; \
ClassName(ClassName && From) : This(From.This) { From.This = nullptr; }; \
ClassName & operator=(const ClassName & Orig){this->~ClassName(); this->ClassName::ClassName(Orig); return *this; }; \
ClassName & operator=(ClassName && From){if(this == &From) return From; this->~ClassName(); this->ClassName::ClassName(std::move(From)); return *this; }; \
~ClassName(){if (This != nullptr) {if (--This->RefCount == 0) {OnDestroy;delete This;}This = nullptr;}}

///<summary>Головний простір імен</summary>
namespace Seed {
	///<summary>Простір імен функцій ядра Рем</summary>
	inline namespace Core {
		///<summary>Атомарний лічильник<para>Використовує функції InterlockedXXX</para></summary>
		class SeedKernelCall RefCountT {
		private:
			///<summary>Містить значення</summary>
			volatile SizeT Count;
		public:
			///<summary>Конструктор з <code>SizeT</code></summary>
			///<param name='Count'>Початкове значення</param>
			__forceinline RefCountT(SizeT Count = 0) noexcept :
				Count(Count) {};

			///<summary>Префіксний інкремент
			///<para>Використовує функцію <code>InterlockedIncrementSizeTNoFence</code> для інкременту</para>
			///</summary>
			///<returns>Вертає силку на елемент після інкременту</returns>
			__forceinline RefCountT & operator++() {
				InterlockedIncrementSizeTNoFence(&this->Count);
				return *this;
			}

			///<summary>Постфіксний інкремент
			///<para>Використовує функцію <code>InterlockedIncrementSizeTNoFence</code> для інкременту</para>
			///</summary>
			///<returns>Вертає копію об'єкта до інкремента</returns>
			__forceinline RefCountT operator++(int) {
				InterlockedIncrementSizeTNoFence(&this->Count);
				return this->Count;
			}

			///<summary>Префіксний декремент
			///<para>Використовує функцію <code>InterlockedDecrementSizeTNoFence</code> для декременту</para>
			///</summary>
			///<returns>Вертає силку на елемент після декременту</returns>
			__forceinline RefCountT & operator--() {
				InterlockedDecrementSizeTNoFence(&this->Count);
				return *this;
			}

			///<summary>Постфіксний декремент
			///<para>Використовує функцію <code>InterlockedDecrementSizeTNoFence</code> для декременту</para>
			///</summary>
			///<returns>Вертає копію об'єкта до декременту</returns>
			__forceinline RefCountT operator--(int) {
				InterlockedDecrementSizeTNoFence(&this->Count);
				return this->Count;
			}

			///<summary>Вертає значення в вигляді <code>SizeT<code> змінної</summary>
			///<returns>Поточне значення об'єкта</returns>
			__forceinline operator SizeT() const { return this->Count; }
		};

		///<summary>Змінює порядок елементів в масиву Array</summary>
		///<param name='Array'>Масив</param>
		///<param name='Size'>Кількість елементів</param>
		template<typename T>
		__forceinline void Reverse(T * Array, SizeT size) {
			for (size_t i = 0; i < size / 2; i++) {
				Swap(Array[i], Array[(size - 1) - i]);
			}
		}

		///<summary>Функція переміщення об'єкта(перенесення rvalue)</summary>
		///<param name="Obj"> lvalue-силка на об'єкт</param>
		///<returns>rvalue силку на об'єкт</returns>
		template<typename T>
		__forceinline T&& MoveRValue(T& Obj) {
			return CRT::move(Obj);
		}
	}
}