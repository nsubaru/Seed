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

	SeedTestDef(UniStringConstDifEquality);
	SeedTestDef(UniStringConstADifEquality);
	SeedTestDef(UniStringStringDifEquality);
	SeedTestDef(UniStringTextRefDifEquality);
	SeedTestDef(UniStringTextARefDifEquality);
	SeedTestDef(UniStringConstArrDifEquality);
	SeedTestDef(UniStringConstArrACharDifEquality);
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

	SeedTestDef(UniStringConstDifSpaceship);
	SeedTestDef(UniStringConstADifSpaceship);
	SeedTestDef(UniStringStringDifSpaceship);
	SeedTestDef(UniStringTextRefDifSpaceship);
	SeedTestDef(UniStringTextARefDifSpaceship);
	SeedTestDef(UniStringConstArrDifSpaceship);
	SeedTestDef(UniStringConstArrACharDifSpaceship);
}

namespace Seed::Core::Testing::Test::UniString::Comparing::Addition
{
	SeedTestDef(UniStringConstModeEmptyAdditionLiteral);
	SeedTestDef(UniStringConstModeEmptyAdditionALiteral);
	SeedTestDef(UniStringConstModeEmptyAdditionConst);
	SeedTestDef(UniStringConstModeEmptyAdditionAConst);
	SeedTestDef(UniStringConstModeEmptyAdditionString);
	SeedTestDef(UniStringConstModeEmptyAdditionConstExpr);
	SeedTestDef(UniStringConstModeEmptyAdditionConstAExpr);
	SeedTestDef(UniStringConstModeEmptyAdditionUniStr);

	SeedTestDef(UniStringConstModeAdditionLiteral);
	SeedTestDef(UniStringConstModeAdditionALiteral);
	SeedTestDef(UniStringConstModeAdditionConst);
	SeedTestDef(UniStringConstModeAdditionAConst);
	SeedTestDef(UniStringConstModeAdditionString);
	SeedTestDef(UniStringConstModeAdditionConstExpr);
	SeedTestDef(UniStringConstModeAdditionConstAExpr);
	SeedTestDef(UniStringConstModeAdditionUniStr);

	SeedTestDef(UniStringDynModeEmptyAdditionLiteral);
	SeedTestDef(UniStringDynModeEmptyAdditionALiteral);
	SeedTestDef(UniStringDynModeEmptyAdditionConst);
	SeedTestDef(UniStringDynModeEmptyAdditionAConst);
	SeedTestDef(UniStringDynModeEmptyAdditionString);
	SeedTestDef(UniStringDynModeEmptyAdditionConstExpr);
	SeedTestDef(UniStringDynModeEmptyAdditionConstAExpr);
	SeedTestDef(UniStringDynModeEmptyAdditionUniStr);

	SeedTestDef(UniStringDynModeAdditionLiteral);
	SeedTestDef(UniStringDynModeAdditionALiteral);
	SeedTestDef(UniStringDynModeAdditionConst);
	SeedTestDef(UniStringDynModeAdditionAConst);
	SeedTestDef(UniStringDynModeAdditionString);
	SeedTestDef(UniStringDynModeAdditionConstExpr);
	SeedTestDef(UniStringDynModeAdditionConstAExpr);
	SeedTestDef(UniStringDynModeAdditionUniStr);
}