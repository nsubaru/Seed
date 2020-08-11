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
				/// <returns>Інформацію про результат тестів</returns>
				static TestPackResult RunAllTest();
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