#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.String.h"
#include "Seed.Core.CTTI.h"

#include "Seed.Core.Testing.Asserts.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			class ITest {
			public:
				virtual ~ITest() = default;
				virtual bool Excecute() = 0;
			};

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
                    LoadTest(Core::CTTI::TypeName<TTest>(), (ITest*)new TTest());
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
