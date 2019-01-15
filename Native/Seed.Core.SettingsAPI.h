#pragma once
#include "Seed.Core.Base.h"

namespace Seed {
	inline namespace Core {
		class CurrentSettings {
		public:
			struct SettingsData {
				struct MemoryManager {
					struct HeapCfg {
						Bytes StartSize;
						Bytes MaxSize;

						struct PosibleAdditional—hecks {
							UInt8 HEAP_FREE_CHECK : 1;
							UInt8 HEAP_TAIL_CHECK : 1;
							UInt8 HEAP_ARGS_CHECK : 1;
						};

						PosibleAdditional—hecks Additional—hecks;
					};

					HeapCfg DefaultHeap;
					HeapCfg SecuredHeap;
					HeapCfg ExceptionHeap;

					SizeT StringReserveSize;
				};

				MemoryManager MainMemoryManager;
			};
		public:
			class Memory {
			public:
				class DefaultHeap {
				public:
					static __forceinline Bytes GetStartSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.DefaultHeap.StartSize;
					}

					static __forceinline Bytes GetMaxSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.DefaultHeap.MaxSize;
					}

					static __forceinline SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks GetConfigs() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.DefaultHeap.Additional—hecks;
					}

					static __forceinline void SetStartSize(Bytes NewStartSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.DefaultHeap.StartSize = NewStartSize;
					}

					static __forceinline void SetMaxSize(Bytes NewMaxSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.DefaultHeap.MaxSize = NewMaxSize;
					}

					static __forceinline void SetAdditionalChecks(SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks NewAdditionalCheks) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.DefaultHeap.Additional—hecks = NewAdditionalCheks;
					}
				};

				class SecuredHeap {
				public:
					static __forceinline Bytes GetStartSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.SecuredHeap.StartSize;
					}

					static __forceinline Bytes GetMaxSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.SecuredHeap.MaxSize;
					}

					static __forceinline SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks GetConfigs() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.SecuredHeap.Additional—hecks;
					}

					static __forceinline void SetStartSize(Bytes NewStartSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.SecuredHeap.StartSize = NewStartSize;
					}

					static __forceinline void SetMaxSize(Bytes NewMaxSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.SecuredHeap.MaxSize = NewMaxSize;
					}

					static __forceinline void SetAdditionalChecks(SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks NewAdditionalCheks) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.SecuredHeap.Additional—hecks = NewAdditionalCheks;
					}
				};

				class ExceptionHeap {
				public:
					static __forceinline Bytes GetStartSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.ExceptionHeap.StartSize;
					}

					static __forceinline Bytes GetMaxSize() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.ExceptionHeap.MaxSize;
					}

					static __forceinline SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks GetConfigs() {
						extern SettingsData CurrentSettings;
						return CurrentSettings.MainMemoryManager.ExceptionHeap.Additional—hecks;
					}

					static __forceinline void SetStartSize(Bytes NewStartSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.ExceptionHeap.StartSize = NewStartSize;
					}

					static __forceinline void SetMaxSize(Bytes NewMaxSize) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.ExceptionHeap.MaxSize = NewMaxSize;
					}

					static __forceinline void SetAdditionalChecks(SettingsData::MemoryManager::HeapCfg::PosibleAdditional—hecks NewAdditionalCheks) {
						extern SettingsData CurrentSettings;
						CurrentSettings.MainMemoryManager.ExceptionHeap.Additional—hecks = NewAdditionalCheks;
					}
				};

				static __forceinline SizeT GetStringReserveSpace() {
					extern SettingsData CurrentSettings;
					return CurrentSettings.MainMemoryManager.StringReserveSize;
				}

				static __forceinline void SetStringReserveSpace(SizeT NewStringReserverSpace){
					extern SettingsData CurrentSettings;
					CurrentSettings.MainMemoryManager.StringReserveSize = NewStringReserverSpace;
				}
			};

			static void SaveCurrentSettings();
		};
	}
}