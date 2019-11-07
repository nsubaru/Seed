#pragma once
#include "Seed.Core.NewCTTI.TypeTraits.Base.h"
#include "Seed.Core.NewCTTI.TypeTraits.Primitives.h"
#include "Seed.Core.NewCTTI.TypeTraits.HeapMetaData.h"

namespace Seed
{
	inline namespace Core
	{
		namespace NewCTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
					template<typename THeap, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<BaseString<THeap>, TIsConst, TIsVolatile> :
						BaseHeapCTTITypeData<BaseString<THeap>, THeap, EType::String, true, TIsConst, TIsVolatile>
					{
						using Character = UChar;
						using CharacterTypeMetaData = CTTITypeMetaData<UChar>;

						static constexpr auto CodePage = CharacterTypeMetaData::CodePage;
					};
				}
			}
		}
	}
}