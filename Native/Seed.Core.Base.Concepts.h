#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.TypeTraits.h"

namespace Seed {
	inline namespace Core {
		namespace Concepts {
			/// <summary>
			/// Перевірка чи тип TCallable можна викликати з набіром аргументів TArgs
			/// </summary>
			template<typename TCallable, typename ... TArgs>
			concept IsInvockable = TypeTraits::IsInvockable<TCallable, TArgs...>;

			/// <summary>
			/// Перевірка чи тип TCallable можна викликати з набіром аргументів TArgs
			/// і вертає заданий тип TRet
			/// </summary>
			template<typename TCallable, typename TRet, typename ... TArgs>
			concept IsInvockableWithResult = TypeTraits::IsInvockableWithResult<TCallable, TRet, TArgs...>;

			template<typename TCallable, typename TRet, typename ... TArgs>
			concept IsInvockableReturnSameAs = TypeTraits::IsSameV<typename TypeTraits::InvockableResult<TCallable, TArgs...>, TRet>;

			template<typename TCallable, typename ... TArgs>
			concept IsInvockableReturnVoid = TypeTraits::IsVoidV<typename TypeTraits::InvockableResult<TCallable, TArgs...>>;

			/// <summary>
			/// Перевірка чи тип U наслідується від типу T
			/// </summary>
			/// <typeparam name="TBase">Базовий класс(інтерфейс)</typeparam>
			/// <typeparam name="TDerived">Дочірній класс(реалізація)</typeparam>
			template<typename TBase, typename TDerived>
			concept IsBasedOn = TypeTraits::IsBasedOnV<TBase, TDerived>;

			/// <summary>
			/// Перевірка чи тип T і U є одним і тим самим типом
			/// </summary>
			template<typename T, typename U>
			concept IsSame = TypeTraits::IsSameV<T, U>;

			template<typename T, typename U>
			concept IsConvertible = TypeTraits::IsConvertible<T, U>;

			template<typename T>
			concept IsPod = TypeTraits::IsPodV<T>;

			template<typename T, typename ... TArgs>
			concept IsConstructibleWith = TypeTraits::IsConstructible<T, TArgs...>;

			template<typename T>
			concept IsDefaultConstructible = TypeTraits::IsConstructible<T>;

			template<typename T, typename U>
			concept IsComparableWith = requires(T x, U y) {
				{ x == y } -> IsSame<bool>;
				{ x != y } -> IsSame<bool>;
				{ x <= y } -> IsSame<bool>;
				{ x >= y } -> IsSame<bool>;

				{ x < y } -> IsSame<bool>;
				{ x > y } -> IsSame<bool>;
			};

			template<typename T, typename U>
			concept HaveEqual = requires(T x, U y) {{ x == y } -> IsSame<bool>; };

			template<typename T, typename U>
			concept HaveNotEqual = requires(T x, U y) {{ x != y } -> IsSame<bool>; };
		}
	}
}