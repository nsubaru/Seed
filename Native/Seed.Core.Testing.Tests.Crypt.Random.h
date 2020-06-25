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
                    class Random final : public TestClass<Random> {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}