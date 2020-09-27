#pragma once
#include "Seed.Core.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			/// <summary>
			/// Перелік категорій типів
			/// </summary>
			enum class EType : UInt8
			{
				MetaData,
				Logic,
				Numeric,
				Enumeration,
				FloatNumeric,
				Character,
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

			/// <summary>
			/// Вертає чи клас тип є простим, тобто не містить ніяких вкладених типів
			/// </summary>
			/// <param name="type">Класс типів</param>
			/// <returns>Логічне значення</returns>
			constexpr Boolean IsSimpleTypes(EType type) {
				switch (type)
				{
				case Seed::Core::CTTI::EType::MetaData:
					return false;
				case Seed::Core::CTTI::EType::Logic:
					return true;
				case Seed::Core::CTTI::EType::Numeric:
					return true;
				case Seed::Core::CTTI::EType::Enumeration:
					return false;
				case Seed::Core::CTTI::EType::FloatNumeric:
					return true;
				case Seed::Core::CTTI::EType::Character:
					return true;
				case Seed::Core::CTTI::EType::Collection:
					return false;
				case Seed::Core::CTTI::EType::String:
					return false;
				case Seed::Core::CTTI::EType::FixedCollection:
					return false;
				case Seed::Core::CTTI::EType::AssociableCollection:
					return false;
				case Seed::Core::CTTI::EType::Box:
					return false;
				case Seed::Core::CTTI::EType::Tuple:
					return false;
				case Seed::Core::CTTI::EType::Pair:
					return false;
				case Seed::Core::CTTI::EType::Pointer:
					return false;
				case Seed::Core::CTTI::EType::Reference:
					return false;
				case Seed::Core::CTTI::EType::FixedArray:
					return false;
				case Seed::Core::CTTI::EType::Struct:
					return false;
				case Seed::Core::CTTI::EType::Object:
					return false;
				case Seed::Core::CTTI::EType::Dynamic:
					return false;
				case Seed::Core::CTTI::EType::Interface:
					return false;
				case Seed::Core::CTTI::EType::Pod:
					return true;
				case Seed::Core::CTTI::EType::Undefine:
					return false;
				default:
					return false;
				}
			}

			/// <summary>
			/// Вертає чи клас типу є складеним, тобто містить вкладені типи
			/// </summary>
			/// <param name="type">Класс типів</param>
			/// <returns>Логічне значення</returns>
			constexpr Boolean IsComplexTypes(EType type) {
				return !IsSimpleTypes(type);
			};
		}
	}
}