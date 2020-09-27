#pragma once
#include "Seed.Core.Testing.Base.h"
#include "Seed.Core.Testing.TestSuitesResult.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			/// <summary>
			/// Реєстр тестів
			/// </summary>
			class TestRegister {
			private:
				static Containers::List<TestRecord>& GetAllTests();
			public:
				/// <summary>
				/// Запуск тестів
				/// </summary>
				/// <param name = "isLogging"> Вказує чи необхідно записувати в логи інформаціб про перебіг тестування </param>
				/// <returns> Інформацію про результат тестів </returns>
				static TestPackResult RunAllTest(Boolean isLogging = false);
			public:
				/// <summary>
				/// Вертає список завантажених тестів
				/// </summary>
				/// <returns></returns>
				static inline Containers::List<String> LoadedTests() {
					return GetAllTests().CopyTo<Containers::List>([] (const TestRecord& record) InlineLambda
					{
						return record.Key.TypeName;
					});
				}
			};
		}
	}
}