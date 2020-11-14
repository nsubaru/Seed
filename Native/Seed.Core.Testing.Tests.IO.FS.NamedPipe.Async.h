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
                namespace IO
                {
                    namespace NamedPipe
                    {
                        namespace Async {
                            class AsyncNamedPipeServerCreation final : public TestClass<AsyncNamedPipeServerCreation> {
                                virtual bool Excecute() override;
                            };

                            class AsyncNamedPipeComunication final : public TestClass<AsyncNamedPipeComunication> {
                                virtual bool Excecute() override;
                            };
                        }
                    }
                }
            }
        }
    }
}