#pragma once
#include "Seed.Core.Serilization.Base.h"
#include "Seed.Core.Serilization.IntegratedTypes.h"
#include "Seed.Core.Serilization.Collection.h"
#include "Seed.Core.Serilization.Composits.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Serilization
        {
			template<typename TSerelizeable>
			using ISerilizable = CTTI::TypeTraits::ISerilizable<TSerelizeable>;
		}
	}
}