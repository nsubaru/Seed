#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::Memory::ImageResidency
{
    SeedTestDef(CurrentImagePolicyTest);
    SeedTestDef(SyntheticImagePolicyTest);
    SeedTestDef(SessionLifecycleTest);
    SeedTestDef(PartialFailureRollbackTest);
}
