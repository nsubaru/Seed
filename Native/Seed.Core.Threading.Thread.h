#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Threading.Event.h"
#include "Seed.Core.Containers.Delegate.h"

#pragma warning(push)
#pragma warning(disable: 4251)

namespace Seed {
	inline namespace Core {
		namespace Threading {
			/// <summary>
			/// Класс потоку
			/// </summary>
			class Thread final : IDisposable{
			private:
				friend class ThreadableThread;
			private:
				struct Data {
					RefCountT RefCount;
					volatile Delegate delegate;
					volatile String name;
					volatile ID TID;
					volatile EThreadabledStates State;
					volatile EThreadabledStates SecondState;
					Handle hThread;
					SynchronizeEvent* AbortEvent;
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					ForceInline Data(const String& Name, Delegate&& delegate) :
						name(Name),
						delegate(MoveRValue(delegate)),
						State(EThreadabledStates::NotStarted),
						hThread(nullptr),
						TID(0),
						This(),
						AbortEvent(nullptr) ,
						RefCount(1)
					{}

					ForceInline Data(String&& Name, Delegate&& delegate) :
						name(MoveRValue(Name)),
						delegate(MoveRValue(delegate)),
						State(EThreadabledStates::NotStarted),
						hThread(nullptr),
						TID(0),
						This(),
						AbortEvent(nullptr),
						RefCount(1)
					{}
				};
			private:
				Data* This = nullptr;
			private:
				friend void RegisterThread(Thread* thread);
				friend void UnRegisterThread(Thread* thread);
				friend volatile Delegate& ThreadEntryPoint(Thread* thread);
				friend volatile EThreadabledStates* GetActiveState(Thread* thread);
				friend bool MainThread(Delegate&& delegate);

				friend void KillThread(Thread& target, UInt32 statusCode);
			private:
				void RefreshName();
			protected:
				virtual void Dispose() override final;
			public:
				/// <summary>
				/// Конструктор потоку з делегата
				/// </summary>
				/// <param name="delegate">Дія яку буде виконувати потік</param>
				ForceInline Thread(Delegate&& delegate) : This(new Data(L"Unnamed Thread", MoveRValue(delegate))) {}

				/// <summary>
				/// Конструктор з імені потоку і делегата
				/// </summary>
				/// <param name="Name">Ім'я потоку</param>
				/// <param name="delegate">Дія яку буде виконувати потік</param>
				ForceInline Thread(String&& Name, Delegate&& delegate) : This(new Data(MoveRValue(Name), MoveRValue(delegate))) {}

				/// <summary>
				/// Конструктор з імені потоку і делегата
				/// </summary>
				/// <param name="Name">Ім'я потоку</param>
				/// <param name="delegate">Дія яку буде виконувати потік</param>
				ForceInline Thread(const String& Name, Delegate&& delegate) : This(new Data(Name, MoveRValue(delegate))) {}
			public:
				/// <summary>
				/// Запуск потоку на виконання
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				Thread& Start();

				/// <summary>
				/// Очікування завершення виконання потоку
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				Thread& Join();

				/// <summary>
				/// Очікування завершення виконання потоку
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				Thread& Join(Time::TimeSpan timeOut);

				/// <summary>
				/// Очікування завершення виконання потоку
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				const Thread& Join() const;

				/// <summary>
				/// Очікування завершення виконання потоку
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				const Thread& Join(Time::TimeSpan timeOut) const;

				/// <summary>
				/// Переривання виконання потоку
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				Thread& Interupt();

				/// <summary>
				/// Замороження виконання потоку
				/// </summary>
				/// <returns></returns>
				Thread& Suspend();

				/// <summary>
				/// Продовження виконання потоку
				/// </summary>
				/// <returns></returns>
				Thread& Resume();
			public:
				/// <summary>
				/// Вертає результат виконання потоку
				/// </summary>
				template<typename T>
				ForceInline T GetThreadResult() {
					return This->delegate.GetResult<T>();
				}
			public:
				/// <summary>
				/// Конвертація у посилання на інтерфейс потоку
				/// </summary>
				/// <returns>Посилання на інтерфейс потоку</returns>
				ForceInline operator volatile IThreadable& () {
					if (This->This != nullptr) {
						return **This->This;
					}
					else {
						Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
					}
				}

				/// <summary>
				/// Конвертація у константне посилання на інтерфейс потоку
				/// </summary>
				/// <returns>Константне посилання на інтерфейс потоку</returns>
				ForceInline operator const volatile IThreadable& () const { 
					if (This->This != nullptr) {
						return **This->This;
					}
					else {
						Throw(NullPointException(ExceptionPosition, reinterpret_cast<SizeT>(nullptr), NullPointException::Types::Read));
					}
				}

				/// <summary>
				/// Конвертація в ідентифікатор потоку
				/// </summary>
				/// <returns>Ідентифікатор потоку</returns>
				ForceInline operator ID () const { return This->TID; }

				/// <summary>
				/// Конвертація в логічне значення
				/// </summary>
				/// <returns>Логічне значення яке вказує чи потік активний</returns>
				ForceInline operator Boolean () const { return This->TID != 0; }
			public:
				/// <summary>
				/// Встановлення імені потоку
				/// </summary>
				/// <param name="name">Нове ім'я потоку</param>
				ForceInline void SetName(const String& name) { const_cast<String&>(This->name) = name; RefreshName(); }

				/// <summary>
				/// Встановлення імені потоку
				/// </summary>
				/// <param name="name">Нове ім'я потоку</param>
				ForceInline void SetName(String&& name) { const_cast<String&>(This->name) = MoveRValue(name); RefreshName(); }

				/// <summary>
				/// Поточне ім'я потоку
				/// </summary>
				/// <returns>Рядкове представлення імені потоку</returns>
				ForceInline const String& GetName() const { return const_cast<String&>(This->name); }

				/// <summary>
				/// Стан потоку
				/// </summary>
				/// <returns>Поточний стан потоку</returns>
				ForceInline EThreadabledStates GetThreadState() const { return this->This->State; }

				/// <summary>
				/// Ідентифікатор потоку(номер)
				/// </summary>
				/// <returns>Числовий ідентифікатор потоку</returns>
				ForceInline ID GetTID() const { return this->This->TID; }

				/// <summary>
				/// Ім'я потоку
				/// </summary>
				String Property(GetName, SetName) Name;

				/// <summary>
				/// Стан потоку
				/// </summary>
				EThreadabledStates Property(GetThreadState) ThreadState;

				/// <summary>
				/// Ідентифікатор потоку
				/// </summary>
				ID Property(GetTID) TID;
			public:
				RefCountClass(Thread, This)
			};
		}
	}
}

#pragma warning(pop)