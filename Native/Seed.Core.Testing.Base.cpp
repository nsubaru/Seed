// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Base.h"

#include "Seed.Core.Testing.Tests.h"

#include "Seed.Core.CoreLog.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

bool Seed::Core::Testing::InitTestRegister()
{
	try {
		TestRegister::tests = new Containers::ConcurencyList<TestRegister::TestRecord>();
		RegisterCoreTests();
	}
	catch (...) {
		return false;
	}
	return true;
}

void Seed::Core::Testing::StopTestRegister()
{
	TestRegister::tests->RemoveAll([](TestRegister::TestRecord&& record) {delete record.testObj; });
	delete TestRegister::tests;
}

Seed::Boolean Seed::Core::Init::TestRegister() noexcept
{
	return Seed::Core::Testing::InitTestRegister();
}

void Seed::Core::Stop::TestRegister() noexcept
{
	try {
		Seed::Core::Testing::StopTestRegister();
	}
	catch (...) {
		toCoreLog(L"При зупинці реєстру тестів виникла помилка!!!");
	}
}