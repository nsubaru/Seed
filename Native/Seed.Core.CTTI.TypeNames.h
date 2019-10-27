#pragma once
#include "Seed.Core.Containers.TypeTraits.h"
#include "Seed.Core.CTTI.CompileStringView.h"
#include "Seed.Core.CTTI.TypeString.h"

#include "Seed.Core.Containers.h"

#include "Seed.Core.Exceptions.h"

#include "Seed.Core.Containers.h"

namespace Seed {
	inline namespace Core {
		namespace TypeTraits{
			template<typename T>
			constexpr bool IsCoreTypes = IsNumeric<T> || IsCharacter<T> || IsBoolean<T> || IsContainer<T> || Collection::IsPair<T>;

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
				else if constexpr (TypeTraits::Collection::IsString<T>)
					return CTTI::TypeStringToArray(GenTypeString("Seed::String")());
				else if constexpr (TypeTraits::Collection::IsArray<T>)
					return
					CTTI::TypeStringToArray(GenTypeString("Seed::Collection::Array<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::Collection::Array<")()).Count - 1>() +
					SimplTypeName<typename T::DataType>().Resize<SimplTypeName<typename T::DataType>().Count - 1>() +
					CTTI::TypeStringToArray(GenTypeString(">")());
				else if constexpr (TypeTraits::Collection::IsList<T>)
					return
					CTTI::TypeStringToArray(GenTypeString("Seed::Collection::List<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::Collection::List<")()).Count - 1>() +
					SimplTypeName<typename T::DataType>().Resize<SimplTypeName<typename T::DataType>().Count - 1>() +
					CTTI::TypeStringToArray(GenTypeString(">")());
				else if constexpr (TypeTraits::Collection::IsMap<T>)
					return
					CTTI::TypeStringToArray(GenTypeString("Seed::Collection::Map<")()).Resize<CTTI::TypeStringToArray(GenTypeString("Seed::Collection::Map<")()).Count - 1>() +
					SimplTypeName<typename T::KeyType>().Resize<SimplTypeName<typename T::KeyType>().Count - 1>() +
					CTTI::TypeStringToArray(GenTypeString(",")()).Resize<CTTI::TypeStringToArray(GenTypeString(",")()).Count - 1>() +
					SimplTypeName<typename T::ValueType>().Resize<SimplTypeName<typename T::ValueType>().Count - 1>() +
					CTTI::TypeStringToArray(GenTypeString(">")());
				//TODO: Add other containers name simplification
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

			constexpr auto name = SimplTypeName<Containers::List<Containers::Map<UInt8, String>>>();
		}
	}
}