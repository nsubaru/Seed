#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
#pragma region Signed type converter helpers
            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<typename>
            struct MakeSigned : LogicFalse {};

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int8> : LogicFalse {
                using Type = signed __int8;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int16> : LogicFalse {
                using Type = signed __int16;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int32> : LogicFalse {
                using Type = signed __int32;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<unsigned __int64> : LogicFalse {
                using Type = signed __int64;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int8> : LogicTrue {
                using Type = signed __int8;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int16> : LogicTrue {
                using Type = signed __int16;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int32> : LogicTrue {
                using Type = signed __int32;
            };

            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeSigned<signed __int64> : LogicTrue {
                using Type = signed __int64;
            };
#pragma endregion
            /// <summary>
            /// Вертає знаковий аналог типу числа
            /// </summary>
            template<typename TInt>
            using Signed = typename MakeSigned<TInt>::Type;

            /// <summary>
            /// Вертає чи тип знаковий
            /// </summary>
            template<typename TInt>
            constexpr auto IsSigned = MakeSigned<TInt>::State;

#pragma region Unsigned type converter helpers
            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<typename>
            struct MakeUnsigned : LogicFalse {};

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int8> : LogicFalse {
                using Type = unsigned __int8;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int16> : LogicFalse {
                using Type = unsigned __int16;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int32> : LogicFalse {
                using Type = unsigned __int32;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<signed __int64> : LogicFalse {
                using Type = unsigned __int64;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int8> : LogicTrue {
                using Type = unsigned __int8;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int16> : LogicTrue {
                using Type = unsigned __int16;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int32> : LogicTrue {
                using Type = unsigned __int32;
            };

            /// <summary>
            /// Вертає беззнаковий аналог типу числа
            /// </summary>
            template<>
            struct MakeUnsigned<unsigned __int64> : LogicTrue {
                using Type = unsigned __int64;
            };
#pragma endregion

            template<typename TInt>
            using Unsigned = typename MakeUnsigned<TInt>::Type;

            template<typename TInt>
            constexpr auto IsUnsigned = MakeUnsigned<TInt>::State;
		}
	}
}