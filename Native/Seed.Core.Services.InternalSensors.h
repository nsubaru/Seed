#pragma once
#include "Seed.Core.Services.Base.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			class InternalSensorsService {
			public:
				InternalSensorsService() {}

				String GetExcecutableNtPath() {
					return CurrentSettings().VirtualSetting.KernelExcecutableLocation;
				}
			};
		}
	}
}