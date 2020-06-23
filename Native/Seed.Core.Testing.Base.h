#pragma once
#include "Seed.Core.Containers.h"
#include "Seed.Core.String.h"
#include "Seed.Core.CTTI.h"

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
                friend void LoadTest(String&& name, ITest*&& instance);
            public:
                virtual ~BaseTest() = 0 {}
            };

            /// <summary>
            /// Класс тесту
            /// </summary>
            template<typename TTest>
            class TestClass : BaseTest, public ITest {
            private:
                static bool GenTestRecord() {
                    LoadTest(Core::CTTI::TypeName<TTest>(), (ITest*)new TTest());
                    return true;
                }
            private:
                inline static volatile bool Result = GenTestRecord();
            public:
                virtual ~TestClass() {}
            };

            /// <summary>
            /// Запис про тест
            /// </summary>
            namespace
            {
                using TestRecord = Containers::Pair<String, ITest*>;
            }
		}
	}
}
