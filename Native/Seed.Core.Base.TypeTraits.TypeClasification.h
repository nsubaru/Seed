#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.NumericConverter.h"
#include "Seed.Core.Base.TypeTraits.LogicOperations.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
			/// <summary>
            /// Вертає true якщо тип POD
            /// </summary>
            template<typename T>
            struct IsPodT : LogicConst<__is_pod(T)> {};

            /// <summary>
            /// Вертає true якщо тип POD
            /// </summary>
            template<typename T>
            constexpr bool IsPodV = IsPodT<T>::State;

            /// <summary>
            /// Вертає true якщо тип цілочиселльний
            /// </summary>
            template<typename T>
            constexpr bool IsIntegerNumber =
                TypeTraits::IsSigned<T> ||
                TypeTraits::IsUnsigned<T>;

            /// <summary>
            /// Вертає true якщо тип дійсних чисел
            /// </summary>
            template<typename T>
            constexpr bool IsFloatPointNumber = 
                TypeTraits::IsSameT<T, Float>::State ||
                TypeTraits::IsSameT<T, Double>::State;

            /// <summary>
            /// Вертає true якщо тип числовий
            /// </summary>
            template<typename T>
            constexpr bool IsNumeric =
                IsIntegerNumber<T> ||
                IsFloatPointNumber<T>;

#pragma warning(push)
            //VS bug char and wchar_t trait as equal
#pragma warning(disable: 6287)
            /// <summary>
            /// Вертає чи тип символьний
            /// </summary>
            template<typename T>
            constexpr bool IsCharacter =
                TypeTraits::IsSameT<T, AChar>() ||
                TypeTraits::IsSameT<T, UChar>() ||
                TypeTraits::IsSameT<T, U8Char>() ||
                TypeTraits::IsSameT<T, U16Char>() ||
                TypeTraits::IsSameT<T, U32Char>();
#pragma warning(pop)

            /// <summary>
            /// Вертає чи тип символьний
            /// </summary>
            template<typename T>
            constexpr bool IsBoolean = TypeTraits::IsSameT<T, bool>();

            /// <summary>
            /// Вертає true якщо тип є переліченням
            /// </summary>
            template<typename T>
            struct IsEnumT : LogicConst<__is_enum(T)> {};

            /// <summary>
            /// Вертає true якщо тип є переліченням
            /// </summary>
            template<typename T>
            constexpr bool IsEnumV = __is_enum(T);

            /// <summary>
            /// Вертає числовий тип перелічення
            /// </summary>
            template<typename T>
            struct EnumUnderlyingTypeT {
                using Type = __underlying_type(T);
            };

            /// <summary>
            /// Вертає числовий тип перелічення
            /// </summary>
            template<typename T>
            using EnumUnderlyingTypeV = __underlying_type(T);

            /// <summary>
            /// Вертає true якщо тип не містить не статичних полів
            /// </summary>
            template <class TTy>
            struct IsEmptyT : LogicConst<__is_empty(TTy)> {};

            /// <summary>
            /// Вертає true якщо тип не містить не статичних полів
            /// </summary>
            template <class TTy>
            inline constexpr bool IsEmptyV = __is_empty(TTy);

            /// <summary>
            /// Вертає true якщо клас позначений модифікатор final
            /// </summary>
            template <class TTy>
            struct IsFinalT : LogicConst<__is_final(TTy)> {};

            /// <summary>
            /// Вертає true якщо клас позначений модифікатор final
            /// </summary>
            template <class TTy>
            constexpr bool IsFinalV = __is_final(TTy);   

            /// <summary>
            /// Вертає true якщо це клас або структура
            /// </summary>
            template<typename T>
            constexpr bool IsClassOrStructV = __is_class(T);
		}
	}
}