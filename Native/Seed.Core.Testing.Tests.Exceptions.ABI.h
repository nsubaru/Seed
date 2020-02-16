#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Containers.NonConcurency.Array.h"
#include "Seed.Core.Containers.Concurency.Array.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Exceptions {
					class Abi final : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}