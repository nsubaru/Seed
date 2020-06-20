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
                    namespace API {
                        class QueryPath final : public TestClass<QueryPath> {
                            virtual bool Excecute() override;
                        };
                    }
				}
			}
		}
	}
}