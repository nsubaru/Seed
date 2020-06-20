#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypeFixedQueueTest final : public TestClass<SimpleTypeFixedQueueTest> {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyFixedQueueTest final : public TestClass<SimpleTypeConcurencyFixedQueueTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypeFixedQueueTest final : public TestClass<ComplexTypeFixedQueueTest> {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyFixedQueueTest final : public TestClass<ComplexTypeConcurencyFixedQueueTest> {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}