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
			/// Сервіс системного пула задач
			/// </summary>
			class SystemTaskPoolService final : IDisposable, IBootUpService<SystemTaskPoolService> {
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto SystemTaskPoolServiceDefinition = ObjectDefinition(
						Services::SystemTaskPoolService,
						NullCtorDescription(Services::SystemTaskPoolService),
						ObjectMethodsDefs(),
						ObjectPropsDefs(
							ObjectProperty(Services::SystemTaskPoolService, JobsCount, GetJobsCount)
						)
					);

					return GenObjectRTTIRecord(SystemTaskPoolServiceDefinition);
				}
			private:
				struct Data {
					Threading::ThreadPool pool;

					RefCountT RefCount;

					Data() : pool(L"System task pool service"), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {}
			public:
				/// <summary>
				/// Конструктор, ініціалізовує пул задач
				/// </summary>
				SystemTaskPoolService() : This(new Data()) {};
			public:
				/// <summary>
				/// Занесення задачі в пул для виконання
				/// </summary>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="action">Дія</param>
				/// <returns>Об'єкт задачі</returns>
				Threading::JobPool PoolJob(const String& name, Delegate&& action) {
					return This->pool.Dispatch(name, MoveRValue(action));
				}

				/// <summary>
				/// Занесення задачі в пул для виконання
				/// </summary>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="action">Дія</param>
				/// <returns>Об'єкт задачі</returns>
				Threading::JobPool PoolJob(String&& name, Delegate&& action) {
					return This->pool.Dispatch(MoveRValue(name), MoveRValue(action));
				}
			public:
				/// <summary>
				/// Кількість задач в пулі на даний момент
				/// </summary>
				/// <returns>Кількість задач в пулі на даний момент</returns>
				UInt64 GetJobsCount() const {
					return This->pool.GetTasksCount();
				}
			public:
				RefCountClass(SystemTaskPoolService, This);
			};
		}
	}
}