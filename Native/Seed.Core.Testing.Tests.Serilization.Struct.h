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
                    class StructSerilization final : public ITest {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}