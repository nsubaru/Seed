#pragma once
#include "Seed.Core.Threading.Base.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>Подія NT
			///<para>Об'єкт синхронізації потоків</para></summary>
			class SeedKernelCall Event final : protected IDisposable
			{
				///<summary>Дані події</summary>
				struct event_data
				{
				///<summary>Тип</summary>
				Boolean IsManualReset;
				///<summary>Дескриптор</summary>
				Handle handle;
				///<summary>Кількість силок на об'єкт</summary>
				RefCountT RefCount;
				///<summary>Ім'я обєкта події в об'єктній системі NT, якщо немає то рядок пустий</summary>
				String Name;

				///<summary>Конструктор з отриманням дескриптора</summary>
				event_data(Boolean IsManualReset, const String& name)
					:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(name)
				{};
				///<summary>Конструктор з оптимізована отриманням дескриптора</summary>
				event_data(Boolean IsManualReset, String&& name)
					:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(MoveRValue(name))
				{};
			};
			///<summary>Дані об'єкту події</summary>
			event_data* data;
			///<summary>Знищення останнього елемента</summary>
			virtual void Dispose() override final;
		public:
			///<summary>Конструктор</summary>
			Event(Boolean IsManualReset = false);
			///<summary>Конструктор з іменем об'єкта</summary>
			Event(const String& name, Boolean IsManualReset = false);
			///<summary>Конструктор з оптимізованою передачею імені об'єкта</summary>
			Event(String&& name, Boolean IsManualReset = false);

			RefCountClass(Event, data)

			///<summary>Очікування на подію</summary>
			void Wait();
			///<summary>Очікування з таймаутом</summary>
			void Wait(SizeT TimeOutMs);

			///<summary>Подати імпульс на подію</summary>
			void Signale() {
				extern void PulseNtEvent(void* handle);
				PulseNtEvent(data->handle);
			}
			///<summary>Виставити true на подію</summary>
			void Set() {
				extern void SetNtEvent(void* handle);
				SetNtEvent(data->handle);
			}
			///<summary>Виставити false на подію</summary>
			void Reset() {
				extern void ResetNtEvent(void* handle);
				ResetNtEvent(data->handle);
			}
			};
		}
	}
}