#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.DynamicCircularBuffer.h"
#include "Seed.Core.Containers.Concurency.DynamicCircularBuffer.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class DynamicCircularBuffer final : public ITest {
						virtual bool Excecute() override;
					};

					class ConcurencyDynamicCircularBuffer final : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}