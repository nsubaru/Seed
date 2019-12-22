#pragma once
#include "Seed.Core.NewSerilization.Base.h"
#include "Seed.Core.NewSerilization.Collection.h"
#include "Seed.Core.NewSerilization.Composits.h"

namespace Seed
{
    inline namespace Core
    {
        namespace NewSerilization
        {
			template<typename TSerelizeable>
			using ISerilizable = NewCTTI::TypeTraits::ISerilizable<TSerelizeable>;
		}
	}
}