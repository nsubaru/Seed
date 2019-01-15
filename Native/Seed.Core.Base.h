#pragma once

#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Math.h"
#include "Seed.Core.Base.BitOperations.h"
#include "Seed.Core.Base.CStr.h"
#include "Seed.Core.Base.Swap.h"
#include "Seed.Core.Base.StaticAsserts.h"
#include "Seed.Core.Base.UUID.h"

///<summary>Якщо визначено SeedKernel то всі виклики 
///позначенні SeedKernelCall будуть експортуватись з виконавчого файла
///інакше будуть імпортуватись</summary>
#ifdef SeedKernel
///<summary>Системний виклик який експортується з ядра Рем</summary>
#define SeedKernelCall __declspec(dllexport)
///<summary>Сигнатура статичного конструктор зміної</summary>
namespace { typedef void(__cdecl *PF)(void); }

///<summary>Сигнатура декструктора статичних зміних</summary>
extern int __cdecl DestroyStaticVariables(PF pf);

#pragma warning(push)
#pragma warning(disable: 4075)
#pragma init_seg("INIT$b", DestroyStaticVariables)  
#define INIT
#pragma warning(pop)
///<summary>Експортує але в межах ядра замість виклику підставляє тіло функції</summary>
///<param name = 'FuncSig'>Сигнатура функції</param>
///<param name = 'Body'>Тіло функції</param>
#define OptimizeExport(FunctSig, Body) SeedKernelCall __forceinline FunctSig Body

#else
///<summary>Системний виклик який імпортується з ядра Рем</summary>
#define SeedKernelCall __declspec(dllimport)
///<summary>Імпортує як явний виклик</summary>
///<param name = 'FuncSig'>Сигнатура функції</param>
///<param name = 'Body'>Тіло функції</param>
#define OptimizeExport(FunctSig, Body) SeedKernelCall FunctSig;
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
__forceinline ClassName(const ClassName & Orig) :This(Orig.This) { ++This->RefCount; }; \
__forceinline ClassName(ClassName && From) : This(From.This) { From.This = nullptr; }; \
__forceinline ClassName & operator=(const ClassName & Orig){this->~ClassName(); this->ClassName::ClassName(Orig); return *this; }; \
__forceinline ClassName & operator=(ClassName && From){if(this == &From) return From; this->~ClassName(); this->ClassName::ClassName(Seed::MoveRValue(From)); return *this; }; \
__forceinline ~ClassName(){if (This != nullptr) {if (--This->RefCount == 0) {OnDestroy;delete This;}This = nullptr;}};

///<summary>Властивість яка можна лиш читати</summary>
#define __Property1(Getter) __declspec( property( get=Getter) ) 
///<summary>Властивість яку можна і читати і записувати</summary>
#define __Property2(Getter, Setter) __declspec( property( get=Getter, put=Setter ) )  

#define __Property_GET_MACRO(_1,_2,NAME,...) NAME
///<summary>Властивість яку можна читати або читати якщо дано лиш гетер і записувати якщо є гетер і сетер</summary>
///<param name = '__VA_ARGS__'>Гетер або гетер і сетер</param>
#define Property(...) __Property_GET_MACRO(__VA_ARGS__, __Property2, __Property1)(__VA_ARGS__)

