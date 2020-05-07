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
                namespace FS
                {
                    class CreateFile final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class DeleteFile final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class OpenExist final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class WriteFile final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class ReadFile final : public ITest {
                        virtual bool Excecute() override;
                    };

                    //class WriteUsingTextProvider final : public ITest {
                    //    virtual bool Excecute() override;
                    //};

                    //class ReadUsingTextProvider final : public ITest {
                    //    virtual bool Excecute() override;
                    //};
				}
			}
		}
	}
}