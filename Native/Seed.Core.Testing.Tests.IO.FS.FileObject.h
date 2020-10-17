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
                        namespace API {
                            class CreateFile final : public TestClass<CreateFile> {
                                virtual bool Excecute() override;
                            };

                            class DeleteFile final : public TestClass<DeleteFile> {
                                virtual bool Excecute() override;
                            };

                            class OpenExist final : public TestClass<OpenExist> {
                                virtual bool Excecute() override;
                            };

                            class WriteFile final : public TestClass<WriteFile> {
                                virtual bool Excecute() override;
                            };

                            class ReadFile final : public TestClass<ReadFile> {
                                virtual bool Excecute() override;
                            };
                        }
                    }
                }
			}
		}
	}
}