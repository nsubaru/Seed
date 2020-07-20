#pragma once
#include "Seed.Core.SettingsAPI.ConstSettings.h"
#include "Seed.Core.Threading.LockPrimitives.h"

namespace Seed {
	inline namespace Core {
		///<summary>Сінглтон татичний класс поточних настройок</summary>
		class CurrentSettings final {
		public:
			///<summary>Дані налаштувань</summary>
			struct SettingsData {
				///<summary>Налаштування менеджера пам'яті</summary>
				struct MemoryManager {
					///<summary>Налаштування кучі</summary>
					struct HeapCfg {
						///<summary>Стартовий розмір кучі</summary>
						Bytes StartSize;
						///<summary>Максимальний розмір кучі</summary>
						Bytes MaxSize;

						///<summary>Додаткові флаги кучі</summary>
						struct PosibleAdditionalСhecks {
							///<summary>Провірка при вивільнені пам'яті</summary>
							UInt8 HEAP_FREE_CHECK : 1;
							///<summary>Провірка хвоста кучі</summary>
							UInt8 HEAP_TAIL_CHECK : 1;
							///<summary>Провірка аргументів при виділення\звільнені</summary>
							UInt8 HEAP_ARGS_CHECK : 1;
							///<summary>Провірка кучі при кожному запуску</summary>
							UInt8 HEAP_VALIDATE_ALL : 1;
						};

						///<summary>Додаткові флаги кучі</summary>
						PosibleAdditionalСhecks AdditionalСhecks;
						///<summary>Логування алокацій</summary>
						Boolean IsLogingAllAllocationAndFree;
					};

					///<summary>Стандартна куча</summary>
					HeapCfg DefaultHeap;
					///<summary>Чутлива куча</summary>
					HeapCfg SensitiveDataHeap;
					///<summary>Куча для помилок</summary>
					HeapCfg ExceptionHeap;

					///<summary>Розмір резервування для рядків у кількості рядки</summary>
					SizeT StringReserveSize;
					///<summary>Стандартна максимальна кількість елементів чергів</summary>
					SizeT DefaultPriorityQueueMaxSize;
					///<summary>Розмір резервування для стеку у кількості елементів</summary>
					SizeT DynamicStackReserveSize;
					///<summary>Розмір резервування для черги у кількіоті елементів</summary>
					SizeT DynamicQueueReserveSize;
					/// <summary>Стартовий розмір динамічного циклічного буферу </summary>
					SizeT DynamicCircularBufferStartSize;
				};

				///<summary>Налаштування багатопотоковості</summary>
				struct ThreadingCfg {
					///<summary>Налаштування потоків</summary>
					struct ThreadCfg {
						///<summary>Стартовий розмір стеку потоків</summary>
						SizeT StackMaxSize;
						///<summary>Максиальний розмір стеку потоків</summary>
						SizeT StackStartSize;
						/// <summary> Таймаут очікуванння на потоки при завершені роботи ядра </summary>
						SizeT StopKernelThreadsTimeout;
					};
					///<summary>Налаштування потоків</summary>
					ThreadCfg Thread;
				};

				///<summary>Налаштування сервісів ядра</summary>
				struct KernelServicesConfigT {
					///<summary>Налаштування планувальника задач</summary>
					struct TaskShedulerCfgT {
						///<summary>Стандартна роздільна здатність таймера для задач в планувальнику задач</summary>
						Int32 DefaultTimerProximity;
					};
					///<summary>Налаштування планувальника задач</summary>
					TaskShedulerCfgT TaskSheduler;
				};

				///<summary>Налаштування менеджера пам'яті</summary>
				MemoryManager MainMemoryManager;
				///<summary>Налаштування багатопотоковості</summary>
				ThreadingCfg Threading;
				///<summary>Налаштування сервісів ядра</summary>
				KernelServicesConfigT KernelServicesConfig;

				/// <summary>
				/// Структура опису версії ядра
				/// </summary>
				struct KernelVersion {
					UInt64 Major;
					UInt64 Minor;
					UInt64 StableBuildNumber;
					UInt64 BuildNumber;
				};

				/// <summary>
				/// Версія налаштувань
				/// </summary>
				KernelVersion SettingsVersion;
			};

			using KernelVersionT = SettingsData::KernelVersion;
		private:
			SeedKernelCall static const SettingsData& GetCurrentSettingDataRaw();
			SeedKernelCall static SettingsData& SetCurrentSettingDataRaw();
			SeedKernelCall static void InformAboutSettingsChange();
			static Threading::SRWLock& GetSettingsLock();
		private:
			template<typename Lambda>
			static decltype(auto) GetCurrentSettingData(Lambda&& lambda) {
				return GetSettingsLock().ShareLock([&] ()
				{
					return lambda(GetCurrentSettingDataRaw());
				});
			}

			template<typename Lambda>
			static void SetCurrentSettingData(Lambda&& lambda) {
				GetSettingsLock().ExclusiveLock([&] ()
				{
					lambda(SetCurrentSettingDataRaw());
				});
				InformAboutSettingsChange();
			}
		public:
			///<summary>Налаштування середовища</summary>
			struct VirtualSettingData {
				///<summary>Розташування виконавчого файла ядра</summary>
				Char KernelExecutableLocation[Math::Max<Int16>()];
				///<summary>Кількість ядер в хост ситема ядра</summary>
				UInt64 CpuCoresCount;
			};
		private:
			SeedKernelCall static const VirtualSettingData& GetVirtualSettingDataRaw();
		public:

			
		public:
			class MemoryManagerT final {
			public:
				class DefaultHeapT final {
				public:
					DefaultHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.DefaultHeap.StartSize;
						});
					}
					void SetStartSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& data)
						{
							data.MainMemoryManager.DefaultHeap.StartSize = NewSize;
						});
					}

					SizeT GetMaxSize() {
						return GetCurrentSettingData([&] (auto& data)
						{
							return data.MainMemoryManager.DefaultHeap.MaxSize;
						});
					}
					void SetMaxSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.DefaultHeap.MaxSize = NewSize;
						});
					}

					Boolean GetHeapFreeCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
						});
					}
					void SetHeapFreeCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapTailCheck() {
						return GetCurrentSettingData([] (auto data)
						{
							return data.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
						});
					}
					void SetHeapTailCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapArgsCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
						});
					}
					void SetHeapArgsCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapValidationChecks() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
						});
					}
					void SetHeapValidationChecks(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapAllocsLogging() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree;
						});
					}
					void SetHeapAllocsLogging(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = NewState;
						});
					}
				public:
					///<summary>Стартовий розмір кучі</summary>
					SizeT Property(GetStartSize, SetStartSize) StartSize;
					///<summary>Максимальний розмір кучі
					///<para>0 - значить без обмежень</para>
					///</summary>
					SizeT Property(GetMaxSize, SetMaxSize) MaxSize;

					///<summary>Провірка кучі на цілісність при звільнені ділянки пам'яті</summary>
					Boolean Property(GetHeapFreeCheck, SetHeapFreeCheck) HeapFreeCheck;
					///<summary>Провірка хвоста кучі</summary>
					Boolean Property(GetHeapTailCheck, SetHeapTailCheck) HeapTailCheck;
					///<summary>Провірка аргументів при виділенні\звільнені</summary>
					Boolean Property(GetHeapArgsCheck, SetHeapArgsCheck) HeapArgsCheck;
					///<summary>Провірка кучі при виділенні\звільненні пам'яті</summary>
					Boolean Property(GetHeapValidationChecks, SetHeapValidationChecks) HeapValidationChecks;

					///<summary>Логування всіх алокацій і звільненнь пам'яті</summary>
					Boolean Property(GetHeapAllocsLogging, SetHeapAllocsLogging) HeapAllocsLogging;
				};

				class SensitiveDataHeapT final {
				public:
					SensitiveDataHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.SensitiveDataHeap.StartSize;
						});
					}
					void SetStartSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.StartSize = NewSize;
						});
					}

					SizeT GetMaxSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.SensitiveDataHeap.MaxSize;
						});
					}
					void SetMaxSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.MaxSize = NewSize;
						});
					}

					Boolean GetHeapFreeCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
						});
					}
					void SetHeapFreeCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapTailCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
						});
					}
					void SetHeapTailCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapArgsCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
						});
					}
					void SetHeapArgsCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapValidationChecks() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
						});
					}
					void SetHeapValidationChecks(Boolean NewState) {
						SetCurrentSettingData([&] (auto CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapAllocsLogging() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree;
						});
					}
					void SetHeapAllocsLogging(Boolean NewState) {
						SetCurrentSettingData([&] (auto CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = NewState;
						});
					}
				public:
					///<summary>Стартовий розмір кучі</summary>
					SizeT Property(GetStartSize, SetStartSize) StartSize;
					///<summary>Максимальний розмір кучі
					///<para>0 - значить без обмежень</para>
					///</summary>
					SizeT Property(GetMaxSize, SetMaxSize) MaxSize;

					///<summary>Провірка кучі на цілісність при звільнені ділянки пам'яті</summary>
					Boolean Property(GetHeapFreeCheck, SetHeapFreeCheck) HeapFreeCheck;
					///<summary>Провірка хвоста кучі</summary>
					Boolean Property(GetHeapTailCheck, SetHeapTailCheck) HeapTailCheck;
					///<summary>Провірка аргументів при виділенні\звільнені</summary>
					Boolean Property(GetHeapArgsCheck, SetHeapArgsCheck) HeapArgsCheck;
					///<summary>Провірка кучі при виділенні\звільненні пам'яті</summary>
					Boolean Property(GetHeapValidationChecks, SetHeapValidationChecks) HeapValidationChecks;

					///<summary>Логування всіх алокацій і звільненнь пам'яті</summary>
					Boolean Property(GetHeapAllocsLogging, SetHeapAllocsLogging) HeapAllocsLogging;
				};

				class ExceptionHeapT final {
				public:
					ExceptionHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.StartSize;
						});
					}
					void SetStartSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.StartSize = NewSize;
						});
					}

					SizeT GetMaxSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.MaxSize;
						});
					}
					void SetMaxSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.MaxSize = NewSize;
						});
					}

					Boolean GetHeapFreeCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
						});
					}
					void SetHeapFreeCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapTailCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
						});
					}
					void SetHeapTailCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapArgsCheck() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
						});
					}
					void SetHeapArgsCheck(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
						});
					}

					Boolean GetHeapValidationChecks() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
						});
					}
					void SetHeapValidationChecks(Boolean NewState) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
						});
					}
				public:
					///<summary>Стартовий розмір кучі</summary>
					SizeT Property(GetStartSize, SetStartSize) StartSize;
					///<summary>Максимальний розмір кучі
					///<para>0 - значить без обмежень</para>
					///</summary>
					SizeT Property(GetMaxSize, SetMaxSize) MaxSize;

					///<summary>Провірка кучі на цілісність при звільнені ділянки пам'яті</summary>
					Boolean Property(GetHeapFreeCheck, SetHeapFreeCheck) HeapFreeCheck;
					///<summary>Провірка хвоста кучі</summary>
					Boolean Property(GetHeapTailCheck, SetHeapTailCheck) HeapTailCheck;
					///<summary>Провірка аргументів при виділенні\звільнені</summary>
					Boolean Property(GetHeapArgsCheck, SetHeapArgsCheck) HeapArgsCheck;
					///<summary>Провірка кучі при виділенні\звільненні пам'яті</summary>
					Boolean Property(GetHeapValidationChecks, SetHeapValidationChecks) HeapValidationChecks;
				};
			public:
				MemoryManagerT() {};

				DefaultHeapT GetDefaultHeap() { return DefaultHeapT(); }
				void SetDefaultHeap(DefaultHeapT) {}

				SensitiveDataHeapT GetSensitiveDataHeap() { return SensitiveDataHeapT(); }
				void SetSensitiveDataHeap(SensitiveDataHeapT) {}

				ExceptionHeapT GetExceptionHeap() { return ExceptionHeapT(); }
				void SetExceptionHeap(ExceptionHeapT) {}

				SizeT GetStringReserveSize() {
					return GetCurrentSettingData([] (auto& data)
					{
						return data.MainMemoryManager.StringReserveSize;
					});
				}
				void SetStringReserveSize(SizeT NewSize) {
					SetCurrentSettingData([&] (auto& CurrentSettingsData)
					{
						CurrentSettingsData.MainMemoryManager.StringReserveSize = NewSize;
					});
				}

				SizeT GetDefaultPriorityQueueMaxSize() {
					return GetCurrentSettingData([] (auto& data)
					{
						return data.MainMemoryManager.DefaultPriorityQueueMaxSize;
					});
				}
				void SetDefaultPriorityQueueMaxSize(SizeT NewSize) {
					SetCurrentSettingData([&] (auto& CurrentSettingsData)
					{
						CurrentSettingsData.MainMemoryManager.DefaultPriorityQueueMaxSize = NewSize;
					});
				}

				SizeT GetStackReserveSize() {
					return GetCurrentSettingData([] (auto& data)
					{
						return data.MainMemoryManager.DynamicStackReserveSize;
					});
				}
				void SetStackReserveSize(SizeT NewSize) {
					SetCurrentSettingData([&] (auto& CurrentSettingsData)
					{
						CurrentSettingsData.MainMemoryManager.DynamicStackReserveSize = NewSize;
					});
				}

				SizeT GetQueueReserveSize() {
					return GetCurrentSettingData([] (auto& data)
					{
						return data.MainMemoryManager.DynamicQueueReserveSize;
					});
				}
				void SetQueueReserveSize(SizeT NewSize) {
					SetCurrentSettingData([&] (auto& CurrentSettingsData)
					{
						CurrentSettingsData.MainMemoryManager.DynamicQueueReserveSize = NewSize;
					});
				}

				SizeT GetDynamicCircularBufferStartSize() {
					return GetCurrentSettingData([] (auto& data)
					{
						return data.MainMemoryManager.DynamicCircularBufferStartSize;
					});
				}
				void SetDynamicCircularBufferStartSize(SizeT NewSize) {
					SetCurrentSettingData([&] (auto& CurrentSettingsData)
					{
						CurrentSettingsData.MainMemoryManager.DynamicCircularBufferStartSize = NewSize;
					});
				}
			public:
				///<summary>Налаштування стандартної кучі пам'яті</summary>
				DefaultHeapT Property(GetDefaultHeap, SetDefaultHeap) DefaultHeap;
				///<summary>Налаштування чутливої кучі пам'яті</summary>
				SensitiveDataHeapT Property(GetSensitiveDataHeap, SetSensitiveDataHeap) SensitiveDataHeap;
				///<summary>Налаштування кучі для помилок</summary>
				ExceptionHeapT Property(GetExceptionHeap, SetExceptionHeap) ExceptionHeap;

				///<summary>Розмір резервування дял рядкової зміної в кількості символів</summary>
				SizeT Property(GetStringReserveSize, SetStringReserveSize) StringReserveSize;
				///<summary>Стандартна максимальна кількість елементів чергів</summary>
				SizeT Property(GetDefaultPriorityQueueMaxSize, SetDefaultPriorityQueueMaxSize) DefaultPriorityQueueMaxSize;
				///<summary>Розмір резервування для стеку у кількості елементів</summary>
				SizeT Property(GetStackReserveSize, SetStackReserveSize) StackReserveSize;
				///<summary>Розмір резервування для черги у кількості елементів</summary>
				SizeT Property(GetQueueReserveSize, SetQueueReserveSize) QueueReserveSize;
				/// <summary>Стартовий розмір динамічного циклічного буферу </summary>
				SizeT Property(GetDynamicCircularBufferStartSize, SetDynamicCircularBufferStartSize) DynamicCircularBufferStartSize;
			};

			class ThreadingCfgT final {
			public:
				class ThreadCfg final {
				public:
					ThreadCfg() {}

					SizeT GetStackStartSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.Threading.Thread.StackStartSize;
						});
					}
					void SetStackStartSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.Threading.Thread.StackStartSize = NewSize;
						});
					}

					SizeT GetStackMaxSize() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.Threading.Thread.StackMaxSize;
						});
					}
					void SetStackMaxSize(SizeT NewSize) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.Threading.Thread.StackMaxSize = NewSize;
						});
					}

					SizeT GetStopKernelThreadsTimeout() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.Threading.Thread.StopKernelThreadsTimeout;
						});
					}
					void SetStopKernelThreadsTimeout(SizeT NewStopKernelThreadsTimeout) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.Threading.Thread.StopKernelThreadsTimeout = NewStopKernelThreadsTimeout;
						});
					}
				public:
					///<summary>Стартовий розмір стеку у потоків</summary>
					SizeT Property(GetStackStartSize, SetStackStartSize) StackStartSize;
					///<summary>Максимальний розмір стеку у потоків</summary>
					SizeT Property(GetStackMaxSize, SetStackMaxSize) StackMaxSize;
					///<summary>Таймаут очікуванння на потоки при завершені роботи ядра</summary>
					SizeT Property(GetStopKernelThreadsTimeout, SetStopKernelThreadsTimeout) StopKernelThreadsTimeout;
				};
			public:
				ThreadingCfgT() {}

				ThreadCfg GetThread() { return ThreadCfg(); }
				void SetThread(ThreadCfg) {}
			public:
				///<summary>Налаштування потоків</summary>
				ThreadCfg Property(GetThread, SetThread) Thread;
			};

			class KernelServicesConfigT final {
			public:
				class TaskShedulerCfgT final {
				public:
					TaskShedulerCfgT() {} //-V730

					Int32 GetDefaultTimerProximity() {
						return GetCurrentSettingData([] (auto& data)
						{
							return data.KernelServicesConfig.TaskSheduler.DefaultTimerProximity;
						});
					}
					void SetDefaultTimerProximity(Int32 NewDefaultTimerProximity) {
						SetCurrentSettingData([&] (auto& CurrentSettingsData)
						{
							CurrentSettingsData.KernelServicesConfig.TaskSheduler.DefaultTimerProximity = NewDefaultTimerProximity;
						});
					}
				public:
					///<summary>Стандартна точність таймера для задач</summary>
					Int32 Property(GetDefaultTimerProximity, SetDefaultTimerProximity) DefaultTimerProximity;
				};
			public:
				TaskShedulerCfgT GetTaskSheduler() { return TaskShedulerCfgT(); }
				void SetTaskSheduler(TaskShedulerCfgT) {}
			public:
				///<summary>Налаштування сервісу планування задач</summary>
				TaskShedulerCfgT Property(GetTaskSheduler, SetTaskSheduler) TaskSheduler;
			};

			class VirtualSettingDataT final {
			public:
				typedef const Char(&KernelExcecutableLocationT)[Math::Max<Int16>()];
			public:
				VirtualSettingDataT() {} //-V730

				KernelExcecutableLocationT GetKernelExcecutableLocation() {
					return GetVirtualSettingDataRaw().KernelExecutableLocation;
				}

				UInt64 GetCpuCoresCount() {
					return GetVirtualSettingDataRaw().CpuCoresCount;
				}
			public:
				///<summary>Повний NT адресс виконавчого файла ядра</summary>
				KernelExcecutableLocationT Property(GetKernelExcecutableLocation) KernelExcecutableLocation;
				///<summary>Кількість ядер в процесора у робочої системи</summary>
				UInt64 Property(GetCpuCoresCount) CpuCoresCount;
			};

			class StaticSettingsDataT final {
			public:
				StaticSettingsDataT() {} //-V730

				constexpr Settings::StaticSettingsData::TShutdownEventName GetShutdownEventName() {
					return Settings::StaticSettingsData::ShutdownEventName;
				}

				constexpr Settings::StaticSettingsData::TReloadEventName GetReloadEventName() {
					return Settings::StaticSettingsData::ReloadEventName;
				}

				constexpr Settings::StaticSettingsData::TKernelNamespaceName GetKernelNamespaceName() {
					return Settings::StaticSettingsData::KernelNamespaceName;
				}

				constexpr Settings::StaticSettingsData::TKernelExcecutableNamespaceName GetKernelExcecutableNamespaceName() {
					return Settings::StaticSettingsData::KernelExcecutableNamespaceName;
				}

				constexpr Settings::StaticSettingsData::THostCommitLowEventName GetHostCommitLowEventName() {
					return Settings::StaticSettingsData::HostCommitLowEventName;
				}

				constexpr Settings::StaticSettingsData::TSettingsChangeEventName GetSettingsChangeEventName() {
					return Settings::StaticSettingsData::SettingsChangeEventName;
				}

				constexpr Settings::StaticSettingsData::TOnLogWriteEventName GetOnLogWriteEventName() {
					return Settings::StaticSettingsData::OnLogWriteEventName;
				}
			public:
				///<summary>Ім'я події завершення роботи ядра</summary>
				Settings::StaticSettingsData::TShutdownEventName Property(GetShutdownEventName) ShutdownEventName;
				///<summary>Ім'я події перезагрузки ядра</summary>
				Settings::StaticSettingsData::TReloadEventName Property(GetReloadEventName) ReloadEventName;

				///<summary>Ім'я простору імен подій ядра Рем</summary>
				Settings::StaticSettingsData::TKernelNamespaceName Property(GetKernelNamespaceName) KernelNamespaceName;
				///<summary>Ім'я простору імен подій виконавчої системи ядра Рем</summary>
				Settings::StaticSettingsData::TKernelExcecutableNamespaceName Property(GetKernelExcecutableNamespaceName) KernelExcecutableNamespaceName;

				/// <summary>
				/// Ім'я події малої кількості вільної пам'яті в системі
				/// </summary>
				Settings::StaticSettingsData::THostCommitLowEventName Property(GetHostCommitLowEventName) HostCommitLowEventName;
				/// <summary>
				/// Ім'я події інформування про зміни налаштувань ядра Рем
				/// </summary>
				Settings::StaticSettingsData::TSettingsChangeEventName Property(GetSettingsChangeEventName) SettingsChangeEventName;
				/// <summary>
				/// Ім'я події запису в логи
				/// </summary>
				Settings::StaticSettingsData::TOnLogWriteEventName Property(GetOnLogWriteEventName) OnLogWriteEventName;
			};

			class ObjectNamespaceConfigurationT final {
			public:
				constexpr ObjectNamespaceConfigurationT() {} //-V730

				constexpr Settings::ObjectManagerConfigurations::TRootPath GetRootPath() {
					return Settings::ObjectManagerConfigurations::RootPath;
				}

				constexpr Settings::ObjectManagerConfigurations::TKernelPathName GetKernelPathName() {
					return Settings::ObjectManagerConfigurations::KernelPathName;
				}

				constexpr Settings::ObjectManagerConfigurations::THostPathName GetHostPathName() {
					return Settings::ObjectManagerConfigurations::HostPathName;
				}

				constexpr Settings::ObjectManagerConfigurations::TRemotePathName GetRemotePathName() {
					return Settings::ObjectManagerConfigurations::RemotePathName;
				}

				constexpr SizeT GetToManyRedirectsTreshold() {
					return Settings::ObjectManagerConfigurations::ToManyRedirectsTreshold;
				}
			public:
				/// <summary>
				/// Шлях корення в об'єктному менеджері
				/// </summary>
				Settings::ObjectManagerConfigurations::TRootPath Property(GetRootPath) RootPath;
				/// <summary>
				/// Шлях провайдерів ядра в об'єктному менеджері
				/// </summary>
				Settings::ObjectManagerConfigurations::TKernelPathName Property(GetKernelPathName) KernelPathName;
				/// <summary>
				/// Шлях провайдерів хостової системи в об'єктному менеджері
				/// </summary>
				Settings::ObjectManagerConfigurations::THostPathName Property(GetHostPathName) HostPathName;
				/// <summary>
				/// Шлях провайдерів віддалених систем в об'єктному менеджері
				/// </summary>
				Settings::ObjectManagerConfigurations::TRemotePathName Property(GetRemotePathName) RemotePathName;
				/// <summary>
				/// Ліміт редіректів при обході шляху в об'єктном просторі
				/// </summary>
				SizeT Property(GetToManyRedirectsTreshold) ToManyRedirectsTreshold;
			};

			/// <summary>
			/// Налаштування менеджера розділів на хості ядра
			/// </summary>
			class PartitionManagerConfigurationT final {
			public:
				constexpr PartitionManagerConfigurationT() {} //-V730

				constexpr Settings::PartitionManagerConfiguration::THostPartitionPath GetHostPartitionPath() {
					return Settings::PartitionManagerConfiguration::HostPartitionPath;
				}

				constexpr Settings::PartitionManagerConfiguration::THostPartitionSymLink GetHostPartitionSymLink() {
					return Settings::PartitionManagerConfiguration::HostPartitionSymLink;
				}

				constexpr Settings::PartitionManagerConfiguration::TKernelFsPathSymLink GetKernelFsPathSymLink() {
					return Settings::PartitionManagerConfiguration::KernelFsPathSymLink;
				}

				constexpr Settings::PartitionManagerConfiguration::TKernelPartitionSymLink GetKernelPartitionSymLink() {
					return Settings::PartitionManagerConfiguration::KernelPartitionSymLink;
				}
			public:
				/// <summary>
				/// Корінь менеджера розділів
				/// </summary>
				Settings::PartitionManagerConfiguration::THostPartitionPath Property(GetHostPartitionPath) HostPartitionPath;
				/// <summary>
				/// Розташування силки на розділ з операційною системою
				/// </summary>
				Settings::PartitionManagerConfiguration::THostPartitionSymLink Property(GetHostPartitionSymLink) HostPartitionSymLink;
				/// <summary>
				/// Розташування силки на розділ з ядром
				/// </summary>
				Settings::PartitionManagerConfiguration::TKernelFsPathSymLink Property(GetKernelFsPathSymLink) KernelFsPathSymLink;
				/// <summary>
				/// Розташування силки на директорію з ядром
				/// </summary>
				Settings::PartitionManagerConfiguration::TKernelPartitionSymLink Property(GetKernelPartitionSymLink) KernelPartitionSymLink;
			};
		public:
			constexpr CurrentSettings() {};

			StaticSettingsDataT GetStaticSettingsData() { return StaticSettingsDataT(); }
			void SetStaticSettingsData(StaticSettingsDataT) {}

			constexpr ObjectNamespaceConfigurationT GetObjectNamespaceConfiguration() { return ObjectNamespaceConfigurationT(); }
			constexpr void SetObjectNamespaceConfiguration(ObjectNamespaceConfigurationT) {}

			VirtualSettingDataT GetVirtualSettingData() { return VirtualSettingDataT(); }
			void SetVirtualSettingDataT(VirtualSettingDataT) {}

			MemoryManagerT GetMemoryManager() { return MemoryManagerT(); }
			void SetMemoryManager(MemoryManagerT) {}

			KernelServicesConfigT GetKernelServicesConfig() { return KernelServicesConfigT(); }
			void SetKernelServicesConfig(KernelServicesConfigT) {}

			ThreadingCfgT GetThreadinfConfig() { return ThreadingCfgT(); }
			void SetThreadinfConfig(ThreadingCfgT) {}

			constexpr PartitionManagerConfigurationT GetPartitionManagerConfiguration() { return PartitionManagerConfigurationT(); }
			constexpr void SetPartitionManagerConfiguration(PartitionManagerConfigurationT) {}

			KernelVersionT GetSettingsKernelVersion() {
				return GetCurrentSettingData([] (auto data)
				{
					return data.SettingsVersion;
				});
			}

			static KernelVersionT GetCurrentKernelVersion();
		public:
			/// <summary>
			/// Налаштування менеджера пам'яті
			/// </summary>
			MemoryManagerT Property(GetMemoryManager, SetMemoryManager) MemoryManager;
			/// <summary>
			/// Налаштування сервісів ядра
			/// </summary>
			KernelServicesConfigT Property(GetKernelServicesConfig, SetKernelServicesConfig) KernelServices;
			/// <summary>
			/// Рантайм налаштування
			/// </summary>
			VirtualSettingDataT Property(GetVirtualSettingData, SetVirtualSettingDataT) VirtualSetting;
			/// <summary>
			/// Налаштування багатопотоковості
			/// </summary>
			ThreadingCfgT Property(GetThreadinfConfig, SetThreadinfConfig) Threading;
			/// <summary>
			/// Статичні налаштування ядра
			/// </summary>
			StaticSettingsDataT Property(GetStaticSettingsData, SetStaticSettingsData) StaticSettings;
			/// <summary>
			/// Налаштування менеджера глобальних об'єктів
			/// </summary>
			ObjectNamespaceConfigurationT Property(GetObjectNamespaceConfiguration, SetObjectNamespaceConfiguration) ObjectNamespaceConfiguration;
			/// <summary>
			/// Налаштування менеджера розділіва хоста
			/// </summary>
			PartitionManagerConfigurationT Property(GetPartitionManagerConfiguration, SetPartitionManagerConfiguration) PartitionManagerConfiguration;
		public:
			void SaveCurrentSettings();
		};
	}
}