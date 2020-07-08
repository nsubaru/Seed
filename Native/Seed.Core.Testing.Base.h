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
            /// Базовий класс тесту
            /// </summary>
            class BaseTest {
            protected:
                friend SizeT LoadTest(RTTI::RTTITypeRecord&& typeRecord, ITest&(*testObjGenerator)());
            public:
                virtual ~BaseTest() = 0 {}
            };

            /// <summary>
            /// Класс тесту
            /// </summary>
            template<typename TTest>
            class TestClass : BaseTest, public ITest {
            private:
                static volatile void* volatile GenTestRecord() {
                    return (volatile void*) LoadTest(TypeRecord<TTest>(), [] () -> ITest&
                    {
                        static auto test = TTest();
                        return test;
                    });
                }
            private:
#pragma section(".TEST$a",read)
                [[maybe_unused]] __declspec(allocate(".TEST$a")) inline static volatile void* volatile Result = GenTestRecord();
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
