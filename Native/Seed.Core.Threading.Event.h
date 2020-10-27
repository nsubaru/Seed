#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			///<summary>Подія NT
			///<para>Об'єкт синхронізації потоків</para></summary>
			class SynchronizeEvent final : protected IDisposable
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
					ForceInline event_data(Boolean IsManualReset, const String& name)
						:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(name)
					{};
					///<summary>Конструктор з оптимізована отриманням дескриптора</summary>
					ForceInline event_data(Boolean IsManualReset, String&& name)
						:IsManualReset(IsManualReset), handle(nullptr), RefCount(1), Name(MoveRValue(name))
					{};
				};
				///<summary>Дані об'єкту події</summary>
				event_data* data;
				///<summary>Знищення останнього елемента</summary>
				SeedKernelCall virtual void Dispose() override final;
			public:
				///<summary>Конструктор</summary>
				SeedKernelCall SynchronizeEvent(Boolean IsManualReset = false);
				///<summary>Конструктор з іменем об'єкта</summary>
				SeedKernelCall SynchronizeEvent(const String& name, Boolean IsManualReset = false);
				///<summary>Конструктор з оптимізованою передачею імені об'єкта</summary>
				SeedKernelCall SynchronizeEvent(String&& name, Boolean IsManualReset = false);

				RefCountClass(SynchronizeEvent, data)

				///<summary>Очікування на подію</summary>
				SeedKernelCall void Wait() const;
				///<summary>Очікування з таймаутом</summary>
				SeedKernelCall void Wait(SizeT TimeOutMs) const;
				///<summary>Очікування з таймаутом</summary>
				SeedKernelCall void Wait(Time::TimeSpan timeOunt) const;

				///<summary>Подати імпульс на подію</summary>
				ForceInline void Signale() {
					Internals::PulseNtEvent(data->handle);
				}
				///<summary>Виставити true на подію</summary>
				ForceInline void Set() {
					Internals::SetNtEvent(data->handle);
				}
				///<summary>Виставити false на подію</summary>
				ForceInline void Reset() {
					Internals::ResetNtEvent(data->handle);
				}
			public:
				/// <summary>
				/// Явне перетворення до дескриптора ОС
				/// </summary>
				/// <returns>Вертає дескриптор системи який звязаний з даним об'єктом подіх</returns>
				InternalApiCall explicit operator Handle() {
					return data->handle;
				}
			public:
				/// <summary>
				/// Тип події(чи ручне скидання стану події чи функції ОС самі це роблять автоматично)
				/// </summary>
				/// <returns>True якщо подія автоматисно скидає свій стан при считувані</returns>
				Boolean GetIsManualReset() const {
					return data->IsManualReset;
				}
				/// <summary>
				/// Ім'я або пустий рядок якщо подія безімена
				/// </summary>
				/// <returns>Рядок з іменем або пустий якщо безіменні</returns>
				const String& GetName() const {
					return data->Name;
				}
			public:
				/// <summary>
				/// Тип події(чи ручне скидання стану події чи функції ОС самі це роблять автоматично)
				/// </summary>
				Boolean Property(GetIsManualReset) IsManualReset;
				/// <summary>
				/// Ім'я або пустий рядок якщо подія безімена
				/// </summary>
				String Property(GetName) Name;
			public:
				friend UInt64 WaitForAll(Containers::Array<SynchronizeEvent>&& events);
				friend UInt64 WaitForAny(Containers::Array<SynchronizeEvent>&& events);
				friend UInt64 WaitForAll(Containers::Array<SynchronizeEvent>&& events, SizeT timeOutMs);
				friend UInt64 WaitForAny(Containers::Array<SynchronizeEvent>&& events, SizeT timeOutMs);
				friend UInt64 WaitForAll(const Containers::Array<SynchronizeEvent>& events);
				friend UInt64 WaitForAny(const Containers::Array<SynchronizeEvent>& events);
				friend UInt64 WaitForAll(const Containers::Array<SynchronizeEvent>& events, SizeT timeOutMs);
				friend UInt64 WaitForAny(const Containers::Array<SynchronizeEvent>& events, SizeT timeOutMs);
			private:
				/// <summary>
				/// Реалізація очікування на декілька подій
				/// </summary>
				/// <param name="events">Масив подій</param>
				/// <param name="isWaitAll">Чи очікувати на всі, якщо false то очікування відбувається на будь-яку з</param>
				/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
				friend UInt64 WaitForMany(const Containers::Array<SynchronizeEvent>& events, Boolean isWaitAll, SizeT timeOutMs = Math::Max<SizeT>());
			};

			/// <summary>
			/// Очікування на всі події в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			ForceInline UInt64 WaitForAll(Containers::Array<SynchronizeEvent>&& events) {
				return WaitForMany(events, true);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			ForceInline UInt64 WaitForAny(Containers::Array<SynchronizeEvent>&& events) {
				return WaitForMany(events, false);
			}
			/// <summary>
			/// Очікування на всі події в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			ForceInline UInt64 WaitForAll(Containers::Array<SynchronizeEvent>&& events, SizeT timeOutMs) {
				return WaitForMany(events, true, timeOutMs);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			ForceInline UInt64 WaitForAny(Containers::Array<SynchronizeEvent>&& events, SizeT timeOutMs) {
				return WaitForMany(events, false, timeOutMs);
			}

			/// <summary>
			/// Очікування на всі події в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			ForceInline UInt64 WaitForAll(const Containers::Array<SynchronizeEvent>& events) {
				return WaitForMany(events, true);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій
			/// </summary>
			/// <param name="events">Масив подій</param>
			ForceInline UInt64 WaitForAny(const Containers::Array<SynchronizeEvent>& events) {
				return WaitForMany(events, false);
			}
			/// <summary>
			/// Очікування на всі події в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			ForceInline UInt64 WaitForAll(const Containers::Array<SynchronizeEvent>& events, SizeT timeOutMs) {
				return WaitForMany(events, true, timeOutMs);
			}
			/// <summary>
			/// Очікування на любу з подій в масиві подій з максимальним часом очікування
			/// </summary>
			/// <param name="events">Масив подій</param>
			/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
			ForceInline UInt64 WaitForAny(const Containers::Array<SynchronizeEvent>& events, SizeT timeOutMs) {
				return WaitForMany(events, false, timeOutMs);
			}
		}
	}
}

#pragma warning(pop)