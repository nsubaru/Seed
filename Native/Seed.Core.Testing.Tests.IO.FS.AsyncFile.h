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
                    namespace FS
                    {
                        namespace AsyncFile {
                            class CreateAsyncFile final : public TestClass<CreateAsyncFile> {
                                virtual bool Excecute() override;
                            };

                            class WriteAsyncFile final : public TestClass<WriteAsyncFile> {
                                virtual bool Excecute() override;
                            };

                            class ReadAsyncFile final : public TestClass<ReadAsyncFile> {
                                virtual bool Excecute() override;
                            };

                            class ReadWriteAsyncFile final : public TestClass<ReadWriteAsyncFile> {
                                virtual bool Excecute() override;
                            };

                            class TwoWritesAsyncFile final : public TestClass<TwoWritesAsyncFile> {
                                virtual bool Excecute() override;
                            };
                        }
                    }
                }
            }
        }
    }
}