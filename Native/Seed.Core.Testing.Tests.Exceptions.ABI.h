#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			namespace Test {
				namespace Exceptions {
					class Abi final : public ITest {
						virtual bool Excecute() override;
					};

					class StoringException final : public ITest {
						virtual bool Excecute() override;
					};
				}
			}
		}
	}
}