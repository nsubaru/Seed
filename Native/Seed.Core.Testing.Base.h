#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Containers.Concurency.List.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

namespace Seed {
	inline namespace Core {
		namespace Testing {
			interface ITest {
				bool Excecute();
			};
			class TestRegister {
			private:
				struct TestRecord {
					String name;
					const TypeInfo & testClassName;
					ITest* testObj;

					TestRecord(const String& name, const TypeInfo& testClassName, ITest*&& testObj) : name(name), testClassName(testClassName), testObj(MoveRValue(testObj)) {}
					TestRecord(String&& name, const TypeInfo& testClassName, ITest*&& testObj) : name(MoveRValue(name)), testClassName(testClassName), testObj(MoveRValue(testObj)) {}
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
					tests->AddLast(TestRecord(MoveRValue(testName), typeid(T), MoveRValue((ITest*&&)test)));
				}

				template<typename T>
				static void UnRegister() {
					tests->RemoveIf([](const TestRecord& record) {return record.testClassName == typeof(T); }, [](TestRecord&& record) {delete record.testObj; })
				}

				static Boolean RunAllTest() {
					Boolean result = true;
					tests->ForEach([&](const TestRecord& record) {
						try {
							try {
								if (record.testObj->Excecute()) {
									toCoreLog(L"Тест \"" + record.name + L"\" успішно пройдено");
								}
								else {
									toCoreLog(L"Тест \"" + record.name + L"\" не пройдено");
									result = false;
								}
							}
							catch (Exception & ex) {
								toCoreLog(L"При обробці тесту " + record.name + L" виникла помилка ", ex);
							}
						}
						catch (...) {
							toCoreLog(L"При обробці тесту " + record.name + L" виникла непередбачувана помилка");
						}
						});
					return result;
				}
			};
		}
	}
}