#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class UnifiedThreadPool;

			class IThreadPoolObject;

			class UnifiedThreadPoolJob;
			class UnifiedThreadPoolScheduledTask;
			class UnifiedThreadPoolScheduledTaskBuilder;
			class UnifiedThreadPoolIoTask;
		}

		namespace IO {
			class IRPBase;
			class IAsyncIOBase;
		}

		namespace IO {
			/// <summary>
			/// Інтерфейс колбеку помилки I/O операції 
			/// </summary>
			class IExceptionCallback : protected IDisposable {
			public:
				/// <summary>
				/// Викликається з кодом помилки, якщо операція завершилась не успішно
				/// </summary>
				/// <param name="errorCode">Код помилки</param>
				virtual void OnError(UInt32 errorCode) = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~IExceptionCallback() = 0 {}
			};

			/// <summary>
			/// Інтерфейс колбеку завершення операції читання
			/// </summary>
			/// <typeparam name="TDataType">Тип блоку даних</typeparam>
			template<typename TDataType = Containers::Blob>
			class IFinishReadCallback : public virtual IExceptionCallback {
			public:
				/// <summary>
				/// Викликається після успішного завершення операції читання
				/// </summary>
				/// <param name="byteTransfered">Вказує кількість зчитаних байтів</param>
				/// <param name="data">Буфер даних</param>
				virtual void OnFinishRead(SizeT byteTransfered, TDataType&& data) volatile = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~IFinishReadCallback() = 0 {}
			};

			/// <summary>
			/// Інтерфейс колбеку завершення операції запису
			/// </summary>
			/// <typeparam name="TDataType">Тип блоку даних</typeparam>
			template<typename TDataType = Containers::Blob>
			class IFinishWriteCallback : public virtual IExceptionCallback {
			public:
				/// <summary>
				/// Викликається після успішного завершення операції запису
				/// </summary>
				/// <param name="byteTransfered">Вказує кількість зчитаних байтів</param>
				/// <param name="data">Буфер даних</param>
				virtual void OnWriteFinish(SizeT byteTransfered, const TDataType&& data) volatile = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~IFinishWriteCallback() = 0 {}
			};

			template<typename TAsyncObject>
			class IFinishConnectCallback : public virtual IExceptionCallback {
			public:
				virtual void OnConnectFinish(TAsyncObject&& connectionObject) volatile = 0;
			public:
				virtual ~IFinishConnectCallback() = 0 {}
			};
		}

		namespace Threading {
			class IThreadPoolObject;

			class UnifiedThreadPoolJob;
			class UnifiedThreadPoolScheduledTask;
			class UnifiedThreadPoolScheduledTaskBuilder;
			class UnifiedThreadPoolIoTask;

			class UnifiedThreadPoolBuilder;

			/// <summary>
			/// Колбек таймера задачі в пулі
			/// </summary>
			class IUnifiedThreadPoolShedulerCallback : protected IDisposable {
			public:
				/// <summary>
				/// Викликається при настані заданого часу
				/// </summary>
				virtual void Shedule() = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~IUnifiedThreadPoolShedulerCallback() = 0 {}
			};

			/// <summary>
			/// Задача в пулі задач
			/// </summary>
			class UnifiedThreadPoolJob final : protected IDisposable {
			public:
				friend class UnifiedThreadPool;
				friend class ThreadableUnifiedThreadPoolJob;
			private:
				struct Data {
					ImplSpecificT pOwnerPool;
					String jobName;
					Delegate action;
					ImplSpecificT hWork;
					ImplSpecificT hEvent;

					Containers::ConcurencyList<UnifiedThreadPoolJob>::Iterator poolQueueRef;

					RefCountT RefCount;

					Data(
						ImplSpecificT pOwnerPool,
						String&& jobName,
						Delegate&& action,
						ImplSpecificT hEvent
					) :
						pOwnerPool(pOwnerPool),
						jobName(MoveRValue(jobName)),
						action(MoveRValue(action)),
						hWork(nullptr),
						hEvent(hEvent),
						poolQueueRef(),
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override;
			private:
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolJob* jobObj);
				friend Threadables::ThreadableRef RegisterThreadable(UnifiedThreadPoolJob* thread, ImplSpecificT callbackInstance);
				friend void UnRegisterThreadable(UnifiedThreadPoolJob* thread, Threadables::ThreadableRef&& ref);
				friend Delegate& GetJobEntryPoint(UnifiedThreadPoolJob* jobObj);

				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj);
				friend void UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj);
			private:
				UnifiedThreadPoolJob(Data* This) : This(This) {}
			public:
				/// <summary>
				/// Вертає ім'я задачі
				/// </summary>
				/// <returns></returns>
				ForceInline const String& GetName() {
					return This->jobName;
				}

				/// <summary>
				/// Встановлює ім'я задачі
				/// </summary>
				/// <param name="newName">Нове ім'я задачі</param>
				ForceInline void SetName(const String& newName) {
					This->jobName = newName;
				}

				/// <summary>
				/// Встановлює ім'я задачі
				/// </summary>
				/// <param name="newName">Нове ім'я задачі</param>
				ForceInline void SetName(String&& newName) {
					This->jobName = MoveRValue(newName);
				}
			public:
				/// <summary>
				/// Вказує чи задача вже виконана
				/// </summary>
				/// <returns>true якщо задача виконана</returns>
				ForceInline Boolean GetIsAlredyExecuted() const {
					return This->action.GetIsEnded();
				}

				/// <summary>
				/// Вказує чи задача виконується
				/// </summary>
				/// <returns>true якщо задача в процесі виконання</returns>
				ForceInline Boolean GetIsStartedExecution() const {
					return This->action.GetIsStarted();
				}

				/// <summary>
				/// Вертає пул потоків в якому зареєстрована задача
				/// </summary>
				/// <returns>Об'єкт пул потоків</returns>
				UnifiedThreadPool GetOwnerPool() const;
			public:
				/// <summary>
				/// Ім'я задачі
				/// </summary>
				String Property(GetName, SetName) Name;

				/// <summary>
				/// Вказує чи задача вже виконана
				/// </summary>
				Boolean Property(GetIsAlredyExecuted) IsAlredyExecuted;

				/// <summary>
				/// Вказує чи задача виконується
				/// </summary>
				Boolean Property(GetIsStartedExecution) IsStartedExecution;
			public:
				/// <summary>
				/// Очікування завершення задачі
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				UnifiedThreadPoolJob& Join();

				/// <summary>
				/// Очікування завершення задачі з таймаутом
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns></returns>
				UnifiedThreadPoolJob& Join(Time::TimeSpan timeOut);

				/// <summary>
				/// Очікування завершення задачі
				/// </summary>
				/// <returns>Посилання на поточний об'єкт</returns>
				const UnifiedThreadPoolJob& Join() const;

				/// <summary>
				/// Очікування завершення задачі з таймаутом
				/// </summary>
				/// <param name="timeOut">Таймаут очікування</param>
				/// <returns></returns>
				const UnifiedThreadPoolJob& Join(Time::TimeSpan timeOut) const;
			public:
				RefCountClass(UnifiedThreadPoolJob, This);
			};

			/// <summary>
			/// Таймер в пулі задач
			/// </summary>
			class UnifiedThreadPoolScheduledTask final : protected IDisposable {
			private:
				friend class UnifiedThreadPoolScheduledTaskBuilder;
				friend class ThreadableUnifiedThreadPoolScheduledTask;
			private:
				struct Data {
					String taskName;
					SmartPtr<IUnifiedThreadPoolShedulerCallback> callback;
					
					ImplSpecificT ownerPool;
					Containers::ConcurencyList<UnifiedThreadPoolScheduledTask>::Iterator poolQueueRef;
					
					
					Time::DateTime startTime;
					Time::TimeSpan intervalLenght;
					Time::DateTime lastStartTime;

					RefCountT intervalsCount;
					RefCountT executedCount;
					RefCountT remainingCount;
					Int32 timerProximity;

					RefCountT activeInstanceRefCount;

					Handle hTask;

					RefCountT RefCount;

					Data(String&& taskName,
						SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback,
						ImplSpecificT ownerPool,
						Time::DateTime startTime,
						Time::TimeSpan intervalLenght,
						UInt64 intervalsCount,
						Int32 timerProximity	
					) :
						taskName(MoveRValue(taskName)),
						callback(MoveRValue(callback)),
						ownerPool(ownerPool),
						poolQueueRef(),
						startTime(startTime),
						intervalLenght(intervalLenght),
						lastStartTime(0),
						intervalsCount(intervalsCount),
						executedCount(0),
						remainingCount(intervalsCount),
						timerProximity(timerProximity),
						activeInstanceRefCount(0),
						hTask(nullptr),
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override;
			private:
				ForceInline UnifiedThreadPoolScheduledTask(Data* This) : This(This) {}
			private:
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolScheduledTask* task);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolScheduledTask* task);
				friend SmartPtr<IUnifiedThreadPoolShedulerCallback>& GetJobEntryPoint(UnifiedThreadPoolScheduledTask* task);
				friend void ZeroingTaskHadle(UnifiedThreadPoolScheduledTask* thread);
			private:
				friend Threadables::ThreadableRef RegisterThreadable(UnifiedThreadPoolScheduledTask* thread, ImplSpecificT callbackInstance);
				friend void UnRegisterThreadable(UnifiedThreadPoolScheduledTask* thread, Threadables::ThreadableRef&& ref);
				friend void UnRegisterTaskFromPoolQueue(UnifiedThreadPoolScheduledTask* thread);
			public:
				/// <summary>
				/// Вертай ім'я таймера
				/// </summary>
				/// <returns>Тескстове ім'я таймера</returns>
				ForceInline const String& GetTaskName() const { return This->taskName; }

				/// <summary>
				/// Встановлює нове ім'я таймера
				/// </summary>
				/// <param name="newTaskName">Нове ім'я таймера</param>
				void SetName(String&& newTaskName);

				/// <summary>
				/// Встановлює нове ім'я таймера
				/// </summary>
				/// <param name="newTaskName">Нове ім'я таймера</param>
				ForceInline void SetName(const String& newTaskName) { SetName(String(newTaskName)); }
			public:
				/// <summary>
				/// Ім'я таймера
				/// </summary>
				String Property(GetTaskName, SetName) Name;
			public:
				/// <summary>
				/// Вертає кількість пройдених циклів
				/// </summary>
				/// <returns>Кількість пройдениз циклів</returns>
				ForceInline SizeT GetExecutedCount() const { return This->executedCount; }

				/// <summary>
				/// Вертає остані час виклику таймера
				/// </summary>
				/// <returns>Останій час вилику таймера</returns>
				ForceInline Time::DateTime GetLastStartTime() const { return This->lastStartTime; }

				/// <summary>
				/// Вертає чм активна задача таймера
				/// </summary>
				/// <returns>Чм активна задача таймера</returns>
				ForceInline Boolean GetIsActive() const { return This->activeInstanceRefCount != 0; }

				/// <summary>
				/// Вертає поточну кількість актичних екземплярів задачі таймера
				/// </summary>
				/// <returns>Поточна кількість актичних екземплярів задачі таймера</returns>
				ForceInline UInt64 GetActiveInstancesCount() const { return This->activeInstanceRefCount; }

				/// <summary>
				/// Вертає пул задач який обробляє таймер
				/// </summary>
				/// <returns>Пул задач який обробляє таймер</returns>
				UnifiedThreadPool GetOwnerPool() const;

				/// <summary>
				/// Вертає точність таймера
				/// </summary>
				/// <returns>Точність таймера</returns>
				ForceInline Time::TimeSpan GetTimerProximity() const { return Time::TimeSpan(This->timerProximity); }

				/// <summary>
				/// Вказує перший час запуску
				/// </summary>
				/// <returns>Час першого запуску</returns>
				ForceInline Time::DateTime GetFirstStartTime() const { return This->startTime; }

				/// <summary>
				/// Вертає екземплря колбеку
				/// </summary>
				/// <returns>Екземпляр колбеку</returns>
				ForceInline const SmartPtr<IUnifiedThreadPoolShedulerCallback>& GetCallbackObject() const { return This->callback; }
			public:
				/// <summary>
				/// Кількість пройдених циклів
				/// </summary>
				SizeT Property(GetExecutedCount) ExecutedCount;

				/// <summary>
				/// Останій час вилику таймера
				/// </summary>
				Time::DateTime Property(GetLastStartTime) LastStartTime;

				/// <summary>
				/// Чм активна задача таймера зараз
				/// </summary>
				Boolean Property(GetIsActive) IsActive;

				/// <summary>
				/// Вертає пул задач який обробляє таймер
				/// </summary>
				UnifiedThreadPool Property(OwnerPool) OwnerPool;

				/// <summary>
				/// Точність таймера
				/// </summary>
				Time::TimeSpan Property(GetTimerProximity) TimerProximity;

				/// <summary>
				/// Час першого запуску
				/// </summary>
				Time::DateTime Property(GetFirstStartTime)FirstStartTime;

				/// <summary>
				/// Екземпляр колбеку
				/// </summary>
				SmartPtr<IUnifiedThreadPoolShedulerCallback> Property(GetCallbackObject) CallbackObject;
			public:
				/// <summary>
				/// Зупиняє таймер
				/// </summary>
				/// <returns></returns>
				void StopSheduling()&&;
			public:
				/// <summary>
				/// Максиальний інтервал між запусками задач таймера
				/// </summary>
				static constexpr auto MaxIntervalLenght = Time::TimeSpan().AddMiliSeconds(Math::Max<Int32>());
			public:
				RefCountClass(UnifiedThreadPoolScheduledTask, This);
			};


			/// <summary>
			/// Об'єкт вводу/виводу пулу задач
			/// </summary>
			class UnifiedThreadPoolIoTask final : protected IDisposable {
			public:
				friend class IO::IAsyncIOBase;
			private:
				struct Data {
					Handle hFile;
					Containers::ConcurencyList<UnifiedThreadPoolIoTask>::Iterator poolRef;
					ImplSpecificT ownerPool;
					ImplSpecificT hIoPoolObj;
					String ioTaskName;

					RefCountT RefCount;

					ForceInline Data(Handle hFile, ImplSpecificT ownerPool) :
						hFile(hFile), ownerPool(ownerPool), poolRef(), hIoPoolObj(nullptr), RefCount(1), ioTaskName("I\\O worker") {}
				};
			private:
				Data* This;
			protected:
				friend void DestroyIoPoolObj(UnifiedThreadPoolIoTask* task);
			protected:
				friend Threadables::ThreadableRef RegisterThreadable(const UnifiedThreadPoolIoTask& task, const SynchronizeEvent& event, ImplSpecificT instance);
				friend void UnRegisterThreadable(const UnifiedThreadPoolIoTask& task, Threadables::ThreadableRef&& ref);
			protected:
				virtual void Dispose() override;
			protected:
				ForceInline UnifiedThreadPoolIoTask(Data* This) : This(MoveRValue(This)) {}
			public:
				/// <summary>
				/// Вертає ім'я задачі
				/// </summary>
				/// <returns>Ім'я задачі</returns>
				ForceInline const String& GetName() const { return This->ioTaskName; }

				/// <summary>
				/// Встановлює нове ім'я задачі
				/// </summary>
				/// <param name="newName">Нове ім'я задачі</param>
				ForceInline void SetName(const String& newName) { This->ioTaskName = newName; }

				/// <summary>
				/// Встановлює нове ім'я задачі
				/// </summary>
				/// <param name="newName">Нове ім'я задачі</param>
				ForceInline void SetName(String&& newName) { This->ioTaskName = MoveRValue(newName); }
			public:
				/// <summary>
				/// Ім'я задачі
				/// </summary>
				String Property(GetName, SetName) Name;
			public:
				RefCountClass(UnifiedThreadPoolIoTask, This);
			};

			/// <summary>
			/// Пул потоків
			/// </summary>
			class UnifiedThreadPool final : protected IDisposable {
			public:
				/// <summary>
				/// Пріоритет задач пулу потоків
				/// </summary>
				enum class EPoolThreadsPriority {
					/// <summary>
					/// Низький пріоритет
					/// </summary>
					Low, 
					/// <summary>
					/// Середній(стандартний) пріоритет
					/// </summary>
					Medium, 
					/// <summary>
					/// Високий пріортет
					/// </summary>
					High
				};
			private:
				friend class UnifiedThreadPoolJob;
				friend class UnifiedThreadPoolScheduledTask;
				friend class UnifiedThreadPoolScheduledTaskBuilder;
				friend class UnifiedThreadPoolIoTask;

				friend class UnifiedThreadPoolBuilder;

				friend class IO::IAsyncIOBase;
			private:
				struct PoolConfigureation {
					ImplSpecificT CallbackEnviroment = nullptr;
					ImplSpecificT ClenupGroup = nullptr;
					EPoolThreadsPriority Priority = EPoolThreadsPriority::Medium;
					SRWLock PoolLock = {};
				};

				struct ImplementationSpecificData {
					ImplSpecificT PoolHandle;

					PoolConfigureation JobConfiguration;
					PoolConfigureation TaskConfiguration;
					PoolConfigureation IrpConfiguration;
				};

				struct Data {
					SizeT MinThreadCount;
					SizeT MaxThreadCount;

					Containers::ConcurencyList<UnifiedThreadPoolJob> JobsQueue;
					Containers::ConcurencyList<UnifiedThreadPoolScheduledTask> TaskQueue;
					Containers::ConcurencyList<UnifiedThreadPoolIoTask> IrpQueue;

					String threadPoolName;

					volatile Boolean isPoolTermination;

					ImplementationSpecificData handlesData;

					RefCountT RefCount;

					Data(SizeT minThreadCount, SizeT maxThreadCount, String&& threadPoolName, ImplementationSpecificData&& handlesData) :
						MinThreadCount(minThreadCount), MaxThreadCount(maxThreadCount),
						JobsQueue(), TaskQueue(), IrpQueue(), 
						threadPoolName(MoveRValue(threadPoolName)),
						handlesData(MoveRValue(handlesData)),
						isPoolTermination(false),
						RefCount(0)
					{}
				};
			private:
				Data* This;
			protected:
				ForceInline virtual void Dispose() override {
					DestroyConfiguration(This->handlesData.JobConfiguration);
					DestroyConfiguration(This->handlesData.TaskConfiguration);
					DestroyConfiguration(This->handlesData.IrpConfiguration);

					DestroyPoolObject(This->handlesData.PoolHandle);
				}
			private:
				static void DestroyPoolObject(ImplSpecificT PoolHandle);
				static void DestroyConfiguration(PoolConfigureation& poolConfiguration);
			private:
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolJob* jobObj);
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolScheduledTask* task);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolScheduledTask* task);

				friend void UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj);
				friend void UnRegisterTaskFromPoolQueue(UnifiedThreadPoolScheduledTask* thread);
			private:
				ForceInline UnifiedThreadPool(Data*& This) :This(This) { ++This->RefCount;  }
			public:
				/// <summary>
				/// Запускає задачі в пулі потоків
				/// </summary>
				/// <param name="jobName">Ім'я задачі</param>
				/// <param name="action">Делегат задачі</param>
				/// <returns>Об'єкт задачі пулу потоків</returns>
				ForceInline UnifiedThreadPoolJob PostJob(String&& jobName, Delegate&& action);

				/// <summary>
				/// Запуск безіменої задачі в пулі потоків
				/// </summary>
				/// <param name="action">Делегат задачі</param>
				/// <returns>Об'єкт задачі пулу потоків</returns>
				ForceInline UnifiedThreadPoolJob PostJob(Delegate&& action) {
					return PostJob(String(), MoveRValue(action));
				}

				/// <summary>
				/// Запускає задачі в пулі потоків
				/// </summary>
				/// <param name="jobName">Ім'я задачі</param>
				/// <param name="action">Делегат задачі</param>
				/// <returns>Об'єкт задачі пулу потоків</returns>
				ForceInline UnifiedThreadPoolJob PostJob(const String& jobName, Delegate&& action) {
					return PostJob(String(jobName), MoveRValue(action));
				}
			public:
				/// <summary>
				/// Запуск безіменого таймера
				/// </summary>
				/// <param name="callback">Колбек таймера</param>
				/// <returns>Об'єкт побудови таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder SheduleJob(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);

				/// <summary>
				/// Запуск таймера
				/// </summary>
				/// <param name="taskName">Ім'я таймера</param>
				/// <param name="callback">Колбек</param>
				/// <returns>Об'єкт побудови таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder SheduleJob(String&& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);

				/// <summary>
				/// Запуск таймера
				/// </summary>
				/// <param name="taskName">Ім'я таймера</param>
				/// <param name="callback">Колбек</param>
				/// <returns>Об'єкт побудови таймера</returns>
				UnifiedThreadPoolScheduledTaskBuilder SheduleJob(const String& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);
			public:
				/// <summary>
				/// Вертає кількість зареєстровних задач в пулі
				/// </summary>
				/// <returns>Кількість зареєстровних задач в пулі</returns>
				ForceInline SizeT GetJobsCount() const { return This->JobsQueue.Count; }

				/// <summary>
				/// Вертає кількість зареєстровних таймерів в пулі
				/// </summary>
				/// <returns>Кількість зареєстровних таймерів в пулі</returns>
				ForceInline SizeT GetScheduledTaskCount() const { return This->TaskQueue.Count; }

				/// <summary>
				/// Вертає кількість зареєстрованих об'єктів вводу/виводу в пулі
				/// </summary>
				/// <returns>Кількість зареєстрованих об'єктів вводу/виводу в пулі</returns>
				ForceInline SizeT GetIORequestCount() const { return This->IrpQueue.Count; }
			public:
				/// <summary>
				/// Вертає чергу задач
				/// </summary>
				/// <returns>Черга задач у вигляді списку</returns>
				Containers::List<UnifiedThreadPoolJob> GetJobsQueue() const { return This->JobsQueue.CopyTo<Containers::List>(); }

				/// <summary>
				/// Вертає список таймерів
				/// </summary>
				/// <returns>Список таймерів</returns>
				Containers::List<UnifiedThreadPoolScheduledTask> GetScheduledTaskList() const { return This->TaskQueue.CopyTo<Containers::List>(); }

				/// <summary>
				/// Вертає список об'єктів вводу виводу
				/// </summary>
				/// <returns>Список об'єктів вводу виводу</returns>
				Containers::List<UnifiedThreadPoolIoTask> GetAsyncIOList() const { return This->IrpQueue.CopyTo<Containers::List>(); }
			public:
				/// <summary>
				/// Кількість зареєстровних задач в пулі
				/// </summary>
				SizeT Property(GetJobsCount) JobsCount;

				/// <summary>
				/// Кількість зареєстровних таймерів в пулі
				/// </summary>
				SizeT Property(GetScheduledTaskCount) ScheduledTaskCount;

				/// <summary>
				/// Кількість зареєстрованих об'єктів вводу/виводу в пулі
				/// </summary>
				SizeT Property(GetIORequestCount) IORequestCount;
			public:
				/// <summary>
				/// Вертає ім'я пул задач
				/// </summary>
				/// <returns>Ім'я пулу задач</returns>
				ForceInline const String& GetName() const { return This->threadPoolName; }

				/// <summary>
				/// Встановлює нове ім'я пулу задач
				/// </summary>
				/// <param name="newName">Нове ім'я пулу задач</param>
				ForceInline void SetName(const String& newName) { This->threadPoolName = newName; }

				/// <summary>
				/// Встановлює нове ім'я пулу задач
				/// </summary>
				/// <param name="newName">Нове ім'я пулу задач</param>
				ForceInline void SetName(String&& newName) { This->threadPoolName = MoveRValue(newName); }
			public:
				/// <summary>
				/// Ім'я пуду задач
				/// </summary>
				String Property(GetName,SetName) Name;
			public:
				/// <summary>
				/// Вертає пріоритет зада в пулу потоку
				/// </summary>
				/// <returns>Пріортет задач в пулі потоку</returns>
				ForceInline EPoolThreadsPriority GetJobsPriority() const { return This->handlesData.JobConfiguration.Priority; }

				/// <summary>
				/// Вертає пріоритет таймерів в пулі потоку
				/// </summary>
				/// <returns>Пріоритет таймерів в пулі потоку</returns>
				ForceInline EPoolThreadsPriority GetScheduledTasksPriority() const { return This->handlesData.TaskConfiguration.Priority; }

				/// <summary>
				/// Вертає пріоритет інформуванні про завершення операцій вводу/виводу в пулів потоків 
				/// </summary>
				/// <returns>Пріоритет інформуванні про завершення операцій вводу/виводу в пулів потоків</returns>
				ForceInline EPoolThreadsPriority GetIORequestPriority() const { return This->handlesData.IrpConfiguration.Priority; }
			public:
				/// <summary>
				/// Пріортет задач в пулі потоку
				/// </summary>
				EPoolThreadsPriority Property(GetJobsPriority) JobsPriority;

				/// <summary>
				/// Пріоритет таймерів в пулі потоку
				/// </summary>
				EPoolThreadsPriority Property(GetScheduledTasksPriority) ScheduledTasksPriority;

				/// <summary>
				/// Пріоритет інформуванні про завершення операцій вводу/виводу в пулів потоків 
				/// </summary>
				EPoolThreadsPriority Property(GetIORequestPriority) IORequestPriority;
			public:
				/// <summary>
				/// Вертає максимальну кількість потоків пулу
				/// </summary>
				/// <returns>Максимальна кількість потоків пулу</returns>
				ForceInline UInt64 GetMaxThreadCount() const { return This->MaxThreadCount; }

				/// <summary>
				/// Вертає мінімальну кількість потоків пулу
				/// </summary>
				/// <returns>Мінімальна кількість потоків пулу</returns>
				ForceInline UInt64 GetMinThreadCount() const { return This->MinThreadCount; }
			public:
				/// <summary>
				/// Максимальна кількість потоків пулу
				/// </summary>
				UInt64 Property(GetMaxThreadCount) MaxThreadCount;

				/// <summary>
				/// Мінімальна кількість потоків пулу
				/// </summary>
				UInt64 Property(GetMinThreadCount) MinThreadCount;
			public: 
				RefCountClass(UnifiedThreadPool, This);
			};

			/// <summary>
			/// Пріоритети задач в пулі потоків
			/// </summary>
			using EPoolThreadsPriority = UnifiedThreadPool::EPoolThreadsPriority;

			/// <summary>
			/// Конструктор пулу потоків
			/// </summary>
			class UnifiedThreadPoolBuilder final {
			private:
				struct Data {
					String threadPoolName;
					UInt32 minThreadCount;
					UInt32 maxThreadCount;
					UnifiedThreadPool::EPoolThreadsPriority jobsPriority;
					UnifiedThreadPool::EPoolThreadsPriority sheduledTasksPriority;
					UnifiedThreadPool::EPoolThreadsPriority irpRequestPriority;

					Data(
						String&& threadPoolName,
						UInt32 minThreadCount,
						UInt32 maxThreadCount,
						UnifiedThreadPool::EPoolThreadsPriority jobsPriority,
						UnifiedThreadPool::EPoolThreadsPriority sheduledTasksPriority,
						UnifiedThreadPool::EPoolThreadsPriority irpRequestPriority
					) :
						threadPoolName(MoveRValue(threadPoolName)),
						minThreadCount(minThreadCount),
						maxThreadCount(maxThreadCount),
						jobsPriority(jobsPriority),
						sheduledTasksPriority(sheduledTasksPriority),
						irpRequestPriority(irpRequestPriority)
					{}
				};
			private:
				Data* This;
			public:
				/// <summary>
				/// Пустий конструктор пулу задач, 
				/// ініціалізовує налаштування безіменого пулу потоків,
				/// з мінімальною кількістю потоків яка рівна клькості половинів ядер процесора хоста,
				/// з максимальною кількістю потоків яка рівна клькості ядер процесора хоста,
				/// і стандартним пріоритетом всіх типів задач
				/// </summary>
				ForceInline UnifiedThreadPoolBuilder() :This(new Data(
					String(),
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount / 2,
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium)) {}

				/// <summary>
				/// Пустий конструктор пулу задач, 
				/// ініціалізовує налаштування іменованого пулу потоків,
				/// з мінімальною кількістю потоків яка рівна клькості половинів ядер процесора хоста,
				/// з максимальною кількістю потоків яка рівна клькості ядер процесора хоста,
				/// і стандартним пріоритетом всіх типів задач
				/// </summary>
				/// <param name="threadPoolName">Ім'я пулу задач</param>
				ForceInline UnifiedThreadPoolBuilder(String&& threadPoolName) :This(new Data(
					MoveRValue(threadPoolName),
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount / 2,
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium)) {}

				/// <summary>
				/// Пустий конструктор пулу задач, 
				/// ініціалізовує налаштування іменованого пулу потоків,
				/// з мінімальною кількістю потоків яка рівна клькості половинів ядер процесора хоста,
				/// з максимальною кількістю потоків яка рівна клькості ядер процесора хоста,
				/// і стандартним пріоритетом всіх типів задач
				/// </summary>
				/// <param name="threadPoolName">Ім'я пулу задач</param>
				ForceInline UnifiedThreadPoolBuilder(const String& threadPoolName) : UnifiedThreadPoolBuilder(String(threadPoolName)) {}
			public:
				/// <summary>
				/// Встновлює пріоритет всіх типів задач
				/// </summary>
				/// <param name="newThreadPoolPriority">Пріоритет, який буде встановлено</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetThreadPoolPriority(UnifiedThreadPool::EPoolThreadsPriority newThreadPoolPriority) && {
					This->irpRequestPriority = newThreadPoolPriority;
					This->jobsPriority = newThreadPoolPriority;
					This->sheduledTasksPriority = newThreadPoolPriority;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює пріортет обробки інформування I/O операцій
				/// </summary>
				/// <param name="newIoThreadsPriority">Пріоритет, який буде встановлено</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetIOThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newIoThreadsPriority) && {
					This->irpRequestPriority = newIoThreadsPriority;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює пріоритет задач таймера
				/// </summary>
				/// <param name="newIoThreadsPriority">Пріоритет, який буде встановлено</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetShedulerThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newShedulerThreadsPriority) && {
					This->sheduledTasksPriority = newShedulerThreadsPriority;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Пріоритет задач пулу потоків
				/// </summary>
				/// <param name="newIoThreadsPriority">Пріоритет, який буде встановлено</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetJobThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newJobThreadsPriority) && {
					This->jobsPriority = newJobThreadsPriority;
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Встановлює мінімальну кількість потоків пулу, може бути 0
				/// </summary>
				/// <param name="newMinThreadCount">Мінімальна кількість потоків пулу</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetMinThreadCount(UInt32 newMinThreadCount) && {
					This->minThreadCount = newMinThreadCount;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює максимальну кількість потоків пулу потоків
				/// </summary>
				/// <param name="newMaxThreadCount">Макстмальна кількість потоків пуду потоків</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetMaxThreadCount(UInt32 newMaxThreadCount) && {
					This->maxThreadCount = newMaxThreadCount;
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Встановлює ім'я пулу потоків
				/// </summary>
				/// <param name="newName">Нове ім'я пулу</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetName(String&& newName) && {
					This->threadPoolName = MoveRValue(newName);
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює ім'я пулу потоків
				/// </summary>
				/// <param name="newName">Нове ім'я пулу</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder&& SetName(const String& newName) && {
					This->threadPoolName = newName;
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Створює пул потоків з задною конфігурацією
				/// </summary>
				/// <returns></returns>
				UnifiedThreadPool CreateThreadPool() &&;
			private:
				UnifiedThreadPool::PoolConfigureation CreateJobConfiguration(ImplSpecificT poolHandle);
				UnifiedThreadPool::PoolConfigureation CreateScheduledTaskConfiguration(ImplSpecificT poolHandle);
				UnifiedThreadPool::PoolConfigureation CreateIrpConfiguration(ImplSpecificT poolHandle);
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline ~UnifiedThreadPoolBuilder()
				{
					if (This != nullptr) {
						delete This;
						This = nullptr;
					}
				}
			public:
				UnifiedThreadPoolBuilder(const UnifiedThreadPoolBuilder&) = delete;
				UnifiedThreadPoolBuilder& operator=(const UnifiedThreadPoolBuilder&) = delete;
			public:
				/// <summary>
				/// Конструктор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline UnifiedThreadPoolBuilder(UnifiedThreadPoolBuilder&& orig) : This(orig.This) { orig.This = nullptr; }

				/// <summary>
				/// Оператор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolBuilder& operator=(UnifiedThreadPoolBuilder&& orig) {
					this->~UnifiedThreadPoolBuilder();
					this->UnifiedThreadPoolBuilder::UnifiedThreadPoolBuilder(MoveRValue(orig));
					return *this;
				}
			};

			/// <summary>
			/// Конструктор таймера пулу потоків
			/// </summary>
			class UnifiedThreadPoolScheduledTaskBuilder final {
			private:
				friend class UnifiedThreadPool;
			private:
				struct Data
				{
					String taskName;
					UInt16 intervalsCount;
					Time::TimeSpan proximity;
					Time::TimeSpan intervalLenght;
					UInt64 startTime;
					Boolean isRelaive;
					SmartPtr<IUnifiedThreadPoolShedulerCallback> callback;
					UnifiedThreadPool ownerPool;

					Data(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback) :
						taskName(),
						proximity(100ms),
						intervalsCount(0),
						intervalLenght(0),
						startTime(0),
						isRelaive(true),
						callback(MoveRValue(callback)),
						ownerPool(ownerPool)
					{}

					Data(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& taskName) :
						taskName(MoveRValue(taskName)),
						proximity(100ms),
						intervalsCount(0),
						intervalLenght(0),
						startTime(0),
						isRelaive(true),
						callback(MoveRValue(callback)),
						ownerPool(ownerPool)
					{}

					Data(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, const String& taskName) :
						taskName(taskName),
						proximity(100ms),
						intervalsCount(0),
						intervalLenght(0),
						startTime(0),
						isRelaive(true),
						callback(MoveRValue(callback)),
						ownerPool(ownerPool)
					{}
				};
			private:
				Data* This;
			private:
				/// <summary>
				/// Створює конфігурацію безіменого таймера
				/// з точністю 100ms, одноразовим запуском зразу після створення
				/// та з задним колбеком
				/// </summary>
				/// <param name="ownerPool">Пул потоків</param>
				/// <param name="callback">Колбек</param>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
					: This(new Data(ownerPool, MoveRValue(callback))) {}

				/// <summary>
				/// Створює конфігурацію іменованого таймера
				/// з точністю 100ms, одноразовим запуском зразу після створення
				/// та з задним колбеком
				/// </summary>
				/// <param name="ownerPool">Пул потоків</param>
				/// <param name="callback">Колбек</param>
				/// <param name="taskName">Ім'я задачі</param>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& taskName)
					: This(new Data(ownerPool, MoveRValue(callback), MoveRValue(taskName))) {}

				/// <summary>
				/// Створює конфігурацію іменованого таймера
				/// з точністю 100ms, одноразовим запуском зразу після створення
				/// та з задним колбеком
				/// </summary>
				/// <param name="ownerPool">Пул потоків</param>
				/// <param name="callback">Колбек</param>
				/// <param name="taskName">Ім'я задачі</param>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, const String& taskName)
					: This(new Data(ownerPool, MoveRValue(callback), taskName)) {}
			public:
				/// <summary>
				/// Встановлює новий колбек
				/// </summary>
				/// <param name="callback">Новий колбек</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetCallback(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)&& {
					This->callback = MoveRValue(callback);
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Встановлю ім'я таймера
				/// </summary>
				/// <param name="name">Нове ім'я таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetName(String&& name)&& {
					This->taskName = MoveRValue(name);
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлю ім'я таймера
				/// </summary>
				/// <param name="name">Нове ім'я таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetName(const String& name)&& {
					This->taskName = name;
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Встановлює кількість запусків таймера
				/// </summary>
				/// <param name="count">Кількість запусків таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetExecutionCount(UInt16 count)&& {
					This->intervalsCount = count;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює довжину інтервалу між запусками таймера
				/// </summary>
				/// <param name="intervalLenght">Довжина між інтервалами</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetIntervalLenght(Time::TimeSpan intervalLenght)&& {
					if(This->intervalLenght > MaxIntervalLenght)
						Throw(InvalidArgumentException(ExceptionPosition, L"IntervalLenght", L"Too big interval lenght"));
					This->intervalLenght = intervalLenght;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює абсолютний час запуск таймера
				/// </summary>
				/// <param name="absoluteStartTime">Абсолютний час запуску таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetStartTime(Time::DateTime absoluteStartTime)&& {
					This->startTime = absoluteStartTime.TimeFromStartEpoch;
					This->isRelaive = false;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює відносний час запуску таймера
				/// </summary>
				/// <param name="relativeStartTime">Віднисний час запуску таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetStartTime(Time::TimeSpan relativeStartTime)&& {
					This->startTime = relativeStartTime.Ticks;
					This->isRelaive = true;
					return MoveRValue(*this);
				}

				/// <summary>
				/// Встановлює точність таймера
				/// </summary>
				/// <param name="proximity">Точність таймера</param>
				/// <returns>RValue посилання на поточний об'єкт</returns>
				ForceInline UnifiedThreadPoolScheduledTaskBuilder&& SetProximity(Time::TimeSpan proximity)&& {
					This->proximity = proximity;
					return MoveRValue(*this);
				}
			public:
				/// <summary>
				/// Створює таймер з заданими конфігураціями
				/// </summary>
				/// <returns></returns>
				UnifiedThreadPoolScheduledTask Shedule()&&;
			public:
				/// <summary>
				/// Максимальний допустимий інтервал між запусками таймера
				/// </summary>
				static constexpr auto MaxIntervalLenght = UnifiedThreadPoolScheduledTask::MaxIntervalLenght;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				~UnifiedThreadPoolScheduledTaskBuilder()
				{
					if (This != nullptr) {
						delete This;
					}
				}
			};

			class ThreadableUnifiedThreadPoolJob;

			ForceInline UnifiedThreadPoolScheduledTaskBuilder UnifiedThreadPool::SheduleJob(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
			{
				return UnifiedThreadPoolScheduledTaskBuilder(*this, MoveRValue(callback));
			}

			ForceInline UnifiedThreadPoolScheduledTaskBuilder UnifiedThreadPool::SheduleJob(String&& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
			{
				return UnifiedThreadPoolScheduledTaskBuilder(*this, MoveRValue(callback), MoveRValue(taskName));
			}

			ForceInline UnifiedThreadPoolScheduledTaskBuilder UnifiedThreadPool::SheduleJob(const String& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
			{
				return UnifiedThreadPoolScheduledTaskBuilder(*this, MoveRValue(callback), String(taskName));
			}
		}

		namespace IO {
			class IRPBase;
			class IOTask;

			/// <summary>
			/// Базовий інтерфес асинхроного об'єкта I/O
			/// </summary>
			class IAsyncIOBase : virtual protected IDisposable {
			protected:
				friend class IRPBase;
			protected:
				Seed::Core::Threading::UnifiedThreadPoolIoTask RegisterNtIoObjectInThreadPool(const Threading::UnifiedThreadPool& pool, Handle hFileObj);

				void UnRegisterNtIoObjectInThreadPool(Seed::Core::Threading::UnifiedThreadPoolIoTask task);

				void* GetReservedPlaceInIrp(IRPBase* irp);

				IOTask GetIOTaskFromIRPBase(IRPBase* irp);

				void NotifyIoOperation(const Threading::UnifiedThreadPoolIoTask& ptpIo);

				void CancelNotifyIoOperation(const Threading::UnifiedThreadPoolIoTask& ptpIo);
			protected:
				virtual Handle GetHandle() = 0;
			protected:
				virtual const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetIoPoolTask() const = 0;
			public:
				/// <summary>
				/// Перевірка чи об'єкт коректний
				/// </summary>
				/// <returns>true якщо об'єкт допускає I/O операції</returns>
				virtual bool GetIsValid() const = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				virtual ~IAsyncIOBase() = 0 {}
			};

			/// <summary>
			/// Інтерфес асинхроного об'єкта I/O
			/// </summary>
			/// <typeparam name="TDataType">Тип об'єкта пакета(блоку) даних</typeparam>
			template<typename TDataType> requires TypeTraits::Collection::IsArray<TDataType>
			class IBaseAsyncIO : virtual public IAsyncIOBase {
			public:
				/// <summary>
				/// Ініціалізація асинхроної операції читання
				/// </summary>
				/// <param name="data">Буфер даних</param>
				/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
				/// <returns>Об'єкт задачі I/O</returns>
				virtual IOTask AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) = 0;

				/// <summary>
				/// Ініціалізація асинхроної операції запису
				/// </summary>
				/// <param name="data">Буфер даних</param>
				/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
				/// <returns>Об'єкт задачі I/O</returns>
				virtual IOTask AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) = 0;
			public:
				/// <summary>
				/// Перевірка чи об'єкт коректний
				/// </summary>
				/// <returns>true якщо об'єкт допускає I/O операції</returns>
				virtual bool GetIsValid() const  = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				virtual ~IBaseAsyncIO() = 0 {};
			};

			template<typename TAsyncObject> requires TypeTraits::IsBasedOnV<IAsyncIOBase, TAsyncObject>
			class IBaseConnectableAsyncIO : virtual public IAsyncIOBase {
				virtual IOTask AsyncConnect(SmartPtr<IFinishConnectCallback<TAsyncObject>> onFinishConnectAction) = 0;
			public:
				virtual bool GetIsValid() const = 0;
			public:
				virtual ~IBaseConnectableAsyncIO() = 0;
			};

			/// <summary>
			/// Інтерфес об'єта асинхроного I/O з позиційним читанням
			/// </summary>
			/// <typeparam name="TDataType">Тип буфера даних</typeparam>
			template<typename TDataType> requires TypeTraits::Collection::IsArray<TDataType>
			class IBasePositionAsyncIO : virtual public IAsyncIOBase {
			public:
				/// <summary>
				/// Ініціаліщація операції асихроного читання
				/// </summary>
				/// <param name="data">Буфер даних</param>
				/// <param name="position">Позиція в клькіост байтів</param>
				/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
				/// <returns>Об'єкт задачі I/O</returns>
				virtual IOTask AsyncRead(TDataType&& data, SizeT position, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) = 0;

				/// <summary>
				/// Ініціаліщація операції асихроного запису
				/// </summary>
				/// <param name="data">Буфер даних</param>
				/// <param name="position">Позиція в клькіост байтів</param>
				/// <param name="onFinishReadAction">Колбек який викличеться по результату операції</param>
				/// <returns>Об'єкт задачі I/O</returns>
				virtual IOTask AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IFinishWriteCallback<TDataType>> onFinishReadAction) = 0;
			public:
				/// <summary>
				/// Перевірка чи об'єкт коректний
				/// </summary>
				/// <returns>true якщо об'єкт допускає I/O операції</returns>
				virtual bool GetIsValid() const = 0;
			public:
				virtual ~IBasePositionAsyncIO() = 0 {};
			};

			/// <summary>
			/// Клас базового пакета I/O операції
			/// </summary>
			class IRPBase : protected IClonable<IRPBase*> {
			public:
				friend class IOTask;
				friend class IAsyncIOBase;
			public:
				/// <summary>
				/// Тип колбека
				/// </summary>
				enum class ECallbackType {
					/// <summary>
					/// Читання
					/// </summary>
					Read, 
					/// <summary>
					/// Запис
					/// </summary>
					Write,
					/// <summary>
					/// З'єднання
					/// </summary>
					Connect
				};
			private:
				Byte Reserverd[16 + sizeof(SizeT)];

				Threading::SynchronizeEvent event;

				ECallbackType irpCallbackType;

				SmartPtr<IAsyncIOBase> irpTarget;

				volatile bool isCompleted;
			private:
				friend const Threading::SynchronizeEvent& GetIrpCompleteEvent(IRPBase* irp);

				friend void MarkIrpCompleted(IRPBase* irp);
			public:
				/// <summary>
				/// Конструктор з об'єкту асинхроного I/O ти типу колбека
				/// </summary>
				/// <param name="irpTarget">Об'єкт асинхроного I/O</param>
				/// <param name="irpCallbackType">Тип колбеку</param>
				ForceInline IRPBase(SmartPtr<IAsyncIOBase>&& irpTarget, ECallbackType irpCallbackType)
					:  irpCallbackType(irpCallbackType), irpTarget(irpTarget), event(false), isCompleted(false) {}

				/// <summary>
				/// Стандарний конструктор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				IRPBase(const IRPBase& orig) = default;

				/// <summary>
				/// Стандартнтй конструкор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				IRPBase(IRPBase&& orig) = default;
			public:
				/// <summary>
				/// Викликає колбек
				/// </summary>
				/// <param name="byteTransfered">Вказує кількість переданих байтів даних</param>
				/// <param name="isSuccess">Успішність операції</param>
				/// <param name="errorCode">Код помилки</param>
				virtual void OnIORequestComplete(SizeT byteTransfered, Boolean isSuccess, UInt32 errorCode) = 0;
			public:
				/// <summary>
				/// Вертає тип колбеку
				/// </summary>
				/// <returns>Тип колбеку</returns>
				ForceInline ECallbackType GetIrpCallbackType() const { return irpCallbackType; }

				/// <summary>
				/// Вертає об'єкт асинхроного I/O
				/// </summary>
				/// <returns>Об'єкт асинхроного I/O</returns>
				ForceInline const SmartPtr<IAsyncIOBase>& GetIrpTarget() const { return irpTarget; }
			public:
				/// <summary>
				/// Тип колбеку
				/// </summary>
				ECallbackType Property(GetIrpCallbackType) CallbackType;

				/// <summary>
				/// Об'єкт асинхроного I/O
				/// </summary>
				SmartPtr<IAsyncIOBase> Property(GetIrpTarget) Target;
			public:
				/// <summary>
				/// Вертає задачу пулу потоків, в контексті якої виконується I/O операція
				/// </summary>
				/// <returns>Задача пулу потоків, в контексті якої виконується I/O операція</returns>
				ForceInline const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetTargetTask() const { return irpTarget->GetIoPoolTask(); }
			public:
				/// <summary>
				/// Задача пулу потоків, в контексті якої виконується I/O операція
				/// </summary>
				Seed::Core::Threading::UnifiedThreadPoolIoTask Property(GetTargetTask) TargetTask;
			public:
				/// <summary>
				/// Очікування на заверщення операції
				/// </summary>
				ForceInline void WaitOnCompleteIrp() const { event.Wait(); }

				/// <summary>
				/// Очікування на завершення операції з таймаутом
				/// </summary>
				/// <param name="timeout">Таймаут</param>
				ForceInline void WaitOnCompleteIrp(Time::TimeSpan timeout) const { event.Wait(timeout); }

				/// <summary>
				/// Скасування операції
				/// </summary>
				void CancelIo();
			public:
				/// <summary>
				/// Вертає чи операція вже завершена
				/// </summary>
				/// <returns>true, якщо операція вже завершена</returns>
				ForceInline Boolean GetIsCompleted() const { return isCompleted; }
			public:
				/// <summary>
				/// true, якщо операція вже завершена
				/// </summary>
				Boolean Property(GetIsCompleted) IsCompleted;
			protected:
				IRPBase* Clone() const = 0;
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~IRPBase() = 0 {}
			};

			/// <summary>
			/// Тип колбеку
			/// </summary>
			using ECallbackType = IRPBase::ECallbackType;

			/// <summary>
			/// Пакет асинхроного читання даних
			/// </summary>
			/// <typeparam name="TDataType">Тип блоку даних</typeparam>
			template<typename TDataType>
			class ReadIRP final : public IRPBase {
			public:
				friend class IOTask;
			private:
				SmartPtr<IFinishReadCallback<TDataType>> finishReadCallback;
				TDataType buffer;
			public:
				/// <summary>
				/// Конструктор з об'єкту асинхроного I/O, буфера та колбеку
				/// </summary>
				/// <param name="irpTarget">Об'єкт асинхроного I/O</param>
				/// <param name="finishWriteCallback">Колбек завершення операції</param>
				/// <param name="buffer">Буфер даних</param>
				ForceInline ReadIRP(SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IFinishReadCallback<TDataType>> finishWriteCallback, TDataType&& buffer) :
					IRPBase(MoveRValue(irpTarget), ECallbackType::Read), finishReadCallback(finishWriteCallback), buffer(MoveRValue(buffer)) {}
			public:
				/// <summary>
				/// Стандарний конструктор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline ReadIRP(const ReadIRP& orig) = default;

				/// <summary>
				/// Стандартнтй конструкор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline ReadIRP(ReadIRP&& orig) = default;

				/// <summary>
				/// Стандартний оператор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline ReadIRP& operator=(const ReadIRP& orig) = default;

				/// <summary>
				/// Стандартний оператор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline ReadIRP& operator=(ReadIRP&&) = default;
			public:
				/// <summary>
				/// Виклик колбеку операції
				/// </summary>
				/// <param name="byteTransfered">Кількість прочитаних байтів</param>
				/// <param name="isSuccess">Успішність операції</param>
				/// <param name="errorCode">Код помилки</param>
				ForceInline virtual void OnIORequestComplete(SizeT byteTransfered, Boolean isSuccess, UInt32 errorCode) override {
					if (isSuccess) {
						finishReadCallback->OnFinishRead(byteTransfered, MoveRValue(buffer));
					}
					else {
						finishReadCallback->OnError(errorCode);
					}
				}
			protected:
				ForceInline virtual IRPBase* Clone() const { return new ReadIRP(*this); }
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~ReadIRP() override {}
			};

			/// <summary>
			/// Пакет асинхроного запису даних
			/// </summary>
			/// <typeparam name="TDataType">Тип блоку даних</typeparam>
			template<typename TDataType>
			class WriteIRP final : public IRPBase {
			public:
				friend class IOTask;
			private:
				SmartPtr<IFinishWriteCallback<TDataType>> finishWriteCallback;
				TDataType buffer;
			public:
				/// <summary>
				/// Конструктор з об'єкту асинхроного I/O, буфера та колбеку
				/// </summary>
				/// <param name="irpTarget">Об'єкт асинхроного I/O</param>
				/// <param name="finishWriteCallback">Колбек завершення операції</param>
				/// <param name="buffer">Буфер даних</param>
				ForceInline WriteIRP(SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IFinishWriteCallback<TDataType>> finishWriteCallback, const TDataType&& buffer) :
					IRPBase(MoveRValue(irpTarget), ECallbackType::Write), finishWriteCallback(finishWriteCallback), buffer(MoveRValue(buffer)) {}

				/// <summary>
				/// Стандарний конструктор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline WriteIRP(const WriteIRP& orig) = default;

				/// <summary>
				/// Стандартнтй конструкор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline WriteIRP(WriteIRP&&) = default;

				/// <summary>
				/// Стандартний оператор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline WriteIRP& operator=(const WriteIRP&) = default;

				/// <summary>
				/// Стандартний оператор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline WriteIRP& operator=(WriteIRP&&) = default;
			public:
				/// <summary>
				/// Виклик колбеку операції
				/// </summary>
				/// <param name="byteTransfered">Кількість прочитаних байтів</param>
				/// <param name="isSuccess">Успішність операції</param>
				/// <param name="errorCode">Код помилки</param>
				ForceInline virtual void OnIORequestComplete(SizeT byteTransfered, Boolean isSuccess, UInt32 errorCode) override {
					if (isSuccess) {
						finishWriteCallback->OnWriteFinish(byteTransfered, MoveRValue(buffer));
					}
					else {
						finishWriteCallback->OnError(errorCode);
					}
				}
			protected:
				ForceInline virtual IRPBase* Clone() const { return new WriteIRP(*this); }
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~WriteIRP() override {}
			};

			/// <summary>
			/// Пакет асинхроного запису даних
			/// </summary>
			/// <typeparam name="TDataType">Тип блоку даних</typeparam>
			template<typename TAsyncObject>
			class ConnectIRP final : public IRPBase {
			public:
				friend class IOTask;
			private:
				SmartPtr<IFinishConnectCallback<TAsyncObject>> finishConnectCallback;
				TAsyncObject connectionObject;
			public:
				/// <summary>
				/// Конструктор з об'єкту асинхроного I/O, буфера та колбеку
				/// </summary>
				/// <param name="irpTarget">Об'єкт асинхроного I/O</param>
				/// <param name="finishWriteCallback">Колбек завершення операції</param>
				/// <param name="buffer">Буфер даних</param>
				ForceInline ConnectIRP(SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IFinishConnectCallback<TAsyncObject>> finishConnectCallback, TAsyncObject&& connectionObject) :
					IRPBase(MoveRValue(irpTarget), ECallbackType::Connect), finishConnectCallback(finishConnectCallback), connectionObject(MoveRValue(connectionObject)) {}

				/// <summary>
				/// Стандарний конструктор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline ConnectIRP(const ConnectIRP& orig) = default;

				/// <summary>
				/// Стандартнтй конструкор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				ForceInline ConnectIRP(ConnectIRP&&) = default;

				/// <summary>
				/// Стандартний оператор копіювання
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline ConnectIRP& operator=(const ConnectIRP&) = default;

				/// <summary>
				/// Стандартний оператор переміщення
				/// </summary>
				/// <param name="orig">Оригинал</param>
				/// <returns>Посилання на поточний об'єкт</returns>
				ForceInline ConnectIRP& operator=(ConnectIRP&&) = default;
			public:
				/// <summary>
				/// Виклик колбеку операції
				/// </summary>
				/// <param name="byteTransfered">Кількість прочитаних байтів</param>
				/// <param name="isSuccess">Успішність операції</param>
				/// <param name="errorCode">Код помилки</param>
				ForceInline virtual void OnIORequestComplete(UnusedData SizeT byteTransfered, Boolean isSuccess, UInt32 errorCode) override {
					if (isSuccess) {
						finishConnectCallback->OnConnectFinish(MoveRValue(connectionObject));
					}
					else {
						finishConnectCallback->OnError(errorCode);
					}
				}
			protected:
				ForceInline virtual IRPBase* Clone() const { return new WriteIRP(*this); }
			public:
				/// <summary>
				/// Деструктор
				/// </summary>
				ForceInline virtual ~ConnectIRP() override {}
			};

			/// <summary>
			/// Об'єкт I/O операції
			/// </summary>
			class IOTask : public IDisposable {
			public:
				friend class IAsyncIOBase;
			private:
				struct Data {
					IRPBase* irp;

					RefCountT RefCount;

					Data(IRPBase* irp) :
						irp(irp->Clone()), RefCount(1) {}
				};
			private:
				Data* This;
			protected:
				IOTask(IRPBase* irp) :
					This(new Data(irp)) {}
			protected:
				virtual void Dispose() {
					delete This->irp;
				}
			public:
				/// <summary>
				/// Вертає тип колбеку
				/// </summary>
				/// <returns>Тип колбеку</returns>
				ForceInline ECallbackType GetIrpCallbackType() const { return This->irp->CallbackType; }

				/// <summary>
				/// Вертає об'єкт асинхроного I/O
				/// </summary>
				/// <returns>Об'єкт асинхроного I/O</returns>
				ForceInline const SmartPtr<IAsyncIOBase>& GetIrpTarget() const { return This->irp->Target; }
			public:
				/// <summary>
				/// Тип колбеку
				/// </summary>
				ECallbackType Property(GetIrpCallbackType) CallbackType;

				/// <summary>
				/// Об'єкт асинхроного I/O
				/// </summary>
				SmartPtr<IAsyncIOBase> Property(GetIrpTarget) Target;
			public:
				/// <summary>
				/// Вертає задачу пулу потоків, в контексті якої виконується I/O операція
				/// </summary>
				/// <returns>Задача пулу потоків, в контексті якої виконується I/O операція</returns>
				ForceInline const Seed::Core::Threading::UnifiedThreadPoolIoTask& GetTargetTask() const { return This->irp->TargetTask; }
			public:
				/// <summary>
				/// Задача пулу потоків, в контексті якої виконується I/O операція
				/// </summary>
				Seed::Core::Threading::UnifiedThreadPoolIoTask Property(GetTargetTask) TargetTask;
			public:
				/// <summary>
				/// Очікування на заверщення операції
				/// </summary>
				ForceInline void WaitComplete() const { This->irp->WaitOnCompleteIrp(); }

				/// <summary>
				/// Очікування на завершення операції з таймаутом
				/// </summary>
				/// <param name="timeout">Таймаут</param>
				ForceInline void WaitComplete(Time::TimeSpan timeout) const { This->irp->WaitOnCompleteIrp(timeout); }

				/// <summary>
				/// Скасування операції
				/// </summary>
				ForceInline void CancelIo() { This->irp->CancelIo(); }
			public:
				/// <summary>
				/// Вертає чи операція вже завершена
				/// </summary>
				/// <returns>Чи операція вже завершена</returns>
				ForceInline Boolean GetIsCompleted() const { return This->irp->IsCompleted; }
			public:
				/// <summary>
				/// Чи операція вже завершенаo
				/// </summary>
				Boolean Property(GetIsCompleted) IsCompleted;
			public:
				RefCountClass(IOTask, This);
			};
		}
	}
}