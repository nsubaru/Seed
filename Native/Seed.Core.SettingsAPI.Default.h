#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.Time.Base.h"
#include "Seed.Core.SettingsAPI.h"
#include "Seed.Core.Memory.DataSize.h"

namespace Seed {
	inline namespace Core {
		///<summary>Конфігурації рантайму  Рем<summary>
		namespace Settings {
			/// <summary>
			/// Ім'я файла з налаштуваннями ядраІ
			/// </summary>
			constexpr Char SettingsFileName[16] = L"CoreSettings";

			/// <summary>
			/// Стартовий розмір стеку головного потоку
			/// </summary>
			constexpr Memory::DataSizeT MainThreadStackStartSize = 512Kb;

			/// <summary>
			/// Максимальний розмір стеку головного потоку
			/// </summary>
			constexpr Memory::DataSizeT MainThreadStackMaxSize = 1Mb;

			///<summary>Стандартні налаштування рантайму Seed</summary>
			namespace Default {
				ForceInline CurrentSettings::SettingsData InitDefaultSettings() {
					CurrentSettings::SettingsData DefaultSettings;
					//Менеджер пам'яті
					{
						//Секція стандартного алокатора пам'яті
						DefaultSettings.MainMemoryManager.DefaultHeap.StartSize = 40Mb;
						DefaultSettings.MainMemoryManager.DefaultHeap.MaxSize = 0Mb;
#ifdef PreReleaseMode
						DefaultSettings.MainMemoryManager.DefaultHeap.AdditionalСhecks = { 0x0, 0x0, 0x0, 0x0 };
						DefaultSettings.MainMemoryManager.DefaultHeap.IsLogingAllAllocationAndFree = false;
#elif DebugMode
						DefaultSettings.MainMemoryManager.DefaultHeap.AdditionalСhecks = { 0x1, 0x1, 0x1, 0x1 };
						DefaultSettings.MainMemoryManager.DefaultHeap.IsLogingAllAllocationAndFree = true;
#else
						DefaultSettings.MainMemoryManager.DefaultHeap.AdditionalСhecks = { 0 };
						DefaultSettings.MainMemoryManager.DefaultHeap.IsLogingAllAllocationAndFree = false;
#endif
						//Секція захищеного алокатора пам'яті
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.StartSize = 20Mb;
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.MaxSize = 0Mb;
#ifdef PreReleaseMode
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks = { 0x0, 0x0, 0x0, 0x0 };
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.IsLogingAllAllocationAndFree = false;
#elif DebugMode
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks = { 0x1, 0x1, 0x1 };
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.IsLogingAllAllocationAndFree = true;
#else
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks = { 0 };
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.IsLogingAllAllocationAndFree = false;
#endif
						//Секція алокатора пам'яті для обробника помилок
						DefaultSettings.MainMemoryManager.ExceptionHeap.StartSize = 80Mb;
						DefaultSettings.MainMemoryManager.ExceptionHeap.MaxSize = 0Mb;
#ifdef PreReleaseMode
						DefaultSettings.MainMemoryManager.ExceptionHeap.AdditionalСhecks = { 0x0, 0x0, 0x0, 0x0 };
						DefaultSettings.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = false;
#elif DebugMode
						DefaultSettings.MainMemoryManager.ExceptionHeap.AdditionalСhecks = { 0x1, 0x1, 0x1, 0x1 };
						DefaultSettings.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = true;
#else
						DefaultSettings.MainMemoryManager.ExceptionHeap.AdditionalСhecks = { 0 };
						DefaultSettings.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = false;
#endif

						//Cекція резервування пам'яті для буфера рядків
						DefaultSettings.MainMemoryManager.StringReserveSize = 50;
						//Секція стандатного максимального розміру черги
						DefaultSettings.MainMemoryManager.DefaultPriorityQueueMaxSize = 64;
						//Секція резервування пам'яті для буфера динамічних стек
						DefaultSettings.MainMemoryManager.DynamicStackReserveSize = 100;
						//Секція резервування пам'яті для буфера динамічних черг
						DefaultSettings.MainMemoryManager.DynamicQueueReserveSize = 100;
					}

					//Багатопотоковість
					{
						//Потоки
						{
							DefaultSettings.Threading.Thread.StackStartSize = 5Mb;
							DefaultSettings.Threading.Thread.StackMaxSize = 10Mb;
							DefaultSettings.Threading.Thread.StopKernelThreadsTimeout = Time::TimeSpan().AddMinutes(3);
							DefaultSettings.Threading.Thread.ThreadCreatingLogging = false;
						}
					}

					//Сервіси ядра
					{
						//Планувальник задач
						{
							//Стандарт роздільна здатність таймера для планувальника задач
							DefaultSettings.KernelServicesConfig.TaskSheduler.DefaultTimerProximity = Time::TimeSpan(1 * Time::SecondInNtTime);
						}

						{
							DefaultSettings.KernelServicesConfig.AutomaticTestingServiceCfg.SavedTestsCount = Time::WeekTicksCount / Time::MinutesTicksCount;
							DefaultSettings.KernelServicesConfig.AutomaticTestingServiceCfg.SelfTestingRetryTimeout = Time::TimeSpan(60 * Time::SecondInNtTime);
						}
					}
					{
						//Логер ядра
#ifdef PreReleaseMode
						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::BootLoaderAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::DepencyInjectionAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::EventsAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ExceptionABI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::InvalidSubsystem]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::IoAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::MemoryAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ObjectNamespaceAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::SerilizationAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::AutomaticTestingAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ManualTestingAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ThreadingAPI]
							= Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;
#elif DebugMode
						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::BootLoaderAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::DepencyInjectionAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::EventsAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ExceptionABI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::InvalidSubsystem]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::IoAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::MemoryAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ObjectNamespaceAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::SerilizationAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::AutomaticTestingAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ManualTestingAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ThreadingAPI]
							= Logger::ELoggerLevel::Debug | Logger::ELoggerLevel::Verbose | Logger::ELoggerLevel::Warning | Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;
#else //Release
						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::BootLoaderAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::DepencyInjectionAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::EventsAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ExceptionABI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::InvalidSubsystem]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::IoAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::MemoryAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ObjectNamespaceAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::SerilizationAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ManualTestingAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::AutomaticTestingAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;

						DefaultSettings.KernelLogger.CurrentStaticLogger[(SizeT)Logger::EStaticSubsystems::ThreadingAPI]
							= Logger::ELoggerLevel::Error | Logger::ELoggerLevel::Critical;
#endif
					}

					//Поточна версія ядра
					DefaultSettings.SettingsVersion = CurrentSettings::GetCurrentKernelVersion();

					return DefaultSettings;
				}
			}
		}
	}
}