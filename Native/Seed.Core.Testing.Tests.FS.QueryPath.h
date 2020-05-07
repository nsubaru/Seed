#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Testing
        {
            namespace Test
            {
                namespace FS
                {
                    class QueryPath final : public ITest {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}