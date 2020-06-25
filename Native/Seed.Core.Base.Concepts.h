#pragma once
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.TypeTraits.h"

namespace Seed {
	inline namespace Core {
		namespace Concepts {
			template<typename TLambda, typename ... TArgs>
			concept Invockable = TypeTraits::IsInvockable<TLambda, TArgs...>;

			constexpr auto l = [] (int a, int b)
			{
				return a + b;
			};

			template<typename TLambda, typename ... TArgs> requires Invockable<TLambda, TArgs...>
			constexpr auto Invoke(TLambda lambda, TArgs... args ) {
				return lambda(args...);
			}

			constexpr auto r = Invoke(l, 1, 2);
		}
	}
}