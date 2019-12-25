#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.CoreLogger.h"
#include "Seed.Core.Services.InternalSensors.h"
#include "Seed.Core.Services.KMS.h"

#include "Seed.Core.DepencyInjection.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			DI::DepencyInjector & GetGlobalIOC();
		}
	}
}