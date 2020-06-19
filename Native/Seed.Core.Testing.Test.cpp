#include "Seed.Core.Testing.Test.h"

auto& TestsStorage() 
{
	using namespace Seed;
	using namespace Seed::Core::Containers;
	static auto tests = List<Pair<String, SmartPtr<Testing::ITest>>>();

	return tests;
}

void Seed::Core::Testing::LoadTest(Seed::Core::String&& name, Seed::Core::Testing::ITest*&& instance)
{
	auto& tests = TestsStorage();
	tests += {MoveRValue(name), MoveRValue(instance)};
}
