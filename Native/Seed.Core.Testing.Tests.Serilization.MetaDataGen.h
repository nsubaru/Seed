#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.NewSerilization.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Testing
        {
            namespace Test
            {
                namespace Serilization
                {
                    class ComplexMetaDataGen final : public ITest {
                        virtual bool Excecute() override;
                    };

					class CustomStructMetaDataGen final : public ITest {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}