#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			class InternalSensorsService final : IBootUpService<InternalSensorsService> {
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto InternalSensorsServiceDefinition = ObjectDefinition(
						Services::InternalSensorsService,
						NullCtorDescription(Services::InternalSensorsService),
						ObjectMethodsDefs(),
						ObjectPropsDefs(
							ObjectProperty(Services::InternalSensorsService, CurrentMemBlkCount, CurrentMemBlkCount),
							ObjectProperty(Services::InternalSensorsService, CurrentTotalAllocatedMemory, CurrentTotalAllocatedMemory),
							ObjectProperty(Services::InternalSensorsService, CurrentTotalHeapsUsage, CurrentTotalHeapsUsage),
							ObjectProperty(Services::InternalSensorsService, ExcecutableNtPath, GetExcecutableNtPath),
							ObjectProperty(Services::InternalSensorsService, CpuCount, CpuCount),
							ObjectProperty(Services::InternalSensorsService, MemoryPageSize, MemoryPageSize),
							ObjectProperty(Services::InternalSensorsService, PhysicalMemorySize, PhysicalMemorySize)
						)
					);

					return GenObjectRTTIRecord(InternalSensorsServiceDefinition);
				}
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

				SizeT CpuCount() const;

				SizeT MemoryPageSize() const;

				SizeT PhysicalMemorySize() const;

				auto KernelVersion() const {
					return CurrentSettings::GetCurrentKernelVersion();
				}
			};
		}
	}
}