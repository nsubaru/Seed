#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"
#include "Seed.Core.Base.TypeTraits.Reference.h"
#include "Seed.Core.Base.TypeTraits.Function.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
            /// <summary>
            /// Вертає true якщо тип є покажчиком
            /// </summary>
            template<typename T>
            struct IsPointerT : LogicFalse {};

            /// <summary>
            /// Вертає true якщо тип є покажчиком
            /// </summary>
            template<typename T>
            struct IsPointerT<T*> : LogicTrue {};
#pragma region C Array
			/// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T>
            struct IsCArray : LogicFalse {};

            /// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T>
            struct IsCArray<T[]> : LogicTrue {};

            /// <summary>
            /// Вертає true якщо тип є статичним масивом
            /// </summary>
            template<class T, SizeT N>
            struct IsCArray<T[N]> : LogicTrue {};

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T>
            struct RemoveSizeFromCArray { typedef T type; };

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T>
            struct RemoveSizeFromCArray<T[]> { typedef T type; };

            /// <summary>
            /// Вертає тип елементам масиву
            /// </summary>
            template<class T, SizeT N>
            struct RemoveSizeFromCArray<T[N]> { typedef T type; };
#pragma endregion

#pragma region Add pointer
            ///<summary>Деталі реалізації</summary>
            namespace Details
            {
                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, bool TIsFunctionType = false >
                struct AddPointer {
                    using Type = typename TypeTraits::RemoveRefT<T>::Type*;
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T >
                struct AddPointer<T, true> {
                    using Type = T;
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, class... TArgs >
                struct AddPointer<T(TArgs...), true> {
                    using Type = T(*)(TArgs...);
                };

                /// <summary>
                /// Реалізація додавання покажчика до типу
                /// </summary>
                template< class T, class... TArgs >
                struct AddPointer<T(TArgs..., ...), true> {
                    using Type = T(*)(TArgs..., ...);
                };
            }

            /// <summary>
            /// Додавання покажчика до типу
            /// </summary>
            template<typename T >
            struct AddPointerStruct : Details::AddPointer<T, IsFunction<T>::State> {};

            /// <summary>
            /// Додавання покажчика до типу
            /// </summary>
            template<typename T>
            using AddPointer = typename AddPointerStruct<T>::Type;
#pragma endregion
		}
	}
}