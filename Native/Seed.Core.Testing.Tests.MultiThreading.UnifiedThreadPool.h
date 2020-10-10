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
                namespace MultiThreading
                {
                    namespace UnifiedThreadPool {
                        class CreateEmptyUnifiedThreadPool final : public TestClass<CreateEmptyUnifiedThreadPool> {
                            virtual bool Excecute() override;
                        };

                        class CreateUnifiedThreadPoolWithjobs final : public TestClass<CreateUnifiedThreadPoolWithjobs> {
                            virtual bool Excecute() override;
                        };
                    }
                }
            }
        }
    }
}