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
			concept IsInvockableWithResult = TypeTraits::IsInvockable<TCallable, TArgs...> &&
				TypeTraits::IsConvertible<typename TypeTraits::InvockableResult<TCallable, TArgs...>, TRet>;

			/// <summary>
			/// Перевірка чи тип T наслідується від типу U
			/// </summary>
			template<typename T, typename U>
			concept IsBasedOn = TypeTraits::IsBasedOnV<T, U>;

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
		}
	}
}