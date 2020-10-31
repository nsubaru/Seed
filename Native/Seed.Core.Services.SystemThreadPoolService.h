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
			class SystemThreadPoolService final : IBootUpService<SystemThreadPoolService> {
			public:
				/// <summary>
				/// Схема сервісу
				/// </summary>
				/// <returns>Рантайм опис типу сервіса</returns>
				static auto GetRttiInformation() {
					constexpr auto SystemThreadPoolServiceDefinition = ObjectDefinition(
						Services::SystemThreadPoolService,
						NullCtorDescription(Services::SystemThreadPoolService),
						ObjectMethodsDefs(),
						ObjectPropsDefs(
							ObjectProperty(Services::SystemThreadPoolService, AllItemCount, GetAllItemCount),

							ObjectProperty(Services::SystemThreadPoolService, AllItemLowPriorityCount, GetAllItemLowPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, AllItemMediumPriorityCount, GetAllItemMediumPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, AllItemHighPriorityCount, GetAllItemHighPriorityCount),

							ObjectProperty(Services::SystemThreadPoolService, JobCount, GetJobCount),

							ObjectProperty(Services::SystemThreadPoolService, JobLowPriorityCount, GetJobLowPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, JobMediumPriorityCount, GetJobMediumPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, JobHighPriorityCount, GetJobHighPriorityCount),

							ObjectProperty(Services::SystemThreadPoolService, ScheduledTaskCount, GetScheduledTaskCount),

							ObjectProperty(Services::SystemThreadPoolService, ScheduledTaskLowPriorityCount, GetScheduledTaskLowPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, ScheduledTaskMediumPriorityCount, GetScheduledTaskMediumPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, ScheduledTaskHighPriorityCount, GetScheduledTaskHighPriorityCount),

							ObjectProperty(Services::SystemThreadPoolService, IoTaskCount, GetIoTaskCount),

							ObjectProperty(Services::SystemThreadPoolService, IoTaskLowPriorityCount, GetIoTaskLowPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, IoTaskMediumPriorityCount, GetIoTaskMediumPriorityCount),
							ObjectProperty(Services::SystemThreadPoolService, IoTaskHighPriorityCount, GetIoTaskHighPriorityCount)
						)
					);

					return GenObjectRTTIRecord(SystemThreadPoolServiceDefinition);
				}
			private:
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				SystemThreadPoolService() {};
			public:
#pragma region Jobs
				/// <summary>
				/// Запуск задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				ForceInline Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), MoveRValue(name), priority);
				}

				/// <summary>
				/// Запуск задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				ForceInline Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), name, priority);
				}

				/// <summary>
				/// Запуск безіменої задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				ForceInline Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), priority);
				}
#pragma endregion
#pragma region Sheduled task
				/// <summary>
				/// Запуск таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="name">Ім'я таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				ForceInline Threading::UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), MoveRValue(name), priority);
				}

				/// <summary>
				/// Запуск таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="name">Ім'я таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				ForceInline Threading::UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), name, priority);
				}

				/// <summary>
				/// Запуск безіменого таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				ForceInline Threading::UnifiedThreadPoolSheduledTaskBuilder CreateLowPriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), priority);
				}
#pragma endregion
#pragma region Statistic
				/// <summary>
				/// Вертає загальну кількість об'єктів в системному пулі задач
				/// </summary>
				/// <returns>Загальна кількість об'єктів в системному пулі задач</returns>
				SizeT GetAllItemCount() const { return Threading::SystemThreadPool::AllItemCount(); }

				/// <summary>
				/// Вертає кількість об'єктів з низьким пріоритетом
				/// </summary>
				/// <returns>Кількість об'єктів з низьким пріоритетом</returns>
				SizeT GetAllItemLowPriorityCount() const { return Threading::SystemThreadPool::AllItemSpecificPriorityCount(Threading::EPoolThreadsPriority::Low); }

				/// <summary>
				/// Вертає кількість об'єктів з середнім пріоритетом
				/// </summary>
				/// <returns>Кількість об'єктів з середнім пріоритетом</returns>
				SizeT GetAllItemMediumPriorityCount()  const { return Threading::SystemThreadPool::AllItemSpecificPriorityCount(Threading::EPoolThreadsPriority::Medium); }

				/// <summary>
				/// Вертає кількість об'єктів з високим пріоритетом
				/// </summary>
				/// <returns>Кількість об'єктів з високим пріоритетом</returns>
				SizeT GetAllItemHighPriorityCount() const { return Threading::SystemThreadPool::AllItemSpecificPriorityCount(Threading::EPoolThreadsPriority::High); }



				/// <summary>
				/// Вертає кількість задач в системному пулі
				/// </summary>
				/// <returns>Кількість задач в системному пулі</returns>
				SizeT GetJobCount() const { return Threading::SystemThreadPool::JobCount(); }

				/// <summary>
				/// Вертає кількість задач з низьким пріоритетом
				/// </summary>
				/// <returns>Кількість задач з низьким пріоритетом</returns>
				SizeT GetJobLowPriorityCount() const { return Threading::SystemThreadPool::JobSpecificPriorityCount(Threading::EPoolThreadsPriority::Low); }

				/// <summary>
				/// Вертає кількість задач з середнім пріоритетом
				/// </summary>
				/// <returns>Кількість задач з середнім пріоритетом</returns>
				SizeT GetJobMediumPriorityCount() const { return Threading::SystemThreadPool::JobSpecificPriorityCount(Threading::EPoolThreadsPriority::Medium); }

				/// <summary>
				/// Вертає кількість задач з високим пріоритетом
				/// </summary>
				/// <returns>Кількість задач з високим пріоритетом</returns>
				SizeT GetJobHighPriorityCount() const { return Threading::SystemThreadPool::JobSpecificPriorityCount(Threading::EPoolThreadsPriority::High); }



				/// <summary>
				/// Вертає кількість таймерів в системному пулі
				/// </summary>
				/// <returns>Кількість таймерів в системному пулі</returns>
				SizeT GetScheduledTaskCount() const { return Threading::SystemThreadPool::ScheduledTaskCount(); }

				/// <summary>
				/// Вертає кількість таймерів з низьким пріоритетом
				/// </summary>
				/// <returns>Кількість таймерів з низьким пріоритетом</returns>
				SizeT GetScheduledTaskLowPriorityCount() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::Low); }
				
				/// <summary>
				/// Вертає кількість таймерів з середнім пріоритетом
				/// </summary>
				/// <returns>Кількість таймерів з середнім пріоритетом</returns>
				SizeT GetScheduledTaskMediumPriorityCount() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::Medium); }
				
				/// <summary>
				/// Вертає кількість таймерів з високим пріоритетом
				/// </summary>
				/// <returns>Кількість таймерів з високим пріоритетом</returns>
				SizeT GetScheduledTaskHighPriorityCount() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::High); }



				/// <summary>
				/// Вертає кількість файлових об'єктів в системному пулі
				/// </summary>
				/// <returns>Кількість файлових об'єктів в системному пулі</returns>
				SizeT GetIoTaskCount() const { return Threading::SystemThreadPool::IoTaskCount(); }

				/// <summary>
				/// Вертає кількість файлових об'єктів в системному пулі з низьким пріоритетом
				/// </summary>
				/// <returns>Кількість файлових об'єктів в системному пулі з низьким пріоритетом</returns>
				SizeT GetIoTaskLowPriorityCount() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::Low); }
				
				/// <summary>
				/// Вертає кількість файлових об'єктів в системному пулі з середнім пріоритетом
				/// </summary>
				/// <returns>Кількість файлових об'єктів в системному пулі з середнім пріоритетом</returns>
				SizeT GetIoTaskMediumPriorityCount() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::Medium); }
				
				/// <summary>
				/// Вертає кількість файлових об'єктів з високим пріоритетом з високим пріоритетом
				/// </summary>
				/// <returns>Кількість файлових об'єктів з високим пріоритетом з високим пріоритетом</returns>
				SizeT GetIoTaskHighPriorityCount() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityCount(Threading::EPoolThreadsPriority::High); }
