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
			class SystemTaskPoolService final : IBootUpService<SystemTaskPoolService> {
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
							ObjectProperty(Services::SystemTaskPoolService, AllItemQueueLenght, GetAllItemQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, AllItemLowPriorityQueueLenght, GetAllItemLowPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, AllItemMediumPriorityQueueLenght, GetAllItemMediumPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, AllItemHighPriorityQueueLenght, GetAllItemHighPriorityQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, JobQueueLenght, GetJobQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, JobLowPriorityQueueLenght, GetJobLowPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, JobMediumPriorityQueueLenght, GetJobMediumPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, JobHighPriorityQueueLenght, GetJobHighPriorityQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, ScheduledTaskQueueLenght, GetScheduledTaskQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, ScheduledTaskLowPriorityQueueLenght, GetScheduledTaskLowPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, ScheduledTaskMediumPriorityQueueLenght, GetScheduledTaskMediumPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, ScheduledTaskHighPriorityQueueLenght, GetScheduledTaskHighPriorityQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, IoTaskQueueLenght, GetIoTaskQueueLenght),

							ObjectProperty(Services::SystemTaskPoolService, IoTaskLowPriorityQueueLenght, GetIoTaskLowPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, IoTaskMediumPriorityQueueLenght, GetIoTaskMediumPriorityQueueLenght),
							ObjectProperty(Services::SystemTaskPoolService, IoTaskHighPriorityQueueLenght, GetIoTaskHighPriorityQueueLenght)
						)
					);

					return GenObjectRTTIRecord(SystemTaskPoolServiceDefinition);
				}
			private:
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				SystemTaskPoolService() {};
			public:
#pragma region Jobs
				Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), MoveRValue(name), priority);
				}
				Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), name, priority);
				}
				Threading::UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::PostPriorityJob(MoveRValue(action), priority);
				}
#pragma endregion
#pragma region Sheduled task
				Threading::UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), MoveRValue(name), priority);
				}
				Threading::UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), name, priority);
				}
				Threading::UnifiedThreadPoolSheduledTaskBuilder CreateLowPriorityScheduledTask(SmartPtr<Threading::IUnifiedThreadPoolShedulerCallback>&& callback, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium) {
					return Threading::SystemThreadPool::CreatePriorityScheduledTask(MoveRValue(callback), priority);
				}
#pragma endregion
#pragma region Statistic
				SizeT GetAllItemQueueLenght() const { return Threading::SystemThreadPool::AllItemQueueLenght(); }

				SizeT GetAllItemLowPriorityQueueLenght() const { return Threading::SystemThreadPool::AllItemSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Low); }
				SizeT GetAllItemMediumPriorityQueueLenght()  const { return Threading::SystemThreadPool::AllItemSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Medium); }
				SizeT GetAllItemHighPriorityQueueLenght() const { return Threading::SystemThreadPool::AllItemSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::High); }

				SizeT GetJobQueueLenght() const { return Threading::SystemThreadPool::JobQueueLenght(); }

				SizeT GetJobLowPriorityQueueLenght() const { return Threading::SystemThreadPool::JobSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Low); }
				SizeT GetJobMediumPriorityQueueLenght() const { return Threading::SystemThreadPool::JobSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Medium); }
				SizeT GetJobHighPriorityQueueLenght() const { return Threading::SystemThreadPool::JobSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::High); }

				SizeT GetScheduledTaskQueueLenght() const { return Threading::SystemThreadPool::ScheduledTaskQueueLenght(); }

				SizeT GetScheduledTaskLowPriorityQueueLenght() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Low); }
				SizeT GetScheduledTaskMediumPriorityQueueLenght() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Medium); }
				SizeT GetScheduledTaskHighPriorityQueueLenght() const { return Threading::SystemThreadPool::ScheduledTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::High); }

				SizeT GetIoTaskQueueLenght() const { return Threading::SystemThreadPool::IoTaskQueueLenght(); }

				SizeT GetIoTaskLowPriorityQueueLenght() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Low); }
				SizeT GetIoTaskMediumPriorityQueueLenght() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::Medium); }
				SizeT GetIoTaskHighPriorityQueueLenght() const { return Threading::SystemThreadPool::IoTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority::High); }
#pragma region Properties
				SizeT Property(GetAllItemQueueLenght) AllItemQueueLenght;

				SizeT Property(GetAllItemLowPriorityQueueLenght) AllItemLowPriorityQueueLenght;
				SizeT Property(GetAllItemMediumPriorityQueueLenght) AllItemMediumPriorityQueueLenght;
				SizeT Property(GetAllItemHighPriorityQueueLenght) AllItemHighPriorityQueueLenght;

				SizeT Property(GetJobQueueLenght) JobQueueLenght;

				SizeT Property(GetJobLowPriorityQueueLenght) JobLowPriorityQueueLenght;
				SizeT Property(GetJobMediumPriorityQueueLenght) JobMediumPriorityQueueLenght;
				SizeT Property(GetJobHighPriorityQueueLenght) JobHighPriorityQueueLenght;

				SizeT Property(GetScheduledTaskQueueLenght) ScheduledTaskQueueLenght;

				SizeT Property(GetScheduledTaskLowPriorityQueueLenght) ScheduledTaskLowPriorityQueueLenght;
				SizeT Property(GetScheduledTaskMediumPriorityQueueLenght) ScheduledTaskMediumPriorityQueueLenght;
				SizeT Property(GetScheduledTaskHighPriorityQueueLenght) ScheduledTaskHighPriorityQueueLenght;

				SizeT Property(GetIoTaskQueueLenght) IoTaskQueueLenght;

				SizeT Property(GetIoTaskLowPriorityQueueLenght) IoTaskLowPriorityQueueLenght;
				SizeT Property(GetIoTaskMediumPriorityQueueLenght) IoTaskMediumPriorityQueueLenght;
				SizeT Property(GetIoTaskHighPriorityQueueLenght) IoTaskHighPriorityQueueLenght;
#pragma endregion
#pragma endregion
			};
		}
	}
}