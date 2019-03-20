#pragma once
#include "Seed.Core.Base.h"
#include "Seed.Core.SettingsAPI.h"

namespace Seed {
	inline namespace Core {
		///<summary>Конфігурації рантайму  Рем<summary>
		namespace Settings {
			constexpr Char SettingsFileName[16] = L"CoreSettings";
			///<summary>Стандартні налаштування рантайму Seed</summary>
			namespace Default {
				__forceinline CurrentSettings::SettingsData InitDefaultSettings() {
					CurrentSettings::SettingsData DefaultSettings;
					//Менеджер пам'яті
					{
						//Секція стандартного алокатора пам'яті
						DefaultSettings.MainMemoryManager.DefaultHeap.StartSize = 2Mb;
						DefaultSettings.MainMemoryManager.DefaultHeap.MaxSize = 0Mb;
						DefaultSettings.MainMemoryManager.DefaultHeap.AdditionalСhecks = { 0 };
						//Секція захищеного алокатора пам'яті
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.StartSize = 1Mb;
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.MaxSize = 0Mb;
						DefaultSettings.MainMemoryManager.SensitiveDataHeap.AdditionalСhecks = { 0 };
						//Секція алокатора пам'яті для обробника помилок
						DefaultSettings.MainMemoryManager.ExceptionHeap.StartSize = 10Mb;
						DefaultSettings.MainMemoryManager.ExceptionHeap.MaxSize = 0Mb;
						DefaultSettings.MainMemoryManager.ExceptionHeap.AdditionalСhecks = { 0 };
					}

					return DefaultSettings;
				}
			}
		}
	}
}