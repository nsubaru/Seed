#pragma once
#include "Seed.Core.Threading.Threadables.Base.h"

#pragma region Unified thread pool threadables
namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				class ThreadableUnifiedThreadPoolIoTaskV2 final : public Seed::Core::Threading::IThreadable, public IDisposable {
				private:
					struct Data {
						UnifiedThreadPoolIoTask task;
						SynchronizeEvent event;
						EThreadabledStates state;
						EThreadabledStates secondState;
						Handle hThread;
						ID threadId;
						ImplSpecificT instance;

						RefCountT RefCount;

						Data(
							const UnifiedThreadPoolIoTask& task,
							const SynchronizeEvent& event,
							Handle hThread,
							ID threadId,
							ImplSpecificT instance
						) :
							task(task),
							event(event),
							state(EThreadabledStates::Executing),
							secondState(EThreadabledStates::Paused),
							hThread(hThread),
							threadId(threadId),
							instance(instance),
							RefCount(1)
						{}
					};
				private:
					Data* This;
				protected:
					virtual void Dispose() {}
				protected:
					ThreadableUnifiedThreadPoolIoTaskV2(
						const UnifiedThreadPoolIoTask& task,
						const SynchronizeEvent& event,
						Handle hThread,
						ID threadId,
						ImplSpecificT instance
					) :
						This(new Data(task, event, hThread, threadId, instance)) {}
				protected:
					friend ThreadableRef RegisterPoolIoThreadable(const UnifiedThreadPoolIoTask& task, const SynchronizeEvent& event, ImplSpecificT instance);
					friend void UnRegisterPoolIoThreadable(ThreadableRef&& ref);
				public:
					/// <summary>
					/// Вертає новий референс
					/// </summary>
					/// <returns></returns>
					virtual SmartPtr<IThreadable> MakeRef() const {
						return new ThreadableUnifiedThreadPoolIoTaskV2(*this);
					}
				public:
					/// <summary>
					/// Отримання імені сутності
					/// </summary>
					/// <returns>Рядкове представлення імені</returns>
					virtual const String& GetName() const {
						return This->task.Name;
					}
					/// <summary>
					/// Встановлення імені сутності
					/// </summary>
					/// <param name="NewName">Нове ім'я сутності</param>
					virtual void SetName(const String& NewName) {
						This->task.Name = NewName;
					}

					/// <summary>
					/// Ідентифікатор сутності
					/// </summary>
					/// <returns></returns>
					virtual ID GetID() const {
						return This->threadId;
					}

					/// <summary>
					/// Тип потокової сутності
					/// </summary>
					/// <returns>Тип потокової сутності</returns>
					virtual EThreadableTypes GetThreadableType() const {
						return EThreadableTypes::JobTask;
					}

					/// <summary>
					/// Стан потокової сутності
					/// </summary>
					/// <returns>Стан потокової сутності</returns>
					virtual EThreadabledStates GetThreadabledState() const {
						return This->state;
					}

					/// <summary>
					/// Заморозка виконання потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Suspend();
					/// <summary>
					/// Розмороження виконання потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Resume();
					/// <summary>
					/// Переривання виконання потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Interupt();
					/// <summary>
					/// Очікувння завершення потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Join();
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Join(Time::TimeSpan timeOut);
					/// <summary>
					/// Очікувння завершення потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join() const;
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join(Time::TimeSpan timeOut) const;
				public:
					RefCountClass(ThreadableUnifiedThreadPoolIoTaskV2, This);
				};
			}
		}
	}
}
#pragma endregion