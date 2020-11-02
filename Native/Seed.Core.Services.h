#pragma once
#include "Seed.Core.Services.Base.h"

#include "Seed.Core.Services.BootupServicesManager.h"

#include "Seed.Core.Services.AutomaticSelfTesting.h"
#include "Seed.Core.Services.CoreLogger.h"
#include "Seed.Core.Services.SystemThreadPoolService.h"
#include "Seed.Core.Services.InternalSensors.h"
#include "Seed.Core.Services.EventService.h"
#include "Seed.Core.Services.ExcecutableStatusManagment.h"
#include "Seed.Core.Services.ObjectNamespaceService.h"
#include "Seed.Core.Services.HostPartitionProvider.h"
#include "Seed.Core.Services.SerilizationService.h"

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