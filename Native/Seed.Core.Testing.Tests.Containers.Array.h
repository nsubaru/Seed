#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypeArrayTest : public ITest {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyArrayTest : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypeArrayTest : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyArrayTest : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}