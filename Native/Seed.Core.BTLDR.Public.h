#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.SettingsAPI.Default.h"

namespace Seed {
	inline namespace Core {
		namespace Btlrd {
			struct BtlrdInPlaceUpdateData {
				Seed::UInt64 SectionAddress;
				Seed::UInt64 InPlaceUpdataEntryPoint;
				Seed::UInt64 EntryPoint;
				Seed::Handle NewKernelFileHandle;
				Seed::UInt64 ntdllHandle;
				Seed::UInt64 ldrLoadFunction;
				Seed::UInt64 oldImageBase;

				Seed::UInt64 ReloadCount;
				Seed::UInt64 ProcessInitTime;
				Seed::UInt64 LastReloadTime;
			};

			UInt64 GetReloadsCount();
			UInt64 GetProcessInitTime();
			UInt64 GetLastReloadTime();
			void SetFastReboot();
		}
	}
}

