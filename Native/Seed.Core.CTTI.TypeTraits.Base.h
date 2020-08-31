#pragma once
#include "Seed.Core.CTTI.TypeName.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			namespace TypeTraits
			{
				namespace Details
				{
					struct ISerilizableGenericBase {};

					template<typename T>
					struct IsDynamicT : Core::TypeTraits::LogicFalse {};
				}
				///<summary>Перелік категорій типів</summary>
				enum class EType : UInt8
				{
					Logic,
					Numeric,
					Enumeration,
					FloatNumeric,
					Character,
					NtTime,
					Collection,
					String,
					FixedCollection,
					AssociableCollection,
					Box,
					Tuple,
					Pair,
					Pointer,
					Reference,
					FixedArray,
					Struct,
					Object,
					Dynamic,
					Interface,
					Pod,
					Undefine = Math::Max<UInt8>()
				};
			}

			namespace Concepts
			{
				template<typename T>
				concept IsLogicCttiType = Core::Concepts::IsSame<T, Boolean>;

				template<typename T>
				concept IsNumericCttiType = Core::TypeTraits::IsIntegerNumber<T>;

				template<typename T>
				concept IsEnumertationCttiType = Core::TypeTraits::IsEnumV<T>;

				template<typename T>
				concept IsCharacterCttiType = Core::TypeTraits::IsCharacter<T>;

				template<typename T>
				concept IsNtTimeCttiType = 
					Core::TypeTraits::IsSameV<T, Date> ||
					Core::TypeTraits::IsSameV<T, DateTime> ||
					Core::TypeTraits::IsSameV<T, DateSpan>;

				template<typename T>
				concept IsCollectionCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::ICollectionGenericBase, T>;

				template<typename T>
				concept IsStringCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::IStringCollectionGenericBase, T>;

				template<typename T>
				concept IsFixedCollectionCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::IFixedCollectionGenericBase, T>;

				template<typename T>
				concept IsAssociableCollectionCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::IAssociableCollectionGenericBase, T>;

				template<typename T>
				concept IsBoxCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::ISmartPtrGenericBase, T> || 
										Core::TypeTraits::IsBasedOnV<Containers::LINQ::ILazyGenericBase, T>;

				template<typename T>
				concept IsTupleCttiType = Core::TypeTraits::IsBasedOnV<Containers::LINQ::ITupleGenericBase, T>;

				template<typename T>
				concept IsPairCttiType = Core::TypeTraits::Collection::IsPair<T>;

				template<typename T>
				concept IsPointerCttiType = Core::TypeTraits::IsPointerT<T>::State;

				template<typename T>
				concept IsReferenceCttiType = Core::TypeTraits::IsReferenceV<T>;
				
				template<typename T>
				concept IsFixedArrayCttiType = Core::TypeTraits::IsCArray<T>::State;

				template<typename T>
				concept IsStructCttiType = requires { T::FieldMetaDataGetter(); };

				template<typename T>
				concept IsObjectCttiType = Core::TypeTraits::IsBasedOnV<TypeTraits::Details::ISerilizableGenericBase, T>;

				template<typename T>
				concept IsDynamicCttiType = TypeTraits::Details::IsDynamicT<T>::State;

				template<typename T>
				concept IsPodCttiType = Core::TypeTraits::IsClassOrStructV<T> && 
										Core::TypeTraits::IsPodV<T> && 
										!(IsStructCttiType<T> || IsObjectCttiType<T>);
			}
		}
	}
}