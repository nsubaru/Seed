#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.NewCTTI.TypeTraits.Primitives.h"
#include "Seed.Core.NewCTTI.TypeTraits.String.h"
#include "Seed.Core.NewCTTI.TypeTraits.Collections.h"

namespace Seed
{
    inline namespace Core
    {
        namespace NewCTTI
        {
            namespace TypeTraits
            {
                constexpr auto TypeClass = CTTITypeMetaData<Float>::TypeClass;
                constexpr auto TypeMax = CTTITypeMetaData<Float>::Epsilon;
                constexpr auto TypeName = CTTITypeMetaData<Float>::TypeName;
                constexpr auto Name = CTTITypeMetaData<Containers::Map<Int32, Containers::List<String>>>::TypeName;
                constexpr auto data = CTTITypeMetaData<Containers::Map<Int32, Containers::List<String>>>::PairTypeMetaData::TypeName;
            }
        }
    }
}