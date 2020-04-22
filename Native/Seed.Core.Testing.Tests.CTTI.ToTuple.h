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
                namespace CTTI
                {
                    class ToTuple final : public ITest {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}