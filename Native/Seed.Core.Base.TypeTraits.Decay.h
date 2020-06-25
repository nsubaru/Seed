#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Pointer.h"
#include "Seed.Core.Base.TypeTraits.CVModificators.h"
#include "Seed.Core.Base.TypeTraits.LogicOperations.h"
#include "Seed.Core.Base.TypeTraits.Function.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
            /// <summary>
            /// Агресивне зняття всіх модифікаторів з тиів
            /// </summary>
			template< class T >
            struct DecayStruct
            {
            private:
                typedef RemoveRefV<T> U;
            public:
                typedef ConditionV <
                    IsCArray<U>::State,
                    typename RemoveSizeFromCArray<U>::type*,
                    ConditionV<
                    IsFunction<U>::State,
                    AddPointer<U>,
                    RemoveCV<U>
                    >
                > Type;
            };

            /// <summary>
            /// Агресивне зняття всіх модифікаторів з тиів
            /// </summary>
            template<class T>
            using DecayV = typename DecayStruct<T>::Type;
		}
	}
}