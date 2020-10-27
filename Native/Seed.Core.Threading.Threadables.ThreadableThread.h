#pragma once
#include "Seed.Core.Threading.Threadables.Base.h"

#pragma region Unified thread pool threadables
namespace Seed {
	inline namespace Core {
		namespace Threading {
			namespace Threadables {
				class ThreadableThread final : public Seed::Core::Threading::IThreadable {
				private:
					Thread thread;
				protected:
					ThreadableThread(const Thread& thread) : thread(thread) {}
				protected:
					friend ThreadableRef RegisterThreadThreadable(const Thread& thread, volatile EThreadabledStates* State);
					friend void UnRegisterThreadThreadable(ThreadableRef&& ref);
				public:
					ThreadableThread(const ThreadableThread& from) : thread(from.thread) {}
					ThreadableThread(ThreadableThread&& from) : thread(MoveRValue(from.thread)) {}
					ThreadableThread& operator=(const ThreadableThread& from) {
						this->~ThreadableThread();
						this->ThreadableThread::ThreadableThread(from);
						return *this;
					}
					ThreadableThread& operator=(ThreadableThread&& from) {
						this->~ThreadableThread();
						this->ThreadableThread::ThreadableThread(MoveRValue(from));
						return *this;
					}
				public:
					/// <summary>
					/// Вертає новий референс
					/// </summary>
					/// <returns></returns>
					virtual SmartPtr<IThreadable> MakeRef() const {
						return new ThreadableThread(*this);
					}
				public:
					/// <summary>
					/// Отримання імені сутності
					/// </summary>
					/// <returns>Рядкове представлення імені</returns>
					virtual const String& GetName() const {
						return thread.Name;
					}
					/// <summary>
					/// Встановлення імені сутності
					/// </summary>
					/// <param name="NewName">Нове ім'я сутності</param>
					virtual void SetName(const String& NewName) {
						thread.Name = NewName;
					}

					/// <summary>
					/// Ідентифікатор сутності
					/// </summary>
					/// <returns></returns>
					virtual ID GetID() const {
						return thread.TID;
					}

					/// <summary>
					/// Тип потокової сутності
					/// </summary>
					/// <returns>Тип потокової сутності</returns>
					virtual EThreadableTypes GetThreadableType() const {
						return EThreadableTypes::Thread;
					}

					/// <summary>
					/// Стан потокової сутності
					/// </summary>
					/// <returns>Стан потокової сутності</returns>
					virtual EThreadabledStates GetThreadabledState() const {
						return thread.ThreadState;
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
					virtual IThreadable& Suspend() {
						thread.Suspend();
						return *this;
					}
					/// <summary>
					/// Розмороження виконання потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Resume() {
						thread.Resume();
						return *this;
					}
					/// <summary>
					/// Переривання виконання потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Interupt() {
						thread.Interupt();
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Join() {
						thread.Join();
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual IThreadable& Join(Time::DateTime timeOut) {
						thread.Join(timeOut);
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності
					/// </summary>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join() const {
						thread.Join();
						return *this;
					}
					/// <summary>
					/// Очікувння завершення потокової сутності з таймаутом
					/// </summary>
					/// <param name="timeOut">Максимальний час очікування</param>
					/// <returns>Посилання на об'єкт</returns>
					virtual const IThreadable& Join(Time::DateTime timeOut) const {
						thread.Join(timeOut);
						return *this;
					}
				};
			}
		}
	}
}
#pragma endregion