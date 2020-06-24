#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypeArrayTest : public TestClass<SimpleTypeArrayTest> {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyArrayTest : public TestClass<SimpleTypeConcurencyArrayTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypeArrayTest : public TestClass<ComplexTypeArrayTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyArrayTest : public TestClass<ComplexTypeConcurencyArrayTest> {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}