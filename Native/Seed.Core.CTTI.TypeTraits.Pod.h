#pragma once
#include "Seed.Core.CTTI.TypeTraits.BaseTypes.h"

namespace Seed
{
	inline namespace Core
	{
        namespace CTTI
        {
            namespace TypeTraits
            {
                namespace Details
                {
                    template<
                        typename T, 
                        bool TIsConst, 
                        bool TIsVolatile,
                        bool TIsSerilizable
                    >
                    struct CTTIPodTypeDataImpl
                        : BaseCTTITypeDataImpl<
                        T,
                        EType::Pod,
                        TIsSerilizable,
                        TIsConst,
                        TIsVolatile,
                        false
                        > {};

                    template<typename T> requires Concepts::IsPodCttiType<T>
                    struct CTTITypeDataSpecialization<
                        T,
                        void
                    >
                        : CTTIPodTypeDataImpl<
                        typename Seed::TypeTraits::RemoveCV<T>,
                        Seed::TypeTraits::IsConstV<T>,
                        Seed::TypeTraits::IsVolatileV<T>,
                        true
                        > {};
                }
            }
        }
	}
}