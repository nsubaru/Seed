#pragma once
#include "Seed.Core.CTTI.TypeTraits.Base.h"

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
                        EType::POD,
                        TIsSerilizable,
                        TIsConst,
                        TIsVolatile,
                        false
                        > {};

                    template<typename T>
                    concept IsSerelizableObject = Core::Concepts::IsBasedOn<ISerilizableBase, T>;

                    template<typename T>
                    concept IsSerelizableStruct = requires { T::FieldMetaDataGetter(); };

                    template<typename T>
                    constexpr bool IsCttiPod =
                        !IsSerelizableObject<T> &&
                        !IsSerelizableStruct<T> &&
                        Core::TypeTraits::IsClassOrStructV<T> &&
                        Core::TypeTraits::IsPodV<T>;

                    template<typename T>
                    struct CTTITypeDataSpecialization<
                        T,
                        typename Seed::Core::TypeTraits::EnableIfV<IsCttiPod<T>, void>
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