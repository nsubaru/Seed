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
                namespace Serilization
                {
                    class SimpleTypeTupleSerilization final : public TestClass<SimpleTypeTupleSerilization> {
                        virtual bool Excecute() override;
                    };

					class ComplexTypeTupleSerilization final : public TestClass<ComplexTypeTupleSerilization> {
                        virtual bool Excecute() override;
                    };

					class TupleTypeTupleSerilization final : public TestClass<TupleTypeTupleSerilization> {
                        virtual bool Excecute() override;
                    };

					class CustomStructSerilization final : public TestClass<CustomStructSerilization> {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}