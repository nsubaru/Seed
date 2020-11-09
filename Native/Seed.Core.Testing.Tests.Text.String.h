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

                    class StringSplitChar final : public TestClass<StringSplitChar> {
                        virtual bool Excecute() override;
                    };

                    class StringSplitCharLiteral final : public TestClass<StringSplitCharLiteral> {
                        virtual bool Excecute() override;
                    };

                    class StringSplitString final : public TestClass<StringSplitString> {
                        virtual bool Excecute() override;
                    };

                    class StringCtorFromConstArray final : public TestClass<StringCtorFromConstArray> {
                        virtual bool Excecute() override;
                    };

                    class StringLeftResize final : public TestClass<StringLeftResize>{
                        virtual bool Excecute() override;
                    };

                    class StringInt32Hex final : public TestClass<StringInt32Hex> {
                        virtual bool Excecute() override;
                    };

                    class StringUInt32Hex final : public TestClass<StringUInt32Hex> {
                        virtual bool Excecute() override;
                    };

                    class StringBlobHex final : public TestClass<StringBlobHex> {
                        virtual bool Excecute() override;
                    };
				}
			}
		}
	}
}