#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class SimpleTypeFixedQueueTest final : public ITest {
						virtual bool Excecute() override;
					};

					class SimpleTypeConcurencyFixedQueueTest final : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypeFixedQueueTest final : public ITest {
						virtual bool Excecute() override;
					};

					class ComplexTypeConcurencyFixedQueueTest final : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}