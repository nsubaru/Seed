#include "Seed.Core.Runtime.h"

#include "Seed.Core.Manage.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#pragma region Static Init

Seed::List<PF>* dtorsList;

#pragma section(".INIT$z",read)
__declspec(allocate(".INIT$z")) const PF InitSegEnd = (PF)1;
#pragma section(".INIT$a", read)
__declspec(allocate(".INIT$a")) const PF InitSegStart = (PF)1;

int __cdecl RegisterStaticDtors(PF pf) {
	dtorsList->AddLast(pf);
	return 0;
}

Seed::Boolean Seed::Core::Init::StaticVariables() noexcept
{
	try {
		dtorsList = new Seed::List<PF>();
		const PF* x = &InitSegStart;
		const PF* end = &InitSegEnd;
		for (++x; x < end; ++x)
			if (*x) (*x)();
	}
	catch (...) {
		toCoreLog(L"При ініціалізації статичних об'єктів було перехоплено неочікувану помилку");
		return false;
	}
	return true;
}

void Seed::Core::Stop::StaticVariables() noexcept
{
	try {
		auto dtor = dtorsList->Last;
		while (dtor != nullptr) {
			(*dtor)();
			++dtor;
		}
		delete dtorsList;
	}
	catch (...) {
		toCoreLog(L"При деструкції статичних об'єктів було перехоплено неочікувану помилку");
	}
}
#pragma endregion

void Seed::Core::RuntimeEntryPoint()
{
	//Phose 0. Init
	{
		if (!Init::Protection())
			return;
		if (!Init::Loging())
			return;
		if (!Init::SettingsAPI())
			return;
		if (!Init::Exceptions())
			return;
		if (!Init::MemoryManagment())
			return;
		if (!Init::Threading())
			return;
		if (!Init::StaticVariables())
			return;
	}

	Main();

	//Phase 0. Stop
	{
		Stop::StaticVariables();
		Stop::Threading();
		Stop::MemoryManagment();
		Stop::Exceptions();
		Stop::SettingsAPI();
		Stop::Loging();
		Stop::Protection();
	}
}