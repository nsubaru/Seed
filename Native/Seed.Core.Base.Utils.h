#pragma once
namespace Seed
{
	inline namespace Core
	{
		namespace StaticAsserts
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
			///<summary>Логічна істина для статичних провірок</summary>
			inline namespace { typedef LogicConst<True> LogicTrue; }
			///<summary>Логічна хиба для статичних провірок</summary>
			inline namespace { typedef LogicConst<False> LogicFalse; }

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRef { using Type = T; };

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRef<T&> { using Type = T; };

			///<summary>Знімає rv-силку з типу</summary>
			template<typename T>
			struct RemoveRef<T&&> { using Type = T; };

			///<summary>Тип без силки</summary>
			template<typename T>
			using RemoveRefT = typename RemoveRef<T>::Type;

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
				using Lvalue = Type & ;
				using Rvalue = Type && ;
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
		constexpr typename StaticAsserts::RemoveRefT<T>&& MoveRValue(T&& arg) {
			return static_cast<typename StaticAsserts::RemoveRefT<T>&&>(arg);
		}

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		constexpr Type&& ForwardReference(StaticAsserts::RemoveRefT<Type>& Arg) noexcept {	// forward an lvalue as either an lvalue or an rvalue
			return (static_cast<Type&&>(Arg));
		}

		///<summary>Ідеальна передача</summary>
		///<param name = 'Arg'>Аргумент</param>
		///<returns>RV-силку</returns>
		template<typename Type>
		[[nodiscard]]
		constexpr Type&& ForwardReference(StaticAsserts::RemoveRefT<Type>&& Arg) noexcept {	// forward an rvalue as an rvalue
			static_assert(!StaticAsserts::IsLValueRef<Type>(), "Bad forward call");
			return (static_cast<Type&&>(_Arg));
		}
	}
}