#include "Seed.Core.Services.ExcecutableStatusManagment.h"

extern volatile Seed::Boolean fastReboot;

void Seed::Core::Services::ExcecutableStatusManagmentService::ReloadKernel() {
	fastReboot = true;
	data->waitableReloadEvent.Set();
}
