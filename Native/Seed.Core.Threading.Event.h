#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.Internal.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			/// <summary> 
			/// Подія NT
			/// <para> Об'єкт синхронізації потоків </para>
			/// </summary>
			class SynchronizeEvent final : protected IDisposable
			{
				struct Data
				{
					Boolean isManualReset;
					Handle handle;
					RefCountT RefCount;
					String Name;

					ForceInline Data(Boolean isManualReset, const String& name)
						:isManualReset(isManualReset), handle(nullptr), RefCount(1), Name(name)
					{};

					ForceInline Data(Boolean isManualReset, String&& name)
						:isManualReset(isManualReset), handle(nullptr), RefCount(1), Name(MoveRValue(name))
					{};
				};
			private:
				Data* data;

				virtual void Dispose() override final;
			public:
				/// <summary>
				/// Конструктор
				/// </summary>
				/// <param name="isManualReset">Вказує чи подія автоматично скидається при очікувані</param>
				SynchronizeEvent(Boolean isManualReset = false);
				
				/// <summary>
				/// Конструктор з іменем об'єкта
				/// </summary>
				/// <param name="name">Ім'я об'єкту(також це ім'я об'єкту в ядрі Nt)</param>
				/// <param name="isManualReset">Вказує чи подія автоматично скидається при очікувані</param>
				SynchronizeEvent(const String& name, Boolean isManualReset = false);
				
				/// <summary>
				/// Конструктор з іменем об'єкта
				/// </summary>
				/// <param name="name">Ім'я об'єкту(також це ім'я об'єкту в ядрі Nt)</param>
				/// <param name="isManualReset">Вказує чи подія автоматично скидається при очікувані</param>
				SynchronizeEvent(String&& name, Boolean isManualReset = false);
			public:
				/// <summary>
				/// Очікування на подію
				/// </summary>
				void Wait() const;

				/// <summary>
				/// Очікування з таймаутом
				/// </summary>
				/// <param name="TimeOutMs">Таймаут в кількості мілісекунд</param>
				void Wait(SizeT TimeOutMs) const;
				
				/// <summary>
				/// Очікування з таймаутом
				/// </summary>
				/// <param name="timeOunt">Таймаут</param>
				void Wait(Time::TimeSpan timeOunt) const;
			public:
				/// <summary>
				/// Подає імпульс на подію
				/// </summary>
				ForceInline void Signale() {
					Internals::PulseNtEvent(data->handle);
				}
				
				/// <summary>
				/// Виставиляє true на подію
				/// </summary>
				ForceInline void Set() {
					Internals::SetNtEvent(data->handle);
				}
				
				/// <summary>
				/// Виставляє false на подію
				/// </summary>
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
				/// <returns> True якщо подія автоматисно скидає свій стан при считувані </returns>
				Boolean GetIsManualReset() const {
					return data->isManualReset;
				}

				/// <summary>
				/// Ім'я або пустий рядок якщо подія безімена
				/// </summary>
				/// <returns> Рядок з іменем або пустий якщо безіменні </returns>
				const String& GetName() const {
					return data->Name;
				}
			public:
				/// <summary>
				/// Тип події(чи ручне скидання стану події чи функції ОС самі це роблять автоматично)
				/// </summary>
				Boolean Property(GetIsManualReset) isManualReset;
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
			public:
				/// <summary>
				/// Реалізація очікування на декілька подій
				/// </summary>
				/// <param name="events">Масив подій</param>
				/// <param name="isWaitAll">Чи очікувати на всі, якщо false то очікування відбувається на будь-яку з</param>
				/// <param name="timeOutMs">Час очікування або SizeT max якщо потрібно очікувати без таймауту</param>
				friend UInt64 WaitForMany(const Containers::Array<SynchronizeEvent>& events, Boolean isWaitAll, SizeT timeOutMs = Math::Max<SizeT>());
			public:
				RefCountClass(SynchronizeEvent, data)
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