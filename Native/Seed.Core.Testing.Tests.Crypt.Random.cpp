#include "Seed.Core.Testing.Tests.Crypt.Random.h"

#include "Seed.Core.Crypt.h"

bool Seed::Core::Testing::Test::Crypt::Random::Excecute() {
	return Seed::Crypt::Random() != Seed::Crypt::Random();
}
