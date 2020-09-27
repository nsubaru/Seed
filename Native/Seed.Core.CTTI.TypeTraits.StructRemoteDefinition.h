#pragma once
#include "Seed.Core.CTTI.TypeTraits.StructDefintionBuild.h"

namespace Seed {
	inline namespace Core {
		namespace CTTI {
			namespace TypeTraits
			{
				namespace Details
				{
                    template<typename T> requires requires { StructCttiShema<T>::FieldMetaDataGetter(); }
                        struct CTTITypeDataSpecialization<
                            T,
                            void
                        > : CTTIStructTypeDataImpl<
                            typename Seed::TypeTraits::RemoveCV<T>,
                            Seed::TypeTraits::IsConstV<T>,
                            Seed::TypeTraits::IsVolatileV<T>,
                            true,
                            true
                        > {
                            static constexpr auto FieldMetaData = StructCttiShema<T>::FieldMetaDataGetter();
                            static constexpr auto Fields = typename FieldMetaData.DataFields;

                            ForceInline static constexpr decltype(auto) ToTuple(const T& data) {
                                return FieldMetaData.ToTuple(data);
                            }
                        };
				}
			}
		}
	}
}