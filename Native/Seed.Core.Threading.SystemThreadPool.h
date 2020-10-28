#pragma once
#include "Seed.Core.Threading.UnifiedThreadPool.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace SystemThreadPool {
#pragma region Jobs
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion
#pragma region Sheduled task
				UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				UnifiedThreadPoolSheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion
#pragma region Statistic
				SizeT AllItemQueueLenght();

				SizeT AllItemSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				SizeT JobQueueLenght();

				SizeT JobSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				SizeT ScheduledTaskQueueLenght();

				SizeT ScheduledTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				SizeT IoTaskQueueLenght();

				SizeT IoTaskSpecificPriorityQueueLenght(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion

			}
		}
	}
}