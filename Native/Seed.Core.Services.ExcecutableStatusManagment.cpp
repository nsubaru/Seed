// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Services.ExcecutableStatusManagment.h"

extern volatile Seed::Boolean fastReboot;

void Seed::Core::Services::ExcecutableStatusManagmentService::ReloadKernel() {
	fastReboot = true;
	data->waitableReloadEvent.Set();
}
