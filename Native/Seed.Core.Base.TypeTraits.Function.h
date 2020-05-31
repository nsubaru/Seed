#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
			///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename>
            struct IsFunction : LogicFalse { };

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)> : LogicTrue {};

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)> : LogicTrue {};

            ///<summary>Compile-time перевірка чи даний тип є функцією</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile> : LogicTrue {};

            ///<summary>specialization for function types that have ref-qualifiers</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...)&& > : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...)&& > : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile&&> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile&&> : LogicTrue {};

            ///<summary>specializations for noexcept versions of all the above (C++17 and later)</summary>
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) & noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) & noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) && noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const&& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) volatile&& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs...) const volatile&& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) && noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const&& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) volatile&& noexcept> : LogicTrue {};
            template<typename TRet, typename... TArgs>
            struct IsFunction<TRet(TArgs..., ...) const volatile&& noexcept> : LogicTrue {};
		}
	}
}