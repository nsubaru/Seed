#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.String.h"
#include "Seed.Core.CTTI.h"
#include "Seed.Core.RTTI.h"

#include "Seed.Core.Testing.Asserts.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
            /// <summary>
            /// Інтерфейс тесту
            /// </summary>
            interface ITest {
                /// <summary>
                /// Виконує тест
                /// </summary>
                /// <returns>якщо true тест пройшов успішно</returns>
                bool Excecute() = 0;
            };

            /// <summary>
            /// Класс тесту
            /// </summary>
            template<typename TTest>
            class TestClass : public ITest {
            private:
                friend SizeT LoadTest(RTTI::RTTITypeRecord&& typeRecord, ITest&(*testObjGenerator)());
            private:
                static volatile void* volatile GenTestRecord() {
                    return (volatile void*) LoadTest(TypeRecord<TTest>(), [] () -> ITest& InlineLambda
                    {
                        static auto test = TTest();
                        return test;
                    });
                }
            private:
#pragma section(".TEST$a",read, write)
                [[maybe_unused]] __declspec(allocate(".TEST$a")) inline static volatile void* volatile Result = GenTestRecord();
#pragma comment(linker, "/MERGE:.data=.TEST") 
            public:
                virtual ~TestClass() {}
            };

            /// <summary>
            /// Запис про тест
            /// </summary>
            namespace
            {
                using TestRecord = Containers::Pair<Core::RTTI::RTTITypeRecord, ITest&(*)()>;
            }
		}
	}
}
