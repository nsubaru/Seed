#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypePriorityQueueTest : public TestClass<SimpleTypePriorityQueueTest> {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyPriorityQueueTest : public TestClass<SimpleTypeConcurencyPriorityQueueTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypePriorityQueueTest : public TestClass<ComplexTypePriorityQueueTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyPriorityQueueTest : public TestClass<ComplexTypeConcurencyPriorityQueueTest> {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}