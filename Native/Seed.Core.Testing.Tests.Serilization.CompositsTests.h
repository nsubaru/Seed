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
                    class SimpleTypeTupleSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class ComplexTypeTupleSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class TupleTypeTupleSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };

					class CustomStructSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}