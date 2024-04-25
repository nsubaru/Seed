#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Text.UniString.h"

namespace Seed::Core::Testing::Test::UniString::Creation
{
	SeedTestDef(EmptyUniStringCreation);
	SeedTestDef(ConstStringUniStringCreation);
	SeedTestDef(ConstAStringUniStringCreation);
	SeedTestDef(StringUniStringCreation);
	SeedTestDef(TextRefUniStringCreation);
	SeedTestDef(TextARefUniStringCreation);
	SeedTestDef(ConstArrUniStringCreation);
	SeedTestDef(ConstArrACharUniStringCreation);
}

namespace Seed::Core::Testing::Test::UniString::Comparing::Equality
{
	SeedTestDef(UniStringConstEquality);
	SeedTestDef(UniStringConstAEquality);
	SeedTestDef(UniStringStringEquality);
	SeedTestDef(UniStringTextRefEquality);
	SeedTestDef(UniStringTextARefEquality);
	SeedTestDef(UniStringConstArrEquality);
	SeedTestDef(UniStringConstArrACharEquality);
}

namespace Seed::Core::Testing::Test::UniString::Comparing::Spaceship
{
	SeedTestDef(UniStringConstSpaceship);
	SeedTestDef(UniStringConstASpaceship);
	SeedTestDef(UniStringStringSpaceship);
	SeedTestDef(UniStringTextRefSpaceship);
	SeedTestDef(UniStringTextARefSpaceship);
	SeedTestDef(UniStringConstArrSpaceship);
	SeedTestDef(UniStringConstArrACharSpaceship);
}