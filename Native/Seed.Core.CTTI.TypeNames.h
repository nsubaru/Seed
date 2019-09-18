#pragma once
#include "Seed.Core.CTTI.CompileStringView.h"
#include "Seed.Core.CTTI.TypeString.h"

#include "Seed.Core.Containers.Array.h"
#include "Seed.Core.Containers.List.h"

#include "Seed.Core.Exceptions.h"

#include "Seed.Core.Containers.Map.h"

namespace Seed {
	inline namespace Core {
		namespace TypeTraits {
			template<typename T>
			struct IsStringT : LogicFalse {};

			template<typename Heap>
			struct IsStringT<Seed::Core::BaseString<Heap>> : LogicTrue {};

			template<typename T>
			struct IsListT : LogicFalse {};

			template<typename THeap, typename T>
			struct IsListT<Seed::Core::BaseList<THeap, T>> : LogicTrue {};

			template<typename T>
			struct IsArrayT : LogicFalse {};

			template<typename T>
			struct IsArrayT<Seed::Core::Array<T>> : LogicTrue {};

			template<typename T>
			struct IsMapT : LogicFalse {};

			template<typename THeap, typename TKey, typename TValue>
			struct IsMapT<Seed::Core::BaseMap<THeap, TKey, TValue>> : LogicTrue {};

			template<typename T>
			struct IsPairT : LogicFalse {};

			template<typename TKey, typename TValue>
			struct IsPairT<Seed::Core::Pair<TKey, TValue>> : LogicTrue {};

			template<typename T> constexpr bool IsString = IsStringT<T>::State;
			template<typename T> constexpr bool IsList = IsListT<T>::State;
			template<typename T> constexpr bool IsArray = IsArrayT<T>::State;
			template<typename T> constexpr bool IsMap = IsMapT<T>::State;

			template<typename T> constexpr bool IsContainer = IsString<T> || IsList<T> || IsArray<T> || IsMap<T>;

			template<typename T> constexpr bool IsPair = IsPairT<T>::State;

			template<typename T>
			struct ContainerSubTypeT {};

			template<typename THeap>
			struct ContainerSubTypeT<BaseString<THeap>> {
				using Type = UChar;
			};

			template<typename T>
			struct ContainerSubTypeT<Array<T>> {
				using Type = T;
			};

			template<typename THeap, typename T>
			struct ContainerSubTypeT<BaseList<THeap, T>> {
				using Type = T;
			};

			template<typename THeap, typename TKey, typename TValue>
			struct ContainerSubTypeT<BaseMap<THeap, TKey, TValue>> {
				using Type = Seed::Core::Pair<TKey, TValue>;
			};

			template<typename T>
			using ContainerSubType = typename ContainerSubTypeT<T>::Type;

			template<typename T>
			constexpr bool IsCoreTypes = IsNumeric<T> || IsCharacter<T> || IsBoolean<T> || IsContainer<T> || IsPair<T>;

			template<typename T>
			constexpr bool IsSimpleType = IsNumeric<T> || IsCharacter<T> || IsBoolean<T> || IsPodV<T>;
		}
		namespace CTTI {

			template<typename T>
			constexpr auto SimplTypeName() {
				if constexpr (TypeTraits::IsSameV<Seed::Int8, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::Int8")());
				else if constexpr (TypeTraits::IsSameV<Seed::Int16, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::Int16")());
				else if constexpr (TypeTraits::IsSameV<Seed::Int32, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::Int32")());
				else if constexpr (TypeTraits::IsSameV<Seed::Int64, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::Int64")());
				else if constexpr (TypeTraits::IsSameV<Seed::UInt8, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::UInt8")());
				else if constexpr (TypeTraits::IsSameV<Seed::UInt16, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::UInt16")());
				else if constexpr (TypeTraits::IsSameV<Seed::UInt32, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::UInt32")());
				else if constexpr (TypeTraits::IsSameV<Seed::UInt64, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::UInt64")());
				else if constexpr (TypeTraits::IsSameV<Seed::AChar, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::AChar")());
				else if constexpr (TypeTraits::IsSameV<Seed::UChar, T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::UChar")());
				else if constexpr (TypeTraits::IsString<T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::String")());
				else if constexpr (TypeTraits::IsArray<T>)
					return 
						CTTI::TypeStringToArray(GenTypeString("Seed::Array<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::Array<")()).Count - 1>() +
						SimplTypeName<TypeTraits::ContainerSubType<T>>().Resize<SimplTypeName<TypeTraits::ContainerSubType<T>>().Count - 1>() +
						CTTI::TypeStringToArray(GenTypeString(">")());
				else if constexpr (TypeTraits::IsList<T>)
					return 
						CTTI::TypeStringToArray(GenTypeString("Seed::List<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::List<")()).Count - 1>() +
						SimplTypeName<TypeTraits::ContainerSubType<T>>().Resize<SimplTypeName<TypeTraits::ContainerSubType<T>>().Count - 1>() +
						CTTI::TypeStringToArray(GenTypeString(">")());
				else if constexpr (TypeTraits::IsMap<T>)
					return 
						CTTI::TypeStringToArray(GenTypeString("Seed::Map<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::Map<")()).Count - 1>() +
						SimplTypeName<TypeTraits::PairTypes<TypeTraits::ContainerSubType<T>>::Key>().Resize<SimplTypeName<TypeTraits::PairTypes<TypeTraits::ContainerSubType<T>>::Key>().Count - 1>() +
						CTTI::TypeStringToArray(GenTypeString(",")()).Resize<CTTI::TypeStringToArray(GenTypeString(",")()).Count - 1>() +
						SimplTypeName<TypeTraits::PairTypes<TypeTraits::ContainerSubType<T>>::Value>().Resize<SimplTypeName<TypeTraits::PairTypes<TypeTraits::ContainerSubType<T>>::Value>().Count - 1>() +
						CTTI::TypeStringToArray(GenTypeString(">")());
				else { 
					if constexpr (TypeName<T>()[0] == 's' ||
						TypeName<T>()[1] == 't' ||
						TypeName<T>()[2] == 'r')

						return TypeName<T>().ResizeLeft<TypeName<T>().Count - CStr::Size("struct ")>();
					else if constexpr (TypeName<T>()[0] == 'c' ||
						TypeName<T>()[1] == 'l' ||
						TypeName<T>()[2] == 'a')

						return TypeName<T>().ResizeLeft<TypeName<T>().Count - CStr::Size("class ")>();
					else
						return TypeName<T>();
				}
			}			

			constexpr auto name = SimplTypeName<List<Map<UInt8, String>>>();
		}
	}
}