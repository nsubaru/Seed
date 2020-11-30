// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Services.ExcecutableStatusManagment.h"

#include "Seed.Core.BTLDR.Public.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

void Seed::Core::Services::ExcecutableStatusManagmentService::ReloadKernel() {
	Btlrd::SetFastReboot();
	data->waitableReloadEvent.Set();
}

Seed::Time::TimeSpan Seed::Core::Services::ExcecutableStatusManagmentService::GetKernelUptime()
{
	return Time::TimeSpan(Time::DateTime::GetCurrentSystemTimeData().Ticks - Btlrd::GetProcessInitTime());
}

Seed::Time::DateTime Seed::Core::Services::ExcecutableStatusManagmentService::GetProcessInitTime()
{
	return Time::DateTime(Btlrd::GetProcessInitTime());
}

Seed::Time::DateTime Seed::Core::Services::ExcecutableStatusManagmentService::GetLastReloadTime()
{
	return Time::DateTime(Btlrd::GetLastReloadTime());
}

void Seed::Core::Services::ExcecutableStatusManagmentService::InPlaceKernelUpdate(String filePath)
{
	Throw(NotImplementedException(ExceptionPosition));
}

Seed::UInt64 Seed::Core::Services::ExcecutableStatusManagmentService::GetReloadsCount()
{
	return Btlrd::GetReloadsCount();
}