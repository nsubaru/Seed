#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Serilization.h"

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
                    class ComplexMetaDataGen final : public TestClass<ComplexMetaDataGen> {
                        virtual bool Excecute() override;
                    };

					class CustomStructMetaDataGen final : public TestClass<CustomStructMetaDataGen> {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}