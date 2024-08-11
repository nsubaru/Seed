#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed::Core::Testing::Test::Serializator::Serialization
{
    SeedTestDef(EmptyDynamic);
    SeedTestDef(PrimitiveDynamic);
    SeedTestDef(StructDynamic);
    SeedTestDef(StringDynamic);
    SeedTestDef(CollectionDynamic);
    SeedTestDef(TupleDynamic);
    SeedTestDef(DynamicInDynamic);
    SeedTestDef(DynamicStructFieldInDynamic);
    SeedTestDef(CollectionWithDynamicInDynamic);
    SeedTestDef(TupleWithDynamicInDynamic);
}