#pragma once
#include "Seed.Core.NewCTTI.Base.h"
#include "Seed.Core.NewCTTI.TypeName.h"

namespace Seed
{
	inline namespace Core
	{
		namespace NewCTTI
		{
			namespace TypeTraits
			{
				///<summary>Перелік категорій типів</summary>
				enum class EType : UInt8
				{
					Logic,
					Numeric,
					FloatNumeric,
					Character,
					Collection,
					String,
					FixedCollection,
					AssociableCollection,
					Tuple,
					Pair,
					Pointer,
					Reference,
					FixedArray,
					POD,
					UserStruct,
					Dynamic,
					Undefine = Math::Max<UInt8>()
				};

				/////<summary>Вертає чи тип є примітивом</summary>
				//constexpr bool IsTypePrimitive(EType type)
				//{
				//	switch (type)
				//	{
				//	case EType::Logic:
				//		return true;
				//	case EType::Numeric:
				//		return true;
				//	case EType::Character:
				//		return true;
				//	case EType::Collection:
				//		return false;
				//	case EType::FixedCollection:
				//		return false;
				//	case EType::AssociableCollection:
				//		return false;
				//	case EType::POD:
				//		return false;
				//	case EType::UserStruct:
				//		return false;
				//	case EType::Dynamic:
				//		return false;
				//	case EType::Undefine:
				//		return false;
				//	default:
				//		return false;
				//	}
				//}

				/////<summary>Вертає чи тип є колекцією</summary>
				//constexpr bool IsTypeCollection(EType type)
				//{
				//	switch (type)
				//	{
				//	case EType::Logic:
				//		return false;
				//	case EType::Numeric:
				//		return false;
				//	case EType::Character:
				//		return false;
				//	case EType::Collection:
				//		return true;
				//	case EType::FixedCollection:
				//		return true;
				//	case EType::AssociableCollection:
				//		return true;
				//	case EType::POD:
				//		return false;
				//	case EType::UserStruct:
				//		return false;
				//	case EType::Dynamic:
				//		return false;
				//	case EType::Undefine:
				//		return false;
				//	default:
				//		return false;
				//	}
				//}

				namespace Details
				{
#pragma region Base implementation
					template<typename T, EType TTypeClass, bool TIsSerilizable, bool TIsConst, bool TIsVolatile>
					struct BaseCTTITypeDataImpl
					{
						using Type = T;
						static constexpr EType TypeClass = TTypeClass;
						static constexpr SizeT Size = sizeof(T);
						static constexpr Boolean IsSerilizable = TIsSerilizable;
						static constexpr Boolean IsConst = TIsConst;
						static constexpr Boolean IsVolatile = TIsVolatile;
						static constexpr auto TypeName = NewCTTI::TypeName<T>();
						static constexpr auto CXXTypeName = NewCTTI::CXXTypeName<T>();
					};
#pragma endregion
//#pragma region Base CV implementation
//					template<typename T, EType TTypeClass, bool TIsSerilizable>
//					struct BaseCTTITypeDataImpl<const T, TTypeClass, TIsSerilizable>
//					{
//						using Type = T;
//						static constexpr EType TypeClass = TTypeClass;
//						static constexpr SizeT Size = sizeof(T);
//						static constexpr Boolean IsSerilizable = TIsSerilizable;
//						static constexpr Boolean IsConst = true;
//						static constexpr Boolean IsVolatile = false;
//						static constexpr auto TypeName = NewCTTI::TypeName<T>();
//						static constexpr auto CXXTypeName = NewCTTI::CXXTypeName<T>();
//					};
//
//					template<typename T, EType TTypeClass, bool TIsSerilizable>
//					struct BaseCTTITypeDataImpl<volatile T, TTypeClass, TIsSerilizable>
//					{
//						using Type = T;
//						static constexpr EType TypeClass = TTypeClass;
//						static constexpr SizeT Size = sizeof(T);
//						static constexpr Boolean IsSerilizable = TIsSerilizable;
//						static constexpr Boolean IsConst = false;
//						static constexpr Boolean IsVolatile = true;
//						static constexpr auto TypeName = NewCTTI::TypeName<T>();
//						static constexpr auto CXXTypeName = NewCTTI::CXXTypeName<T>();
//					};
//
//					template<typename T, EType TTypeClass, bool TIsSerilizable>
//					struct BaseCTTITypeDataImpl<const volatile T, TTypeClass, TIsSerilizable>
//					{
//						using Type = T;
//						static constexpr EType TypeClass = TTypeClass;
//						static constexpr SizeT Size = sizeof(T);
//						static constexpr Boolean IsSerilizable = TIsSerilizable;
//						static constexpr Boolean IsConst = true;
//						static constexpr Boolean IsVolatile = true;
//						static constexpr auto TypeName = NewCTTI::TypeName<T>();
//						static constexpr auto CXXTypeName = NewCTTI::CXXTypeName<T>();
//					};
//#pragma endregion
#pragma region Default Implementation
					template<typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl : BaseCTTITypeDataImpl<T, EType::Undefine, false, TIsConst, TIsVolatile>
					{};

					template<typename T>
					struct CTTITypeData : CTTITypeDataImpl<typename Seed::TypeTraits::RemoveCV<T>, Seed::TypeTraits::IsConst<T>, Seed::TypeTraits::IsVolatile<T>>
					{};
#pragma endregion
#pragma region Pointer, Reference and CArray implementations
					
					template<typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<T*, TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<T*, EType::Pointer, false, TIsConst, TIsVolatile>
					{
						using PointerType = CTTITypeData<T>;
					};

					template<typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<T&, TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<T&, EType::Reference, false, TIsConst, TIsVolatile>
					{
						using ReferenceType = CTTITypeData<T>;
					};

					template<typename T, SizeT TSize, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl<T[TSize], TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<T[TSize], EType::Reference, false, TIsConst, TIsVolatile>
					{
						using ItemType = CTTITypeData<T>;
					};
#pragma endregion

				}
#pragma region Public using
				template<typename T>
				using CTTITypeMetaData = Details::CTTITypeData<T>;
#pragma endregion
			}
		}
	}
}