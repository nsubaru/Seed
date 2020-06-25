#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Reference.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
#pragma region Remove CV
			/// <summary>
            /// Вертає тип без const модифікатора
            /// </summary>
            template<typename T>
            struct RemoveConstT
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип без const модифікатора
            /// </summary>
            template<typename T>
            struct RemoveConstT<const T>
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе const модифікатора
            /// </summary>
            template<typename T>
            using RemoveConst = typename RemoveConstT<T>::Type;

            /// <summary>
            /// Вертає тип без volatile модифікатора
            /// </summary>
            template<typename T>
            struct RemoveVolatileT
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип без volatile модифікатора
            /// </summary>
            template<typename T>
            struct RemoveVolatileT<volatile T>
            {
                using Type = T;
            };

            /// <summary>
            /// Вертає тип безе volatile модифікатора
            /// </summary>
            template<typename T>
            using RemoveVolatile = typename RemoveVolatileT<T>::Type;

            /// <summary>
            /// Вертає тип без cv модифікаторів
            /// </summary>
            template< class T >
            struct RemoveCVT
            {
                typedef typename RemoveVolatileT<typename RemoveConstT<T>::Type>::Type Type;
            };

            /// <summary>
            /// Вертає тип без cv модифікаторів
            /// </summary>
            template<typename T>
            using RemoveCV = typename RemoveCVT<T>::Type;

            /// <summary>
            /// Вертає тип без cv модифікаторів
            /// </summary>
            template<typename T>
            using RemoveCVRefV = RemoveCV<RemoveRefV<T>>;
#pragma endregion

#pragma region Add CV
            /// <summary>
            /// Додає const модифікатор до типу
            /// </summary>
            template <class TTy>
            struct AddConstT { 
                using Type = const TTy;
            };

            /// <summary>
            /// Додає const модифікатор до типу
            /// </summary>
            template <class TTy>
            using AddConstV = typename AddConstT<TTy>::Type;

            /// <summary>
            /// Додає volatile модифікатор до типу
            /// </summary>
            template <class TTy>
            struct AddVolatileT {
                using Type = volatile TTy;
            };

            /// <summary>
            /// Додає volatile модифікатор до типу
            /// </summary>
            template <class TTy>
            using AddVolatileV = typename AddVolatileT<TTy>::Type;

            /// <summary>
            /// Додає cv модифікатори до типу
            /// </summary>
            template <class TTy>
            struct AddCvT {
                using Type = const volatile TTy;
            };

            /// <summary>
            /// Додає cv модифікатори до типу
            /// </summary>
            template <class TTy>
            using AddCvV = typename AddCvT<TTy>::Type;
#pragma endregion

#pragma region Check CV
            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            struct IsConstT : LogicFalse
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            struct IsConstT<const T> : LogicTrue
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор const
            /// </summary>
            template<typename T>
            constexpr Boolean IsConstV = IsConstT<T>::State;

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            struct IsVolatileT : LogicFalse
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            struct IsVolatileT<volatile T> : LogicTrue
            {};

            /// <summary>
            /// Вертає чи тип має модифікатор volatile
            /// </summary>
            template<typename T>
            constexpr Boolean IsVolatileV = IsVolatileT<T>::State;
#pragma endregion
		}
	}
}