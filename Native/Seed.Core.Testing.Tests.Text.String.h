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

                namespace Comparing
                {
                    class EmptyStringEquals final : public TestClass<EmptyStringEquals> {
                        virtual bool Excecute() override;
                    };                    
                    
                    class SelfStringTestResult final : public TestClass<SelfStringTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithSameHeapTestResult final : public TestClass<StringWithSameHeapTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithOtherHeapTestResult final : public TestClass<StringWithOtherHeapTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithConstStringTestResult final : public TestClass<StringWithConstStringTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithConstAStringTestResult final : public TestClass<StringWithConstAStringTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithLiteralTestResult final : public TestClass<StringWithLiteralTestResult> {
                        virtual bool Excecute() override;
                    };
                    class StringWithAciiLiteralTestResult final : public TestClass<StringWithAciiLiteralTestResult> {
                        virtual bool Excecute() override;
                    };
                }

                namespace Ordering
                {
                    class ComparingEmptyStrings final : public TestClass<ComparingEmptyStrings> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithEmptyString final : public TestClass<ComparingWithEmptyString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingSameDataStrings final : public TestClass<ComparingSameDataStrings> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithSelf final : public TestClass<ComparingWithSelf> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithOtherHeapString final : public TestClass<ComparingWithOtherHeapString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithConstString final : public TestClass<ComparingWithConstString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithConstAString final : public TestClass<ComparingWithConstAString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLiteral final : public TestClass<ComparingWithLiteral> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithAsciiLiteral final : public TestClass<ComparingWithAsciiLiteral> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLowerString final : public TestClass<ComparingWithLowerString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLowerOtherHeapString final : public TestClass<ComparingWithLowerOtherHeapString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLowerConstString final : public TestClass<ComparingWithLowerConstString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLowerConstAString final : public TestClass<ComparingWithLowerConstAString> {
                        virtual bool Excecute() override;
                    };
                    class ComparingWithLowerLiteral final : public TestClass<ComparingWithLowerLiteral> {
                        virtual bool Excecute() override;
                    };
                    class CompatingWithLowerAciiLiteral final : public TestClass<CompatingWithLowerAciiLiteral> {
                        virtual bool Excecute() override;
                    };
                }
			}
		}
	}
}