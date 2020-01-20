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
				void Wait() const;
				///<summary>Очікування з таймаутом</summary>
				void Wait(SizeT TimeOutMs) const;

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

				friend void WaitForAll(Containers::Array<Event>&& events);
				friend void WaitForAny(Containers::Array<Event>&& events);
				friend void WaitForAll(Containers::Array<Event>&& events, SizeT timeOutMs);
				friend void WaitForAny(Containers::Array<Event>&& events, SizeT timeOutMs);
				friend void WaitForAll(const Containers::Array<Event>& events);
				friend void WaitForAny(const Containers::Array<Event>& events);
				friend void WaitForAll(const Containers::Array<Event>& events, SizeT timeOutMs);
				friend void WaitForAny(const Containers::Array<Event>& events, SizeT timeOutMs);
			private:
				/// <summary>
				/// Реалізація очікування на декілька подій
				/// </summary>
				/// <param name="events">Масив подій</param>
				/// <param name="isWaitAll">Чи очікувати на всі, якщо false то очікування відбувається на будь-яку з</param>
				/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
				friend void WaitForMany(const Containers::Array<Event>& events, Boolean isWaitAll, SizeT timeOutMs = Math::Max<SizeT>());
			};

			/// <summary>
			/// Очікування на всі події в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			forceinline void WaitForAll(Containers::Array<Event>&& events) {
				WaitForMany(events, true);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			forceinline void WaitForAny(Containers::Array<Event>&& events) {
				WaitForMany(events, false);
			}
			/// <summary>
			/// Очікування на всі події в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			forceinline void WaitForAll(Containers::Array<Event>&& events, SizeT timeOutMs) {
				WaitForMany(events, true, timeOutMs);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			forceinline void WaitForAny(Containers::Array<Event>&& events, SizeT timeOutMs) {
				WaitForMany(events, false, timeOutMs);
			}

			/// <summary>
			/// Очікування на всі події в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			forceinline void WaitForAll(const Containers::Array<Event>& events) {
				WaitForMany(events, true);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			forceinline void WaitForAny(const Containers::Array<Event>& events) {
				WaitForMany(events, false);
			}
			/// <summary>
			/// Очікування на всі події в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			forceinline void WaitForAll(const Containers::Array<Event>& events, SizeT timeOutMs) {
				WaitForMany(events, true, timeOutMs);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			forceinline void WaitForAny(const Containers::Array<Event>& events, SizeT timeOutMs) {
				WaitForMany(events, false, timeOutMs);
			}
		}
	}
}