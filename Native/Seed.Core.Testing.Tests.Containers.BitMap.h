#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::API::Containers::BitMap
{
	SeedTestDef(DefaultConstructorInitializesBitsAsZeroTest);
	SeedTestDef(SetAllConstructorSetsOnlyActualBitsTest);
	SeedTestDef(SetResetBitAtClusterBoundaryTest);
	SeedTestDef(SetResetBitSequelAcrossClustersTest);
	SeedTestDef(FindBitSequelSkipsOccupiedRangesTest);
	SeedTestDef(FindBitSequalFromHonorsStartAndBoundsTest);
	SeedTestDef(BitwiseOperationsRespectPartialClusterTest);
	SeedTestDef(EqualityIgnoresPaddingBitsTest);
	SeedTestDef(OutOfRangeAccessThrowsTest);
}
