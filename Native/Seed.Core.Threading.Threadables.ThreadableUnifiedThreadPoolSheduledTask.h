#pragma once
#include "Seed.Core.Threading.Threadables.Base.h"

#pragma region Unified thread pool threadables
namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				class ThreadableUnifiedThreadPoolSheduledTaskV2 final : public Seed::Core::Threading::IThreadable, public IDisposable {
				private:
					struct Data {
						UnifiedThreadPoolSheduledTask task;
						SynchronizeEvent event;
						EThreadabledStates state;
						EThreadabledStates secondState;
						Handle hThread;
						ID threadId;
						ImplSpecificT instance;

						RefCountT RefCount;

						Data(
							const UnifiedThreadPoolSheduledTask& task,
							SynchronizeEvent event,
							Handle hThread,
							ID threadId,
							ImplSpecificT instance
						) :
							task(task),
							event(event),
							state(),
							secondState(),
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
					ThreadableUnifiedThreadPoolSheduledTaskV2(
						const UnifiedThreadPoolSheduledTask& task,
						const SynchronizeEvent event,
						Handle hThread,
						ID threadId,
						ImplSpecificT instance
					) :
						This(new Data(task, event, hThread, threadId, instance)) {}
				protected:
					friend ThreadableRef RegisterPoolSheduledThreadable(const UnifiedThreadPoolSheduledTask& task, ImplSpecificT instance);
					friend void UnRegisterPoolSheduledThreadable(ThreadableRef&& ref);
				public:
					/// <summary>
					/// Вертає новий референс
					/// </summary>
					/// <returns></returns>
					virtual SmartPtr<IThreadable> MakeRef() const {
						return new ThreadableUnifiedThreadPoolSheduledTaskV2(*this);
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
					/// Стек викликів даної потокової сутності
					/// </summary>
					/// <returns></returns>
					virtual Containers::Array<String> GetCallStack() const {
						Throw(NotImplementedException(ExceptionPosition));
					}

					/// <summary>
					/// Вертає батьківську потокову сутність якщо така є
					/// </summary>
					/// <returns></returns>
					virtual IThreadable& GetParrentThreadable() const {
						Throw(NullPointException(ExceptionPosition));
					}

					/// <summary>
					/// Вказує чи є батьківська потокова сутність
					/// </summary>
					/// <returns></returns>
					virtual Boolean IsHaveParrentThreadable() const {
						return false;
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
					virtual IThreadable& Join() {
						This->event.Wait();
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Join(Time::DateTime timeOut) {
						This->event.Wait(Time::TimeSpan(timeOut.Ticks));
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join() const {
						This->event.Wait();
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join(Time::DateTime timeOut) const {
						This->event.Wait(Time::TimeSpan(timeOut.Ticks));
						return *this;
					}
				public:
					RefCountClass(ThreadableUnifiedThreadPoolSheduledTaskV2, This);
				};
			}
		}
	}
}
#pragma endregion