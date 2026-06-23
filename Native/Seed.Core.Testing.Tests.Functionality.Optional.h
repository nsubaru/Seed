#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::Functionality::Optional
{
    SeedTestDef(SetValueL);
    SeedTestDef(SetValueR);

    SeedTestDef(AssignValueFromConvertible);

    SeedTestDef(SetValueL_WhenValueExist);
    SeedTestDef(SetValueR_WhenValueExist);

    SeedTestDef(GetValueL);
    SeedTestDef(GetValueR);

    SeedTestDef(GetValueL_WhenValueNotExist);
    SeedTestDef(GetValueR_WhenValueNotExist);
}