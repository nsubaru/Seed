#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace TypeTraits
		{
#pragma region Check Ref modificators
			/// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class>
            constexpr bool IsLValueReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class TTy>
            constexpr bool IsLValueReferenceV<TTy&> = true;

            /// <summary>
            /// Вертає true якщо тип LValue
            /// </summary>
            template <class TTy>
            struct IsLValueReferenceT : LogicConst<IsLValueReferenceV<TTy>> {};

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class>
            constexpr bool IsRValueReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class TTy>
            constexpr bool IsRValueReferenceV<TTy&&> = true;

            /// <summary>
            /// Вертає true якщо тип RValue
            /// </summary>
            template <class TTy>
            struct IsRValueReferenceT : LogicConst<IsRValueReferenceV<TTy>> {};

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class>
            constexpr bool IsReferenceV = false;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            constexpr bool IsReferenceV<TTy&> = true;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            constexpr bool IsReferenceV<TTy&&> = true;

            /// <summary>
            /// Вертає true якщо тип силка
            /// </summary>
            template <class TTy>
            struct IsReferenceT : LogicConst<IsReferenceV<TTy>> {};
#pragma endregion

#pragma region Remove Ref
			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRefT { using Type = T; };

			///<summary>Знімає силку з типу</summary>
			template<typename T>
			struct RemoveRefT<T&> { using Type = T; };

			///<summary>Знімає rv-силку з типу</summary>
			template<typename T>
			struct RemoveRefT<T&&> { using Type = T; };

			///<summary>Тип без силки</summary>
			template<typename T>
			using RemoveRefV = typename RemoveRefT<T>::Type;
#pragma endregion

#pragma region Add Ref
			/// <summary>
			/// Додає LValue і RValue силку до типу
			/// </summary>
			template<class TType,
				class = void>
				struct AddReference
			{	// add reference
				using Lvalue = TType;
				using Rvalue = TType;
			};

            /// <summary>
            /// Додає LValue і RValue силку до типу
            /// </summary>
			template<class TType>
			struct AddReference<TType, VoidT<TType&>>
			{	// add reference
				using Lvalue = TType&;
				using Rvalue = TType&&;
			};

            /// <summary>
            /// Додає RValue силку
            /// </summary>
			template<class TType>
			using AddRvalueReferenceV = typename AddReference<TType>::Rvalue;

            /// <summary>
            /// Додає LValue силку
            /// </summary>
			template<class TType>
			using AddLvalueReferenceV = typename AddReference<TType>::Lvalue;

            /// <summary>
            /// Надає визначення типу в процесі інстанціювання шаблону
            /// </summary>
			template<class TType>
			AddRvalueReferenceV<TType> DeclVal() noexcept { return {}; };
#pragma endregion
		}
	}
}