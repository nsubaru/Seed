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
                namespace Text
                {
                    class StringCtor final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class StringComparing final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class StringOrdering final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class StringContactanation final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class StringDoubleFloatConvert final : public ITest {
                        virtual bool Excecute() override;
                    };

                    class StringIntConvert final : public ITest {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}