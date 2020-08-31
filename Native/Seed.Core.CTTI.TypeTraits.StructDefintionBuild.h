#pragma once
#include "Seed.Core.CTTI.TypeTraits.BaseTypes.h"
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

                    template<typename T> requires Concepts::IsStructCttiType<T>
                    struct CTTITypeDataSpecialization<
                        T,
                        void
                    > : CTTIStructTypeDataImpl<
                        typename Seed::TypeTraits::RemoveCV<T>,
                        Seed::TypeTraits::IsConstV<T>,
                        Seed::TypeTraits::IsVolatileV<T>,
                        true,
                        true
                        >  {
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