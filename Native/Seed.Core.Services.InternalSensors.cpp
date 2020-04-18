// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Services.InternalSensors.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

using namespace Seed;

SizeT Seed::Core::Services::InternalSensorsService::CpuCount() const {
	SYSTEM_BASIC_INFORMATION basic_inf;
	NtQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS::SystemBasicInformation, 
		&basic_inf, 
		sizeof(SYSTEM_BASIC_INFORMATION), 
		NULL);
	return basic_inf.NumberOfProcessors;
}

SizeT Seed::Core::Services::InternalSensorsService::MemoryPageSize() const {
	SYSTEM_BASIC_INFORMATION basic_inf;
	NtQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS::SystemBasicInformation, 
		&basic_inf, 
		sizeof(SYSTEM_BASIC_INFORMATION), 
		NULL);
	return basic_inf.PageSize;
}

SizeT Seed::Core::Services::InternalSensorsService::PhysicalMemorySize() const {
	SYSTEM_BASIC_INFORMATION basic_inf;
	NtQuerySystemInformation(
		SYSTEM_INFORMATION_CLASS::SystemBasicInformation, 
		&basic_inf, 
		sizeof(SYSTEM_BASIC_INFORMATION), 
		NULL);
	return basic_inf.NumberOfPhysicalPages * basic_inf.PageSize;
}

#pragma warning(pop)