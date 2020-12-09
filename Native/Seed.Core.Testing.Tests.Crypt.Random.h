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
                namespace Crypt
                {
                    namespace Random
                    {
                        class RandomTest final : public TestClass<RandomTest> {
                            virtual bool Excecute() override;
                        };
                    }
                }
            }
        }
    }
}