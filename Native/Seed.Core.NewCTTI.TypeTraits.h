#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.NewCTTI.TypeTraits.Primitives.h"

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
			}
		}
	}
}