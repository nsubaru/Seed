#pragma once
#include "Seed.Core.Testing.TestSuitesResult.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			class SeedKernelCall ITest {
			public:
				virtual ~ITest() = default;
				virtual bool Excecute() = 0;
			};

			class SeedKernelCall TestRegister {
			private:
				struct TestRecord {
					String name;
					String testClassName;
					const TypeInfo & testClassInfo;
					ITest* testObj;

					TestRecord(const String& name, String&& testClassName, const TypeInfo& testClassInfo, ITest*&& testObj) : 
						name(name), 
						testClassName(MoveRValue(testClassName)), 
						testClassInfo(testClassInfo),
						testObj(MoveRValue(testObj)) 
					{}

					TestRecord(String&& name, String&& testClassName, const TypeInfo& testClassInfo, ITest*&& testObj) : 
						name(MoveRValue(name)), 
						testClassName(MoveRValue(testClassName)), 
						testClassInfo(testClassInfo), 
						testObj(MoveRValue(testObj)) 
					{}
				};
				static inline Containers::ConcurencyList<TestRecord> * tests;

				friend bool InitTestRegister();
				friend void StopTestRegister();
			public:
				template<typename T>
				static void Register(T*&& test, const String& testName) {
					tests->AddLast(TestRecord(testName, typeid(T), MoveRValue((ITest*&&)test)));
				}

				template<typename T>
				static void Register(T*&& test, String&& testName) {
					static_assert(TypeTraits::IsBasedOnV<ITest, T>, L"Invalid test type passed");
					tests->AddLast(TestRecord(MoveRValue(testName), NewCTTI::TypeName<T>(),  typeid(T), MoveRValue((ITest*&&)test)));
				}

				template<typename T>
				static void UnRegister() {
					tests->RemoveIf([](const TestRecord& record) {return record.testClassName == typeof(T); }, [](TestRecord&& record) {delete record.testObj; })
				}

				static TestPackResult RunAllTest() {
					Containers::List<TestResult> results;
					toCoreLog(L"Запуск тестів...");
					DateTime startTime = DateTime::GetCurrentSystemTimeData();
					tests->ForEach([&](const TestRecord& record) {
						DateTime testDurration = DateTime::GetCurrentSystemTimeData();
						try {
							try {
								if (record.testObj->Excecute()) {
									toCoreLog(L"Тест \"" + record.name + L"\" успішно пройдено");
									testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
									results += {record.testClassName, TestResult::EStatus::Success, "", testDurration};
								}
								else {
									toCoreLog(L"Тест \"" + record.name + L"\" не пройдено");
									testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
									results += {record.testClassName, TestResult::EStatus::Failure, L"Вернув false", testDurration};
								}
							}
							catch (Exception & ex) {
								toCoreLog(L"При обробці тесту " + record.name + L" виникла помилка ", ex);
								testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
								results += {record.testClassName, TestResult::EStatus::Exception, ex.toString(), testDurration};
							}
						}
						catch (...) {
							toCoreLog(L"При обробці тесту " + record.name + L" виникла непередбачувана помилка");
							testDurration = DateTime::GetCurrentSystemTimeData() - testDurration;
							results += {record.testClassName, TestResult::EStatus::Exception, L"Uknown", testDurration};
						}
						});
					DateTime endTime = DateTime::GetCurrentSystemTimeData();
					return TestPackResult(String(L"Тести ядра"), results, startTime, endTime);
				}

				static void StoreTestResultInNUnitReport(const TestPackResult& results);
			};
		}
	}
}