#pragma once
#include "Seed.Core.CTTI.TypeTraits.Base.h"
#include "Seed.Core.CTTI.TypeTraits.Collections.h"
#include "Seed.Core.CTTI.TypeTraits.Struct.h"

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
                        bool TIsSerilizable,
                        bool TIsConvertibleToTuple
                    >
                    struct CTTIStructTypeDataImpl
                        : BaseCTTITypeDataImpl<
                        T,
                        EType::Struct,
                        TIsSerilizable,
                        TIsConst,
                        TIsVolatile,
                        TIsConvertibleToTuple
                        > {};

                    template<typename T>
                    struct CTTITypeDataSpecialization<
                        T,
                        typename Seed::Core::TypeTraits::EnableIfV<Seed::Core::TypeTraits::LogicConst<T::FieldMetaDataGetter != nullptr>::State, void>
                    >
                        : CTTIStructTypeDataImpl<
                        typename Seed::TypeTraits::RemoveCV<T>,
                        Seed::TypeTraits::IsConstV<T>,
                        Seed::TypeTraits::IsVolatileV<T>,
                        T::FieldMetaDataGetter().IsSerilizable,
                        true
                        > {
                        using FieldMetaData = decltype(T::FieldMetaDataGetter());
                        using Fields = typename FieldMetaData::Fields;
                        
                        static constexpr decltype(auto) ToTuple(const T& data) {
                            return FieldMetaData::ToTuple(data);
                        }
                    };
                }
            }
        }
    }
}

#define StructCttiDef(StructName, ...)\
		ForceInline constexpr static auto FieldMetaDataGetter()\
		{\
			DefStructMetaData(StructName,__VA_ARGS__);\
			return FieldMetaData{};\
		}