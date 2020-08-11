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
                    class StructSerilization final : public TestClass<StructSerilization> {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}