#pragma region Properties
				/// <summary>
				/// Загальна кількість об'єктів в системному пулі задач
				/// </summary>
				SizeT Property(GetAllItemCount) AllItemCount;


				/// <summary>
				/// Кількість об'єктів з низьким пріоритетом
				/// </summary>
				SizeT Property(GetAllItemLowPriorityCount) AllItemLowPriorityCount;

				/// <summary>
				/// Кількість об'єктів з середнім пріоритетом
				/// </summary>
				SizeT Property(GetAllItemMediumPriorityCount) AllItemMediumPriorityCount;

				/// <summary>
				/// Кількість об'єктів з високим пріоритетом
				/// </summary>
				SizeT Property(GetAllItemHighPriorityCount) AllItemHighPriorityCount;


				/// <summary>
				/// Кількість задач в системному пулі
				/// </summary>
				SizeT Property(GetJobCount) JobCount;

				/// <summary>
				/// Кількість задач в системному пулі з низьким пріоритетом
				/// </summary>
				SizeT Property(GetJobLowPriorityCount) JobLowPriorityCount;

				/// <summary>
				/// Кількість задач в системному пулі з середнім пріоритетом
				/// </summary>
				SizeT Property(GetJobMediumPriorityCount) JobMediumPriorityCount;

				/// <summary>
				/// Кількість задач в системному пулі з високим пріоритетом
				/// </summary>
				SizeT Property(GetJobHighPriorityCount) JobHighPriorityCount;


				/// <summary>
				/// Кількість таймерів в системному пулі
				/// </summary>
				SizeT Property(GetScheduledTaskCount) ScheduledTaskCount;

				/// <summary>
				/// Кількість таймерів в системному пулі з низьким пріоритетом
				/// </summary>
				SizeT Property(GetScheduledTaskLowPriorityCount) ScheduledTaskLowPriorityCount;

				/// <summary>
				/// Кількість таймерів в системному пулі з середнім пріоритетом
				/// </summary>
				SizeT Property(GetScheduledTaskMediumPriorityCount) ScheduledTaskMediumPriorityCount;

				/// <summary>
				/// Кількість таймерів в системному пулі з високим пріоритетом
				/// </summary>
				SizeT Property(GetScheduledTaskHighPriorityCount) ScheduledTaskHighPriorityCount;



				/// <summary>
				/// Кількість файлових об'єктів в системному пулі
				/// </summary>
				SizeT Property(GetIoTaskCount) IoTaskCount;

				/// <summary>
				/// Кількість файлових об'єктів в системному пулі з низьким пріоритетом
				/// </summary>
				SizeT Property(GetIoTaskLowPriorityCount) IoTaskLowPriorityCount;

				/// <summary>
				/// Кількість файлових об'єктів в системному пулі з середнім пріоритетом
				/// </summary>
				SizeT Property(GetIoTaskMediumPriorityCount) IoTaskMediumPriorityCount;

				/// <summary>
				/// Кількість файлових об'єктів в системному пулі з високим пріоритетом
				/// </summary>
				SizeT Property(GetIoTaskHighPriorityCount) IoTaskHighPriorityCount;
#pragma endregion
#pragma endregion
			};
		}
	}
}