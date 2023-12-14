#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::Services::Pool
{
	SeedTestDef(PoolStartTest);
	SeedTestDef(PoolCreateDestTest);
	SeedTestDef(PoolCustomTest);
	SeedTestDef(PoolOverflowTest);
	SeedTestDef(PoolFlushTest);
	SeedTestDef(PoolExtendTest);
	SeedTestDef(PoolShrinkTest);
}