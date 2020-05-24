// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Testing.Tests.Crypt.Random.h"

#include "Seed.Core.Crypt.h"

bool Seed::Core::Testing::Test::Crypt::Random::Excecute() {
	return Seed::Crypt::Random() != Seed::Crypt::Random();
}
