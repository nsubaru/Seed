#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypePriorityQueueTest : public ITest {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyPriorityQueueTest : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypePriorityQueueTest : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyPriorityQueueTest : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}