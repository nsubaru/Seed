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
                        namespace Sync {
                            class SyncNamedPipeServerCreation final : public TestClass<SyncNamedPipeServerCreation> {
                                virtual bool Excecute() override;
                            };

                            class SyncNamedPipeComunication final : public TestClass<SyncNamedPipeComunication> {
                                virtual bool Excecute() override;
                            };
                        }
                    }
                }
            }
        }
    }
}