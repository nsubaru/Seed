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
                namespace DI
                {
                    class RegisterService final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class CallServiceMethodByNameAndArgs : public ITest {
                        virtual bool Excecute() override;
                    };

                    class DataTypeInformation : public ITest {
                        virtual bool Excecute() override;
                    };

                    class ResolveServiceByDI : public ITest {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}