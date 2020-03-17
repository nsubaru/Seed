#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Containers {
					class FixedCircularBuffer final : public ITest {
						virtual bool Excecute() override;
					};

					class ConcurencyFixedCircularBuffer final : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}