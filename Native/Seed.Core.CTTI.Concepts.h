#pragma once
#include "Seed.Core.CTTI.EType.h"
#include "Seed.Core.CTTI.Concepts.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace CTTI
		{
			namespace Concepts
			{
				template<typename T, CTTI::EType TType>
				concept IsCttiType = CTTI::TypeTraits::CTTITypeMetaData<T>::TypeClass == TType;

				template<typename T>
				concept IsCttiLogic = IsCttiType<T, CTTI::EType::Logic>;

				template<typename T>
				concept IsCttiNumeric = IsCttiType<T, CTTI::EType::Numeric>;

				template<typename T>
				concept IsCttiFloatNumeric = IsCttiType<T, CTTI::EType::FloatNumeric>;

				template<typename T>
				concept IsCttiString = IsCttiType<T, CTTI::EType::String>;

				template<typename T>
				concept IsCttiStruct = IsCttiType<T, CTTI::EType::Struct>;

				template<typename T>
				concept IsCttiSerelizable = CTTI::TypeTraits::CTTITypeMetaData<T>::IsSerilizable;
			}
		}
	}
}