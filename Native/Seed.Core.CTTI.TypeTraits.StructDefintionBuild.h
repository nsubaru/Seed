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

                    template<typename T> requires requires { T::FieldMetaDataGetter(); }
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
                        static constexpr auto FieldMetaData = T::FieldMetaDataGetter();
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

#define StructCttiDef(StructName, ...)\
		ForceInline constexpr static auto FieldMetaDataGetter()\
		{\
			auto FieldMetaData = DefStructMetaDataObj(StructName,__VA_ARGS__);\
			return FieldMetaData;\
		}