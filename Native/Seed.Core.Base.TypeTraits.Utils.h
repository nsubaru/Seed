#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Reference.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
			/// <summary>
			/// Створення уявної змінної, необхідно для патерн матчінгу по типам
			/// </summary>
            template<class TTy>
            AddRvalueReferenceV<TTy> Declval() noexcept { return {}; }
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
		__forceinline constexpr TType&& ForwardReference(typename TypeTraits::RemoveRefV<TType>& arg) noexcept {	// forward an lvalue as either an lvalue or an rvalue
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
		__forceinline constexpr TType&& ForwardReference(typename TypeTraits::RemoveRefV<TType>&& arg) noexcept {	// forward an rvalue as an rvalue
			static_assert(!TypeTraits::IsLValueReferenceV<TType>(), "Bad forward call");
			return (static_cast<TType&&>(arg));
		}
	}
}