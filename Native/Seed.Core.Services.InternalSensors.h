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

				String GetExcecutableNtPath() const {
#pragma warning(push)
//Call internal unsafe String ctor
#pragma warning(disable: 4996)
					return const_cast<Char*>(static_cast<const Char*>(CurrentSettings().VirtualSetting.KernelExcecutableLocation));
#pragma warning(pop)
				}

				SizeT CurrentTotalAllocatedMemory() const {
					return Memory::CurrentTotalAllocatedMemory();
				}

				SizeT CurrentTotalHeapsUsage() const {
					return Memory::CurrentTotalHeapsUsage();
				}

				SizeT CurrentMemBlkCount() const {
					return Memory::CurrentTotalAllocatedMemBlks();
				}
			};
		}
	}
}