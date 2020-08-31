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
                    class LazySerilization final : public TestClass<LazySerilization> {
                        virtual bool Excecute() override;
                    };

                    class SmartPtrSerilization final : public TestClass<SmartPtrSerilization> {
                        virtual bool Excecute() override;
                    };
                }
            }
        }
    }
}