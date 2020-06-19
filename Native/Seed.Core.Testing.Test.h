#pragma once
#include "Seed.Core.Testing.Base.h"

namespace Seed
{
    inline namespace Core
    {
        namespace Testing
        {
            class BaseTest {
            protected:
                friend void LoadTest(String&& name, ITest*&& instance);
            public:
                virtual ~BaseTest() = 0 {}
            };

            template<typename TTest>
            class TestClass : BaseTest, public ITest {
            private:
                static bool GenTestRecord() {
                    LoadTest(CTTI::TypeName<TTest>(), (ITest*)new TTest());
                    return true;
                }
            private:
                inline static bool Result = GenTestRecord();
            public:
                virtual ~TestClass() {}
            };
        }
    }
}