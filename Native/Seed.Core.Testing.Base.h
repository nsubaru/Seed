#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.String.h"

#include "Seed.Core.Testing.Asserts.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			class ITest {
			public:
				virtual ~ITest() = default;
				virtual bool Excecute() = 0;
			};
		}
	}
}
