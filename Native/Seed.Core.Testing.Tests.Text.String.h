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
                    class StringCtor final : public TestClass<StringCtor> {
                        virtual bool Excecute() override;
                    };

                    class StringComparing final : public TestClass<StringComparing> {
                        virtual bool Excecute() override;
                    };

                    class StringOrdering final : public TestClass<StringOrdering> {
                        virtual bool Excecute() override;
                    };

                    class StringContactanation final : public TestClass<StringContactanation> {
                        virtual bool Excecute() override;
                    };

                    class StringDoubleFloatConvert final : public TestClass<StringDoubleFloatConvert> {
                        virtual bool Excecute() override;
                    };

                    class StringIntConvert final : public TestClass<StringIntConvert> {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}