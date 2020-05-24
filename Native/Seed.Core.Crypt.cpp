// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Crypt.Random.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

using namespace Seed;

unsigned long randSeed;

Boolean Seed::Core::Init::Crypt() noexcept
{
	LARGE_INTEGER t;
	NtQuerySystemTime(&t);
	randSeed = (unsigned long)t.QuadPart;
	return true;
}

void Seed::Core::Stop::Crypt() noexcept
{
}

UInt32 Seed::Core::Crypt::Random() {
	return RtlRandomEx(&randSeed);
}
