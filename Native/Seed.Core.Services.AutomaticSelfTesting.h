#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.BootupServicesManager.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Services
		{
			/// <summary>
			/// Сервіс автоматичного регулярного тестування
			/// </summary>
			class SeedKernelCall AutomatiSelfTestingService final : IDisposable, IBootUpService<AutomatiSelfTestingService> {
			public:
				template<typename TService, typename ... TCtorArgs>
				friend class IBootupService;
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto AutomatiSelfTestingServiceDefinition = ObjectDefinition(
						Services::AutomatiSelfTestingService,
						NullCtorDescription(Services::AutomatiSelfTestingService),
						ObjectMethodsDefs(
							MethodDescription(Services::AutomatiSelfTestingService, RunTestsNow)
						),
						ObjectPropsDefs(
							ObjectProperty(Services::AutomatiSelfTestingService, LastTestResult, GetLastTestResult),
							ObjectProperty(Services::AutomatiSelfTestingService, TestRunPeriod, GetTestRunPeriod),
							ObjectProperty(Services::AutomatiSelfTestingService, SavedTestsResults, GetSavedTestsResults),
							ObjectProperty(Services::AutomatiSelfTestingService, SavedTestsStartAndDurationTime, GetSavedTestsStartAndDurationTime)
						)
					);

					return GenObjectRTTIRecord(AutomatiSelfTestingServiceDefinition);
				}
			private:
				struct Data
				{
					Threading::TaskSheduler* sheduler;
					Threading::SheduledJob* testRunnerJob;
					Containers::ConcurencyList<Testing::TestPackResult> testsResults;
					Threading::SRWLock testRunLock;

					RefCountT RefCount;

					Data() :
						sheduler(nullptr),
						testRunnerJob(nullptr),
						testsResults(),
						testRunLock(),
						RefCount(1) {}
				};
			private:
				class AutomaticTestingTask final : public Threading::ITask {
					Data* This;
				private:
					AutomaticTestingTask(Data* This) : This(This) {}
				public:
					virtual void Execute(Time::DateTime startTimeUtc) volatile;
				public:
					virtual ~AutomaticTestingTask() {}
				public:
					friend class AutomatiSelfTestingService;
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override;
			public:
				/// <summary>
				/// Конструктор, ініціалізовує автоматичне регулярне тестування
				/// </summary>
				AutomatiSelfTestingService();
			public:
				/// <summary>
				/// Вертає інформацію про останнє тестування
				/// </summary>
				/// <returns></returns>
				Testing::TestPackResult GetLastTestResult() const {
					return *This->testsResults.LastIt;
				}

				/// <summary>
				/// Вертає тривалість останнього тестування
				/// </summary>
				/// <returns></returns>
				Time::TimeSpan GetTestRunPeriod() const {
					return This->testsResults.LastIt->Duration;
				}

				/// <summary>
				/// Вертає інформацію про збереженні результати тестування
				/// </summary>
				/// <returns></returns>
				Containers::List<Testing::TestPackResult> GetSavedTestsResults() const {
					return This->testsResults.CopyTo<Containers::List>();
				}

				/// <summary>
				/// Вертає інформацію про збереженні тривалості тестування
				/// </summary>
				/// <returns></returns>
				Containers::List<Time::TimeRange> GetSavedTestsStartAndDurationTime() const {
					return This->testsResults.CopyTo<Containers::List>(ArrowFunct(x, x.TestTimeRange));
				}
			public:
				/// <summary>
				/// Запуск тестів
				/// </summary>
				/// <param name = "isLogging"> Вказує чи необхідно записувати в логи інформаціб про перебіг тестування </param>
				/// <returns> Інформацію про результат тестів </returns>
				Testing::TestPackResult RunTestsNow(Boolean isLogging = false);
			public:
				RefCountClass(AutomatiSelfTestingService, This);
			};
		}
	}
}