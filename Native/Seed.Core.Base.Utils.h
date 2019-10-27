#pragma once
namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
			template<class... _Types>
			using VoidT = void;

			///<summary>Шаблон логічної константи, використовується для логічних суджень в compile-time</summary>
			template<bool value>
			struct LogicConst
			{
				__forceinline constexpr operator bool() { return value; }
				static constexpr bool State = value;
			};

			///<summary>Шаблон логічної константи, використовується для логічних суджень в compile-time</summary>
			template<typename value>
			struct LogicConstT
			{
				__forceinline constexpr operator bool() { return value::State; }
				static constexpr bool State = value::State;
			};
			///<summary>Логічна істина для статичних провірок</summary>
			inline namespace { typedef LogicConst<true> LogicTrue; }
			///<summary>Логічна хиба для статичних провірок</summary>
			inline namespace { typedef LogicConst<false> LogicFalse; }

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRefT { using Type = T; };

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRefT<T&> { using Type = T; };

			///<summary>Знімає rv-силку з типу</summary>
			template<typename T>
			struct RemoveRefT<T&&> { using Type = T; };

			///<summary>Тип без силки</summary>
			template<typename T>
			using RemoveRefV = typename RemoveRefT<T>::Type;

			// STRUCT TEMPLATE _Add_reference
			template<class Type,
				class = void>
				struct AddReference
			{	// add reference
				using Lvalue = Type;
				using Rvalue = Type;
			};

			template<class Type>
			struct AddReference<Type, VoidT<Type&>>
			{	// add reference
				using Lvalue = Type&;
				using Rvalue = Type&&;
			};

			template<class Type>
			using AddRvalueReferenceT = typename AddReference<Type>::Rvalue;

			template<class Type>
			using AddLvalueReferenceT = typename AddReference<Type>::Lvalue;

			template<class Type>
			AddRvalueReferenceT<Type> DeclVal() noexcept {};

			template<typename Type>
			struct IsLValueRef : LogicFalse {};

			template<typename Type>
			struct IsLValueRef<Type&> : LogicTrue {};
		}
		///<summary>Функція переміщення об'єкта(перенесення rvalue)</summary>
		///<param name="Obj"> lvalue-силка на об'єкт</param>
		///<returns>rvalue силку на об'єкт</returns>
		template <typename T>
		__forceinline constexpr typename TypeTraits::RemoveRefV<T>&& MoveRValue(T&& arg) {
			return static_cast<typename TypeTraits::RemoveRefV<T>&&>(arg);
		}

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		__forceinline constexpr Type&& ForwardReference(TypeTraits::RemoveRefV<Type>& Arg) noexcept {	// forward an lvalue as either an lvalue or an rvalue
			return (static_cast<Type&&>(Arg));
		}

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		__forceinline constexpr Type&& ForwardReference(TypeTraits::RemoveRefV<Type>&& Arg) noexcept {	// forward an rvalue as an rvalue
			static_assert(!TypeTraits::IsLValueRef<Type>(), "Bad forward call");
			return (static_cast<Type&&>(Arg));
		}
	}
}