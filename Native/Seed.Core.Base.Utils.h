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
				__forceinline constexpr operator bool() const { return value; }
				static constexpr bool State = value;
			};

			///<summary>Шаблон логічної константи, використовується для логічних суджень в compile-time</summary>
			template<typename value>
			struct LogicConstT
			{
				__forceinline constexpr operator bool() const { return value::State; }
				static constexpr bool State = value::State;
			};
			///<summary>Логічна істина для статичних перевірок</summary>
			inline namespace { typedef LogicConst<true> LogicTrue; }
			///<summary>Логічна хиба для статичних перевірок</summary>
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

			/// <summary>
			/// Додає LValue і RValue силку до типу
			/// </summary>
			template<class TType,
				class = void>
				struct AddReference
			{	// add reference
				using Lvalue = TType;
				using Rvalue = TType;
			};

            /// <summary>
            /// Додає LValue і RValue силку до типу
            /// </summary>
			template<class TType>
			struct AddReference<TType, VoidT<TType&>>
			{	// add reference
				using Lvalue = TType&;
				using Rvalue = TType&&;
			};

            /// <summary>
            /// Додає RValue силку
            /// </summary>
			template<class TType>
			using AddRvalueReferenceT = typename AddReference<TType>::Rvalue;

            /// <summary>
            /// Додає LValue силку
            /// </summary>
			template<class TType>
			using AddLvalueReferenceT = typename AddReference<TType>::Lvalue;

            /// <summary>
            /// Надає визначення типу в процесі інстанціювання шаблону
            /// </summary>
			template<class TType>
			AddRvalueReferenceT<TType> DeclVal() noexcept { return {}; };

            /// <summary>
            /// Перевірка чи заданий тип LValue силка
            /// </summary>
			template<typename TType>
			struct IsLValueRef : LogicFalse {};

            /// <summary>
            /// Перевірка чи заданий тип LValue силка
            /// </summary>
			template<typename Type>
			struct IsLValueRef<Type&> : LogicTrue {};
		}
		/// <summary>
		/// Функція переміщення об'єкта(перенесення rvalue)
		/// </summary>
		/// <typeparam name="T">Тип об'єкта(без rvalue)</typeparam>
		/// <param name="arg">lvalue-силка на rvalue об'єкт</param>
		/// <returns>rvalue силку на об'єкт</returns>
		template <typename T>
		__forceinline constexpr TypeTraits::RemoveRefV<T>&& MoveRValue(T&& arg) {
			return static_cast<TypeTraits::RemoveRefV<T>&&>(arg);
		}

		/// <summary>
		/// Ідеальна передача
		/// </summary>
		/// <typeparam name="TType">Тип об'єкта без rvalue</typeparam>
		/// <param name="arg">lvalue-силка на rvalue об'єкт</param>
		/// <returns>rvalue силку на об'єкт</returns>
		template<typename TType>
		[[nodiscard]]
		__forceinline constexpr TType&& ForwardReference(TypeTraits::RemoveRefV<TType>& arg) noexcept {	// forward an lvalue as either an lvalue or an rvalue
			return (static_cast<TType&&>(arg));
		}

		/// <summary>
		/// Ідеальна передача
		/// </summary>
		/// <typeparam name="TType">Тип об'єкта без rvalue</typeparam>
		/// <param name="arg">lvalue-силка на rvalue об'єкт</param>
		/// <returns>rvalue силку на об'єкт</returns>
		template<typename TType>
		[[nodiscard]]
		__forceinline constexpr TType&& ForwardReference(TypeTraits::RemoveRefV<TType>&& arg) noexcept {	// forward an rvalue as an rvalue
			static_assert(!TypeTraits::IsLValueRef<TType>(), "Bad forward call");
			return (static_cast<TType&&>(arg));
		}
	}
}
