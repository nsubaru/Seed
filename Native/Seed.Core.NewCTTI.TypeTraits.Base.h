#pragma once
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
					Struct,
					Dynamic,
					Undefine = Math::Max<UInt8>()
				};

                struct ISerilizableBase {};

				namespace Details
				{
#pragma region Base implementation
					template<typename T, EType TTypeClass, bool TIsSerilizable, bool TIsConst, bool TIsVolatile>
					struct BaseCTTITypeDataImpl
					{
						static_assert(!Core::TypeTraits::IsSameV<T, void>);

						using Type = T;
						static constexpr EType TypeClass = TTypeClass;
						static constexpr SizeT Size = sizeof(T);
						static constexpr Boolean IsSerilizable = TIsSerilizable;
						static constexpr Boolean IsConst = TIsConst;
						static constexpr Boolean IsVolatile = TIsVolatile;
						static constexpr auto TypeName = NewCTTI::TypeName<T>();
						static constexpr auto CXXTypeName = NewCTTI::CXXTypeName<T>();

                        static constexpr Boolean IsContaineSubTypes =
                            !(
                            TypeClass == EType::Logic ||
                            TypeClass == EType::Numeric ||
                            TypeClass == EType::FloatNumeric ||
                            TypeClass == EType::Character ||
                            TypeClass == EType::POD);
					};
#pragma endregion
#pragma region Default Implementation
					template<typename T, bool TIsConst, bool TIsVolatile>
					struct CTTITypeDataImpl : BaseCTTITypeDataImpl<T, EType::Undefine, false, TIsConst, TIsVolatile>
					{};

                    template<typename T, typename Enable = void>
                    struct CTTITypeDataSpecialization
						: CTTITypeDataImpl<typename Seed::TypeTraits::RemoveCV<T>, Seed::TypeTraits::IsConstV<T>, Seed::TypeTraits::IsVolatile<T>>
					{};

                    template<typename T>
                    struct CTTITypeDataSpecialization
                        <
                        T,
                        typename Seed::Core::TypeTraits::EnableIfV<!Seed::Core::TypeTraits::IsBasedOnV<ISerilizableBase, T>, void>
                        >
                        : CTTITypeDataImpl<typename Seed::TypeTraits::RemoveCV<T>, Seed::TypeTraits::IsConstV<T>, Seed::TypeTraits::IsVolatile<T>> {

                    };

					template<typename T>
					struct CTTITypeData : CTTITypeDataSpecialization<T>
					{
					};
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
					struct CTTITypeDataImpl<T[TSize], TIsConst, TIsVolatile> : BaseCTTITypeDataImpl<T[TSize], EType::FixedArray, CTTITypeData<T>::IsSerilizable, TIsConst, TIsVolatile>
					{
                        static constexpr SizeT Count = TSize;
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