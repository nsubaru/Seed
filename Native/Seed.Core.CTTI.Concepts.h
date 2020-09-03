#pragma once
#include "Seed.Core.CTTI.Concepts.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			namespace Concepts
			{
				template<typename T, CTTI::TypeTraits::EType TType>
				concept IsCttiType = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == TType;

				template<typename T>
				concept IsCttiLogic = IsCttiType<T, CTTI::TypeTraits::EType::Logic>;

				template<typename T>
				concept IsCttiNumeric = IsCttiType<T, CTTI::TypeTraits::EType::Numeric>;

				template<typename T>
				concept IsCttiFloatNumeric = IsCttiType<T, CTTI::TypeTraits::EType::FloatNumeric>;

				template<typename T>
				concept IsCttiString = IsCttiType<T, CTTI::TypeTraits::EType::String>;

				template<typename T>
				concept IsCttiStruct = IsCttiType<T, CTTI::TypeTraits::EType::Struct>;

				template<typename T>
				concept IsCttiSerelizable = CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;
			}
		}
	}
}