///<summary>Головний простір імен</summary>
namespace Seed {
	///<summary>Простір імен функцій ядра Рем</summary>
	inline namespace Core {
		///<summary>Атомарний лічильник<para>Використовує функції InterlockedXXX</para></summary>
		class SeedKernelCall RefCountT {
		private:
			///<summary>Містить значення</summary>
			volatile SizeT count;
		protected:
		public:
			///<summary>Гетер</summary>
			///<returns>Значення лічильника</returns>
			SizeT GetValue() { return count; }
			///<summary>Поточне значення лічильника</summary>
			///<value>Значення лічильника</value>
			Property(GetValue) SizeT Count;
			///<summary>Конструктор з <code>SizeT</code></summary>
			///<param name='Count'>Початкове значення</param>
			__forceinline RefCountT(SizeT Count = 0) noexcept :
				count(Count) {};

			///<summary>Префіксний інкремент
			///<para>Використовує функцію <code>InterlockedIncrementSizeTNoFence</code> для інкременту</para>
			///</summary>
			///<returns>Вертає силку на елемент після інкременту</returns>
			__forceinline RefCountT & operator++() {
				this->count++;
				return *this;
			}

			///<summary>Постфіксний інкремент
			///<para>Використовує функцію <code>InterlockedIncrementSizeTNoFence</code> для інкременту</para>
			///</summary>
			///<returns>Вертає копію об'єкта до інкремента</returns>
			__forceinline RefCountT operator++(int) {
				auto cp = *this;
				++this->count;
				return cp;
			}

			///<summary>Префіксний декремент
			///<para>Використовує функцію <code>InterlockedDecrementSizeTNoFence</code> для декременту</para>
			///</summary>
			///<returns>Вертає силку на елемент після декременту</returns>
			__forceinline RefCountT & operator--() {
				this->count--;
				return *this;
			}

			///<summary>Постфіксний декремент
			///<para>Використовує функцію <code>InterlockedDecrementSizeTNoFence</code> для декременту</para>
			///</summary>
			///<returns>Вертає копію об'єкта до декременту</returns>
			__forceinline RefCountT operator--(int) {
				auto cp = *this;
				this->count--;
				return cp;
			}

			///<summary>Вертає значення в вигляді <code>SizeT<code> змінної</summary>
			///<returns>Поточне значення об'єкта</returns>
			__forceinline operator SizeT() const { return this->count; }
		};

		///<summary>Змінює порядок елементів в масиву Array</summary>
		///<param name='Array'>Масив</param>
		///<param name='Size'>Кількість елементів</param>
		template<typename T>
		DeprecatedCall("Робота з с-подібнимим масивами") __forceinline void Reverse(T * Array, SizeT size) {
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

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		constexpr Type&& ForwardReference(StaticAsserts::RemoveRefT<Type>& Arg) noexcept
		{	// forward an lvalue as either an lvalue or an rvalue
			return (static_cast<Type&&>(Arg));
		}

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		constexpr Type&& ForwardReference(StaticAsserts::RemoveRefT<Type>&& Arg) noexcept
		{	// forward an rvalue as an rvalue
			static_assert(!StaticAsserts::IsLValueRef<Type>(), "Bad forward call");
			return (static_cast<Type&&>(_Arg));
		}
	}
}

namespace {
	template <class F, class... Args>
	inline auto INVOKE(F&& f, Args&&... args) ->
		decltype(Seed::ForwardReference<F>(f)(Seed::ForwardReference<Args>(args)...)) {
		return Seed::ForwardReference<F>(f)(Seed::ForwardReference<Args>(args)...);
	}

	template <class Base, class T, class Derived>
	inline auto INVOKE(T Base::*pmd, Derived&& ref) ->
		decltype(Seed::ForwardReference<Derived>(ref).*pmd) {
		return Seed::ForwardReference<Derived>(ref).*pmd;
	}

	template <class PMD, class Pointer>
	inline auto INVOKE(PMD&& pmd, Pointer&& ptr) ->
		decltype((*Seed::ForwardReference<Pointer>(ptr)).*Seed::ForwardReference<PMD>(pmd)) {
		return (*Seed::ForwardReference<Pointer>(ptr)).*Seed::ForwardReference<PMD>(pmd);
	}

	template <class Base, class T, class Derived, class... Args>
	inline auto INVOKE(T Base::*pmf, Derived&& ref, Args&&... args) ->
		decltype((Seed::ForwardReference<Derived>(ref).*pmf)(Seed::ForwardReference<Args>(args)...)) {
		return (Seed::ForwardReference<Derived>(ref).*pmf)(Seed::ForwardReference<Args>(args)...);
	}

	template <class PMF, class Pointer, class... Args>
	inline auto INVOKE(PMF&& pmf, Pointer&& ptr, Args&&... args) ->
		decltype(((*Seed::ForwardReference<Pointer>(ptr)).*Seed::ForwardReference<PMF>(pmf))(Seed::ForwardReference<Args>(args)...)) {
		return ((*Seed::ForwardReference<Pointer>(ptr)).*Seed::ForwardReference<PMF>(pmf))(Seed::ForwardReference<Args>(args)...);
	}
}

namespace Seed {
	inline namespace Core {
		namespace StaticAsserts {
			template <typename, typename = void>
			struct ResultOf {
				using Type = VoidT<>;
			};
			template <typename F, typename...Args>
			struct ResultOf<F(Args...),
				decltype(void(::INVOKE(StaticAsserts::DeclVal<F>(), StaticAsserts::DeclVal<Args>()...)))> {
				using Type = decltype(::INVOKE(StaticAsserts::DeclVal<F>(), StaticAsserts::DeclVal<Args>()...));
			};

			///<summary>Тип без силки</summary>
			template<typename T>
			using ResultOfT = typename ResultOf<T>::Type;
		}
	}
}