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
                    namespace Hashing
                    {
                        class Sha256HashTest1 final : public TestClass<Sha256HashTest1> {
                            virtual bool Excecute() override;
                        };

                        class Sha256HashTest2 final : public TestClass<Sha256HashTest2> {
                            virtual bool Excecute() override;
                        };
                    }
                }
            }
        }
    }
}