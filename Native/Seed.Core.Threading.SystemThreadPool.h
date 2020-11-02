#pragma once
#include "Seed.Core.Threading.UnifiedThreadPool.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace SystemThreadPool {
#pragma region Jobs
				/// <summary>
				/// Запуск задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				
				/// <summary>
				/// Запуск задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="name">Ім'я задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				
				/// <summary>
				/// Запуск безіменої задачі в системному пулі задач
				/// </summary>
				/// <param name="action">Колбек задачі</param>
				/// <param name="priority">Пріоритет задачі</param>
				/// <returns>Об'єкт задачі</returns>
				UnifiedThreadPoolJob PostPriorityJob(Delegate&& action, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion
#pragma region Scheduled task
				/// <summary>
				/// Запуск таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="name">Ім'я таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				
				/// <summary>
				/// Запуск таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="name">Ім'я таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, const String& name, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
				
				/// <summary>
				/// Запуск безіменого таймера в системному пулі задач
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <param name="priority">Пріоритет таймера</param>
				/// <returns>Конструктор таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder CreatePriorityScheduledTask(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion
#pragma region Statistic
				/// <summary>
				/// Вертає загальну кількість об'єктів в системному пулі задач
				/// </summary>
				/// <returns>Загальна кількість об'єктів в системному пулі задач</returns>
				SizeT AllItemCount();

				/// <summary>
				/// Вертаж кількість об'єктів в системному пулі задачі з певним пріоритетом
				/// </summary>
				/// <param name="priority">Пріоритет об'єктів</param>
				/// <returns>Кількість об'єктів в системному пулі задачі з заданим пріоритетом</returns>
				SizeT AllItemSpecificPriorityCount(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				/// <summary>
				/// Вертає кількість задач в системному пулі
				/// </summary>
				/// <returns>Кількість задач в системному пулі</returns>
				SizeT JobCount();

				/// <summary>
				/// Вертає кількість задач в системному пулі з певним пріоритетом
				/// </summary>
				/// <param name="priority">Пріоритет задач</param>
				/// <returns>Кількість задач з заданим пріоритетом в системному пулі</returns>
				SizeT JobSpecificPriorityCount(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				/// <summary>
				/// Вертає кількість таймерів в системному пулі
				/// </summary>
				/// <returns>Кількість таймерів в системному пулі</returns>
				SizeT ScheduledTaskCount();

				/// <summary>
				/// Вертає кількість таймерів в системному пулі з певним пріоритетом
				/// </summary>
				/// <param name="priority">Пріоритет таймерів</param>
				/// <returns>Кількість таймерів з заданим пріоритетом в системному пулі</returns>
				SizeT ScheduledTaskSpecificPriorityCount(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);

				/// <summary>
				/// Вертає кількість файлових об'єктів в системному пулі
				/// </summary>
				/// <returns>Кількість файлових об'єктів в системному пулі</returns>
				SizeT IoTaskCount();

				/// <summary>
				/// Вертає кількість файлових об'єктів в системному пулі з певним пріоритетом
				/// </summary>
				/// <param name="priority">Пріоритет файлових об'єктів</param>
				/// <returns>Кількість файлових об'єктів з заданим пріоритетом в системному пулі</returns>
				SizeT IoTaskSpecificPriorityCount(Threading::EPoolThreadsPriority priority = Threading::EPoolThreadsPriority::Medium);
#pragma endregion

			}
		}
	}
}