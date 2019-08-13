#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		///<summary>Сінглтон татичний класс поточних настройок</summary>
		class SeedKernelCall CurrentSettings {
		public:
			///<summary>Дані налаштувань</summary>
			struct SeedKernelCall SettingsData {
				///<summary>Налаштування менеджера пам'яті</summary>
				struct SeedKernelCall MemoryManager {
					///<summary>Налаштування кучі</summary>
					struct SeedKernelCall HeapCfg {
						///<summary>Стартовий розмір кучі</summary>
						Bytes StartSize;
						///<summary>Максимальний розмір кучі</summary>
						Bytes MaxSize;

						///<summary>Додаткові флаги кучі</summary>
						struct SeedKernelCall PosibleAdditionalСhecks {
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
					SizeT QueueMaxSize;
				};

				///<summary>Налаштування багатопотоковості</summary>
				struct SeedKernelCall ThreadingCfg {
					///<summary>Налаштування потоків</summary>
					struct SeedKernelCall ThreadCfg {
						///<summary>Стартовий розмір стеку потоків</summary>
						SizeT StackMaxSize;
						///<summary>Максиальний розмір стеку потоків</summary>
						SizeT StackStartSize;
					};
					///<summary>Налаштування потоків</summary>
					ThreadCfg Thread;
				};

				///<summary>Налаштування сервісів ядра</summary>
				struct SeedKernelCall KernelServiceConfigT {
					///<summary>Налаштування планувальника задач</summary>
					struct SeedKernelCall TaskShedulerCfgT {
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
				KernelServiceConfigT KernelServiceConfig;
			};

			///<summary>Налаштування середовища</summary>
			struct SeedKernelCall VirtualSettingData {
				///<summary>Розташування виконавчого файла ядра</summary>
				Char KernelExecutableLocation[Math::Max<Int16>()];
				///<summary>Кількість ядер в хост ситема ядра</summary>
				UInt64 CpuCoresCount;
			};
		public:
			class SeedKernelCall MemoryManagerT {
			public:
				class SeedKernelCall DefaultHeapT {
				public:
					DefaultHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.DefaultHeap.StartSize;
					}
					void SetStartSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.DefaultHeap.StartSize = NewSize;
					}

					SizeT GetMaxSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.DefaultHeap.MaxSize;
					}
					void SetMaxSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.DefaultHeap.MaxSize = NewSize;
					}

					Boolean GetHeapFreeCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
					}
					void SetHeapFreeCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapTailCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
					}
					void SetHeapTailCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapArgsCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
					}
					void SetHeapArgsCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.DefaultHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapValidationChecks() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
					}
					void SetHeapValidationChecks(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapAllocsLogging() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree;
					}
					void SetHeapAllocsLogging(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = NewState;
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

				class SeedKernelCall SensitiveDataHeapT {
				public:
					SensitiveDataHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.StartSize;
					}
					void SetStartSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.StartSize = NewSize;
					}

					SizeT GetMaxSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.MaxSize;
					}
					void SetMaxSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.MaxSize = NewSize;
					}

					Boolean GetHeapFreeCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
					}
					void SetHeapFreeCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapTailCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
					}
					void SetHeapTailCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapArgsCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
					}
					void SetHeapArgsCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapValidationChecks() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
					}
					void SetHeapValidationChecks(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapAllocsLogging() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree;
					}
					void SetHeapAllocsLogging(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.IsLogingAllAllocationAndFree = NewState;
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

				class SeedKernelCall ExceptionHeapT {
				public:
					ExceptionHeapT() {} //-V730
				public:
					SizeT GetStartSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.StartSize;
					}
					void SetStartSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.StartSize = NewSize;
					}

					SizeT GetMaxSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.MaxSize;
					}
					void SetMaxSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.MaxSize = NewSize;
					}

					Boolean GetHeapFreeCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_FREE_CHECK == 0x1;
					}
					void SetHeapFreeCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_FREE_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapTailCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_TAIL_CHECK == 0x1;
					}
					void SetHeapTailCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_TAIL_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapArgsCheck() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_ARGS_CHECK == 0x1;
					}
					void SetHeapArgsCheck(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_ARGS_CHECK = (NewState) ? 0x1 : 0x0;
					}

					Boolean GetHeapValidationChecks() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL == 0x1;
					}
					void SetHeapValidationChecks(Boolean NewState) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.MainMemoryManager.ExceptionHeap.AdditionalСhecks.HEAP_VALIDATE_ALL = (NewState) ? 0x1 : 0x0;
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
					extern SettingsData CurrentSettingsData;
					return CurrentSettingsData.MainMemoryManager.StringReserveSize;
				}
				void SetStringReserveSize(SizeT NewSize) {
					extern SettingsData CurrentSettingsData;
					CurrentSettingsData.MainMemoryManager.StringReserveSize = NewSize;
				}

				SizeT GetQueueMaxSize() {
					extern SettingsData CurrentSettingsData;
					return CurrentSettingsData.MainMemoryManager.QueueMaxSize;
				}
				void SetQueueMaxSize(SizeT NewSize) {
					extern SettingsData CurrentSettingsData;
					CurrentSettingsData.MainMemoryManager.QueueMaxSize = NewSize;
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
				SizeT Property(GetQueueMaxSize, SetQueueMaxSize) QueueMaxSize;
			};
			
			class SeedKernelCall ThreadingCfgT {
			public:
				class SeedKernelCall ThreadCfg {
				public:
					ThreadCfg() {}

					SizeT GetStackStartSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.Threading.Thread.StackStartSize;
					}
					void SetStackStartSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.Threading.Thread.StackStartSize = NewSize;
					}

					SizeT GetStackMaxSize() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.Threading.Thread.StackMaxSize;
					}
					void SetStackMaxSize(SizeT NewSize) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.Threading.Thread.StackMaxSize = NewSize;
					}
				public:
					///<summary>Стартовий розмір стеку у потоків</summary>
					SizeT Property(GetStackStartSize, SetStackStartSize) StackStartSize;
					///<summary>Максимальний розмір стеку у потоків</summary>
					SizeT Property(GetStackMaxSize, SetStackMaxSize) StackMaxSize;
				};
			public:
				ThreadingCfgT() {}

				ThreadCfg GetThread() { return ThreadCfg(); }
				void SetThread(ThreadCfg) {}
			public:
				///<summary>Налаштування потоків</summary>
				ThreadCfg Property(GetThread, SetThread) Thread;
			};

			class SeedKernelCall KernelServiceConfigT {
			public:
				class SeedKernelCall TaskShedulerCfgT {
				public:
					TaskShedulerCfgT() {} //-V730

					Int32 GetDefaultTimerProximity() {
						extern SettingsData CurrentSettingsData;
						return CurrentSettingsData.KernelServiceConfig.TaskSheduler.DefaultTimerProximity;
					}
					void SetDefaultTimerProximity(Int32 NewDefaultTimerProximity) {
						extern SettingsData CurrentSettingsData;
						CurrentSettingsData.KernelServiceConfig.TaskSheduler.DefaultTimerProximity = NewDefaultTimerProximity;
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

			class SeedKernelCall VirtualSettingDataT {
			public:
				typedef const Char(&KernelExcecutableLocationT)[Math::Max<Int16>()];
			public:
				VirtualSettingDataT() {} //-V730

				KernelExcecutableLocationT GetKernelExcecutableLocation() {
					extern VirtualSettingData CurrentVirtualSettingsData;
					return CurrentVirtualSettingsData.KernelExecutableLocation;
				}

				UInt64 GetCpuCoresCount() {
					extern VirtualSettingData CurrentVirtualSettingsData;
					return CurrentVirtualSettingsData.CpuCoresCount;
				}
			public:
				///<summary>Повний NT адресс виконавчого файла ядра</summary>
				KernelExcecutableLocationT Property(GetKernelExcecutableLocation) KernelExcecutableLocation;
				///<summary>Кількість ядер в процесора у робочої системи</summary>
				UInt64 Property(GetCpuCoresCount) CpuCoresCount;
			};
		public:
			CurrentSettings() {};

			VirtualSettingDataT GetVirtualSettingData() { return VirtualSettingDataT(); }
			void SetVirtualSettingDataT(VirtualSettingDataT) {}
			
			MemoryManagerT GetMemoryManager() { return MemoryManagerT(); }
			void SetMemoryManager(MemoryManagerT) {}

			KernelServiceConfigT GetKernelServiceConfig() { return KernelServiceConfigT(); }
			void SetKernelServiceConfig(KernelServiceConfigT) {}

			ThreadingCfgT GetThreadinfConfig() { return ThreadingCfgT(); }
			void SetThreadinfConfig(ThreadingCfgT) {}
		public:
			///<summary>Налаштування менеджера пам'яті</summary>
			MemoryManagerT Property(GetMemoryManager, SetMemoryManager) MemoryManager;
			///<summary>Налаштування сервісів ядра</summary>
			KernelServiceConfigT Property(GetKernelServiceConfig, SetKernelServiceConfig) KernelService;
			///<summary>Рантайм налаштування</summary>
			VirtualSettingDataT Property(GetVirtualSettingData, SetVirtualSettingDataT) VirtualSetting;
			///<summary>Налаштування багатопотоковості</summary>
			ThreadingCfgT Property(GetThreadinfConfig, SetThreadinfConfig) Threading;
		public:
			void SaveCurrentSettings();
		};


	}
}