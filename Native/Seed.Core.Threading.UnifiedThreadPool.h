#pragma once
#include "Seed.Core.IO.Base.h"

#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.Threading.LockPrimitives.h"
#include "Seed.Core.Containers.Delegate.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			class UnifiedThreadPool;

			class IThreadPoolObject;

			class UnifiedThreadPoolJob;
			class UnifiedThreadPoolSheduledTask;
			class UnifiedThreadPoolSheduledTaskBuilder;
			class UnifiedThreadPoolIoTask;
		}

		namespace IO {
			class IRPBase;
			class IAsyncIOBase;
		}

		namespace IO {
			class IExceptionCallback : protected IDisposable {
			public:
				virtual void OnError(UInt32 errorCode) = 0;
			public:
				virtual ~IExceptionCallback() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IFinishReadCallback : public IExceptionCallback {
			public:
				virtual void OnReadFinish(TDataType&& data) volatile = 0;
			private:
				virtual ~IFinishReadCallback() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IFinishWriteCallback : public IExceptionCallback {
			public:
				virtual void OnWriteFinish(const TDataType&& data) volatile = 0;
			private:
				virtual ~IFinishWriteCallback() = 0 {}
			};
		}

		namespace Threading {
			
			class IThreadPoolObject;

			class UnifiedThreadPoolJob;
			class UnifiedThreadPoolSheduledTask;
			class UnifiedThreadPoolSheduledTaskBuilder;
			class UnifiedThreadPoolIoTask;

			class UnifiedThreadPoolBuilder;

			class IUnifiedThreadPoolShedulerCallback : protected IDisposable {
			public:
				virtual void Shedule() = 0;
			public:
				virtual ~IUnifiedThreadPoolShedulerCallback() = 0 {}
			};

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
					ImplSpecificT hInstance;
					ImplSpecificT hEvent;
					ID threadId;
					Handle hThread;
					EThreadabledStates threadableState;
					EThreadabledStates secondThreadableState;
					Containers::ConcurencyList<UnifiedThreadPoolJob>::Iterator poolQueueRef;
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator threadbleRef;

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
						hInstance(nullptr),
						hEvent(hEvent),
						threadId(0),
						hThread(nullptr),
						threadableState(EThreadabledStates::NotStarted),
						secondThreadableState(EThreadabledStates::Paused),
						poolQueueRef(),
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override;
			private:
				friend void SetUnifiedThreadPoolJobInstanceHandleObj(UnifiedThreadPoolJob* jobObj, ImplSpecificT hInstance);
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolJob* jobObj);
				friend void SetEventOnJobFinish(UnifiedThreadPoolJob* jobObj);
				friend void RegisterThread(UnifiedThreadPoolJob* thread);
				friend void UnRegisterThread(UnifiedThreadPoolJob* thread);
				friend Delegate& GetJobEntryPoint(UnifiedThreadPoolJob* jobObj);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj);
				friend void UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj);
			private:
				UnifiedThreadPoolJob(Data* This) : This(This) {}
			public:
				const String& GetName() {
					return This->jobName;
				}
				void SetName(const String& newName) {
					SetName(String(newName));
				}
				void SetName(String&& newName);
			public:
				Boolean GetIsAlredyExecuted() const {
					return This->action.GetIsEnded();
				}
				Boolean GetIsStartedExecution() const {
					return This->action.GetIsStarted();
				}
				UnifiedThreadPool GetOwnerPool() const;
			public:
				String Property(GetName, SetName) Name;
				Boolean Property(GetIsAlredyExecuted) IsAlredyExecuted;
				Boolean Property(GetIsStartedExecution) IsStartedExecution;
			private:
				UnifiedThreadPoolJob& Suspend();
				UnifiedThreadPoolJob& Resume();
				UnifiedThreadPoolJob& Interupt();
			public:
				UnifiedThreadPoolJob& Join();
				UnifiedThreadPoolJob& Join(Time::DateTime timeOut);
				const UnifiedThreadPoolJob& Join() const;
				const UnifiedThreadPoolJob& Join(Time::DateTime timeOut) const;
			public:
				RefCountClass(UnifiedThreadPoolJob, This);
			};

			class UnifiedThreadPoolSheduledTask final : protected IDisposable {
			private:
				friend class UnifiedThreadPoolSheduledTaskBuilder;
				friend class ThreadableUnifiedThreadPoolSheduledTask;
			private:
				struct Data {
					String taskName;
					SmartPtr<IUnifiedThreadPoolShedulerCallback> callback;
					
					ImplSpecificT ownerPool;
					Containers::ConcurencyList<UnifiedThreadPoolSheduledTask>::Iterator poolQueueRef;
					
					
					Time::DateTime startTime;
					Time::TimeSpan intervalLenght;
					Time::DateTime lastStartTime;

					RefCountT intervalsCount;
					RefCountT executedCount;
					RefCountT remainingCount;
					Int32 timerProximity;

					Handle hTask;
					SRWLock taskLock;

					ID activeTid;
					Handle hActiveThread;
					Handle hEvent;
					EThreadabledStates activeState;
					EThreadabledStates secondState;
					ImplSpecificT callbackInstance;
					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator threadbleRef;

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
						hTask(nullptr),
						taskLock(),
						activeTid(0),
						hActiveThread(nullptr),
						hEvent(nullptr),
						activeState(EThreadabledStates::NotStarted),
						secondState(EThreadabledStates::NotStarted),
						callbackInstance(nullptr),
						threadbleRef(),
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override;
			private:
				UnifiedThreadPoolSheduledTask(Data* This) : This(This) {}
			private:
				friend SRWLock& GetTaskLock(UnifiedThreadPoolSheduledTask* task);
				friend void SetCallbackInstanceObj(UnifiedThreadPoolSheduledTask* task, ImplSpecificT callbackInstance);
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolSheduledTask* task);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolSheduledTask* task);
				friend SmartPtr<IUnifiedThreadPoolShedulerCallback>& GetJobEntryPoint(UnifiedThreadPoolSheduledTask* task);
				friend void PulseEvent(UnifiedThreadPoolSheduledTask* task);
				friend void ZeroingTaskHadle(UnifiedThreadPoolSheduledTask* thread);
			private:
				friend void RegisterThread(UnifiedThreadPoolSheduledTask* thread);
				friend void UnRegisterThread(UnifiedThreadPoolSheduledTask* thread);
				friend void UnRegisterTaskFromPoolQueue(UnifiedThreadPoolSheduledTask* thread);
			public:
				const String& GetTaskName() const { return This->taskName; }
				void SetName(String&& newTaskName);
				void SetName(const String& newTaskName) { SetName(String(newTaskName)); }
			public:
				SizeT GetExecutedCount() const { return This->executedCount; }
				Time::DateTime GetLastStartTime() const { return This->lastStartTime; }
				Boolean GetIsActive() const { return This->hActiveThread != nullptr; }
				UnifiedThreadPool GetOwnerPool() const;
				Time::TimeSpan GetTimerProximity() const { return Time::TimeSpan(This->timerProximity); }
				Time::DateTime GetFirstStartTime() const { return This->startTime; }
				SmartPtr<IUnifiedThreadPoolShedulerCallback>& GetCallbackObject() const { return This->callback; }
			public:
				SizeT Property(GetExecutedCount) ExecutedCount;
				Time::DateTime Property(GetLastStartTime) LastStartTime;
				Boolean Property(GetIsActive) IsActive;
				UnifiedThreadPool Property(OwnerPool) OwnerPool;
				Time::TimeSpan Property(GetTimerProximity) TimerProximity;
				Time::DateTime Property(GetFirstStartTime)FirstStartTime;
				SmartPtr<IUnifiedThreadPoolShedulerCallback> Property(GetCallbackObject) CallbackObject;
			private:
				UnifiedThreadPoolSheduledTask& Suspend();
				UnifiedThreadPoolSheduledTask& Resume();
				UnifiedThreadPoolSheduledTask& Interupt();
			public:
				UnifiedThreadPoolSheduledTask& Join();
				UnifiedThreadPoolSheduledTask& Join(Time::DateTime timeOut);
				const UnifiedThreadPoolSheduledTask& Join() const;
				const UnifiedThreadPoolSheduledTask& Join(Time::DateTime timeOut) const;
			public:
				static constexpr auto MaxIntervalLenght = Time::TimeSpan().AddMiliSeconds(Math::Max<Int32>());
			public:
				RefCountClass(UnifiedThreadPoolSheduledTask, This);
			};

			class UnifiedThreadPool final : protected IDisposable {
			public:
				enum class EPoolThreadsPriority {
					Low, Medium, High
				};
			private:
				friend class UnifiedThreadPoolJob;
				friend class UnifiedThreadPoolSheduledTask;
				friend class UnifiedThreadPoolSheduledTaskBuilder;
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
					Containers::ConcurencyList<UnifiedThreadPoolSheduledTask> TaskQueue;
					Containers::ConcurencyList<SmartPtr<IO::IRPBase>> IrpQueue;

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
						RefCount(1)
					{}
				};
			private:
				Data* This;
			protected:
				virtual void Dispose() override {
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
				friend bool IsUnifiedThreadPoolTerminate(UnifiedThreadPoolSheduledTask* task);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolJob* jobObj);
				friend SRWLock& GetUnifiedPoolLock(UnifiedThreadPoolSheduledTask* task);

				friend void UnregisterJobFromJobList(UnifiedThreadPoolJob* jobObj);
				friend void UnRegisterTaskFromPoolQueue(UnifiedThreadPoolSheduledTask* thread);
			private:
				UnifiedThreadPool(Data*&& This) :This(MoveRValue(This)) {}
				UnifiedThreadPool(Data*& This) :This(This) { ++This->RefCount;  }
			public:
				UnifiedThreadPoolJob PostJob(String&& jobName, Delegate&& action);

				UnifiedThreadPoolJob PostJob(Delegate&& action) {
					return PostJob(String(), MoveRValue(action));
				}
				UnifiedThreadPoolJob PostJob(const String& jobName, Delegate&& action) {
					return PostJob(String(jobName), MoveRValue(action));
				}
			public:
				UnifiedThreadPoolSheduledTaskBuilder SheduleJob(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);
				UnifiedThreadPoolSheduledTaskBuilder SheduleJob(String&& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);
				UnifiedThreadPoolSheduledTaskBuilder SheduleJob(const String& taskName, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback);
			public:
				SizeT GetJobsCount() const { return This->JobsQueue.Count; }
				SizeT GetSheduledTaskCount() const { return This->TaskQueue.Count; }
				SizeT GetIORequestCount() const { return This->IrpQueue.Count; }
			public:
				Containers::List<UnifiedThreadPoolJob> GetJobsQueue() const { return This->JobsQueue.CopyTo<Containers::List>(); }
				Containers::List<UnifiedThreadPoolSheduledTask> GetSheduledTaskQueue() const { return This->TaskQueue.CopyTo<Containers::List>(); }
				Containers::List<SmartPtr<IO::IRPBase>> GetIrpQueue() const { return This->IrpQueue.CopyTo<Containers::List>(); }
			public:
				SizeT Property(GetJobsCount) JobsCount;
				SizeT Property(GetSheduledTaskCount) SheduledTaskCount;
				SizeT Property(GetIORequestCount) IORequestCount;
			public:
				const String& GetName() const { return This->threadPoolName; }
			public:
				String Property(GetName) Name;
			public:
				EPoolThreadsPriority GetJobsPriority() const { return This->handlesData.JobConfiguration.Priority; }
				EPoolThreadsPriority GetSheduledTasksPriority() const { return This->handlesData.TaskConfiguration.Priority; }
				EPoolThreadsPriority GetIORequestPriority() const { return This->handlesData.IrpConfiguration.Priority; }
			public:
				EPoolThreadsPriority Property(GetJobsPriority) JobsPriority;
				EPoolThreadsPriority Property(GetSheduledTasksPriority) SheduledTasksPriority;
				EPoolThreadsPriority Property(GetIORequestPriority) IORequestPriority;
			public: 
				RefCountClass(UnifiedThreadPool, This);
			};

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
				UnifiedThreadPoolBuilder() :This(new Data(
					String(),
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount / 2,
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium)) {}

				UnifiedThreadPoolBuilder(String&& threadPoolName) :This(new Data(
					MoveRValue(threadPoolName),
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount / 2,
					(UInt32)CurrentSettings().VirtualSetting.CpuCoresCount,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium,
					UnifiedThreadPool::EPoolThreadsPriority::Medium)) {}

				UnifiedThreadPoolBuilder(const String& threadPoolName) : UnifiedThreadPoolBuilder(String(threadPoolName)) {}
			public:
				UnifiedThreadPoolBuilder&& SetThreadPoolPriority(UnifiedThreadPool::EPoolThreadsPriority newThreadPoolPriority) && {
					This->irpRequestPriority = newThreadPoolPriority;
					This->jobsPriority = newThreadPoolPriority;
					This->sheduledTasksPriority = newThreadPoolPriority;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolBuilder&& SetIOThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newIoThreadsPriority) && {
					This->irpRequestPriority = newIoThreadsPriority;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolBuilder&& SetShedulerThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newShedulerThreadsPriority) && {
					This->sheduledTasksPriority = newShedulerThreadsPriority;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolBuilder&& SetJobThreadsPriority(UnifiedThreadPool::EPoolThreadsPriority newJobThreadsPriority) && {
					This->jobsPriority = newJobThreadsPriority;
					return MoveRValue(*this);
				}
			public:
				UnifiedThreadPoolBuilder&& SetMinThreadCount(UInt32 newMinThreadCount) && {
					This->minThreadCount = newMinThreadCount;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolBuilder&& SetMaxThreadCount(UInt32 newMaxThreadCount) && {
					This->maxThreadCount = newMaxThreadCount;
					return MoveRValue(*this);
				}
			public:
				UnifiedThreadPoolBuilder&& SetName(String&& newName) && {
					This->threadPoolName = MoveRValue(newName);
					return MoveRValue(*this);
				}

				UnifiedThreadPoolBuilder&& SetName(const String& newName) && {
					This->threadPoolName = newName;
					return MoveRValue(*this);
				}
			public:
				UnifiedThreadPool CreateThreadPool() &&;
			private:
				UnifiedThreadPool::PoolConfigureation CreateJobConfiguration(ImplSpecificT PoolHandle);
				UnifiedThreadPool::PoolConfigureation CreateSheduledTaskConfiguration(ImplSpecificT PoolHandle);
				UnifiedThreadPool::PoolConfigureation CreateIrpConfiguration(ImplSpecificT PoolHandle);
			public:
				~UnifiedThreadPoolBuilder()
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
				UnifiedThreadPoolBuilder(UnifiedThreadPoolBuilder&& orig) : This(orig.This) { orig.This = nullptr; }

				UnifiedThreadPoolBuilder& operator=(UnifiedThreadPoolBuilder&& orig) {
					this->~UnifiedThreadPoolBuilder();
					this->UnifiedThreadPoolBuilder::UnifiedThreadPoolBuilder(MoveRValue(orig));
					return *this;
				}
			};

			class UnifiedThreadPoolSheduledTaskBuilder final {
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
				};
			private:
				Data* This;
			private:
				UnifiedThreadPoolSheduledTaskBuilder(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)
					: This(new Data(ownerPool, MoveRValue(callback))) {}
				UnifiedThreadPoolSheduledTaskBuilder(UnifiedThreadPool& ownerPool, SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback, String&& taskName)
					: This(new Data(ownerPool, MoveRValue(callback), MoveRValue(taskName))) {}
			public:
				UnifiedThreadPoolSheduledTaskBuilder&& SetCallback(SmartPtr<IUnifiedThreadPoolShedulerCallback>&& callback)&& {
					This->callback = MoveRValue(callback);
					return MoveRValue(*this);
				}
			public:
				UnifiedThreadPoolSheduledTaskBuilder&& SetName(String&& name)&& {
					This->taskName = MoveRValue(name);
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetName(const String& name)&& {
					This->taskName = name;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetExecutionCount(UInt16 count)&& {
					This->intervalsCount = count;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetIntervalLenght(Time::TimeSpan intervalLenght)&& {
					if(This->intervalLenght > MaxIntervalLenght)
						Throw(InvalidArgumentException(ExceptionPosition, L"IntervalLenght", L"Too big interval lenght"));
					This->intervalLenght = intervalLenght;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetStartTime(Time::DateTime absoluteStartTime)&& {
					This->startTime = absoluteStartTime.TimeFromStartEpoch;
					This->isRelaive = false;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetStartTime(Time::TimeSpan relativeStartTime)&& {
					This->startTime = relativeStartTime.Ticks;
					This->isRelaive = true;
					return MoveRValue(*this);
				}

				UnifiedThreadPoolSheduledTaskBuilder&& SetProximity(Time::TimeSpan proximity)&& {
					This->proximity = proximity;
					return MoveRValue(*this);
				}
			public:
				UnifiedThreadPoolSheduledTask Shedule()&&;
			public:
				static constexpr auto MaxIntervalLenght = UnifiedThreadPoolSheduledTask::MaxIntervalLenght;
			public:
				~UnifiedThreadPoolSheduledTaskBuilder()
				{
					if (This != nullptr) {
						delete This;
					}
				}
			};

			class ThreadableUnifiedThreadPoolJob;
			class UnifiedThreadPoolIoTask {};
		}

		namespace IO {
			class IAsyncIOBase : virtual protected IDisposable {
			protected:
				Threading::UnifiedThreadPool threadPool;
			protected:
				//TODO: void RegisterNtIoObjectInThreadPool(Handle ntIoObjectHandle);
			public:
				IAsyncIOBase(const Threading::UnifiedThreadPool& threadPool) : threadPool(threadPool) {}
				IAsyncIOBase(const IAsyncIOBase&) = default;
				IAsyncIOBase(IAsyncIOBase&&) = default;
			public:
				virtual Handle GetIOObjectHandle() = 0;
			public:
				virtual ~IAsyncIOBase() = 0 {}
			};

			template<typename TDataType = Containers::Blob>
			class IAsyncIO : virtual public IAsyncIOBase {
			public:
				IAsyncIO(const Threading::UnifiedThreadPool& threadPool) : IAsyncIOBase(threadPool) {}
				IAsyncIO(const IAsyncIO&) = default;
				IAsyncIO(IAsyncIO&&) = default;
			public:
				virtual void AsyncRead(TDataType&& data, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SmartPtr<IExceptionCallback> onErrorAction) = 0;
			public:
				virtual void IsValid() = 0;
			public:
				virtual ~IAsyncIO() = 0 {};
			};

			template<typename TDataType = Containers::Blob>
			class IPositionAsyncIO : virtual public IAsyncIOBase {
			public:
				virtual void AsyncRead(TDataType&& data, SizeT position, SmartPtr<IFinishReadCallback<TDataType>> onFinishReadAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IFinishWriteCallback<TDataType>> onFinishWriteAction) = 0;
				virtual void AsyncWrite(const TDataType&& data, SizeT position, SmartPtr<IExceptionCallback> onErrorAction) = 0;
			public:
				virtual void IsValid() = 0;
			public:
				virtual ~IPositionAsyncIO() = 0 {};
			};

			class IRPBase {
			public:
				enum class ECallbackType {
					Read, Write, Exception
				};
			private:
				ID irpId;
				ECallbackType irpCallbackType;
				SmartPtr<IAsyncIOBase> irpTarget;
			public:
				IRPBase(ID irpId, SmartPtr<IAsyncIOBase>&& irpTarget, ECallbackType irpCallbackType)
					: irpId(irpId), irpCallbackType(irpCallbackType), irpTarget(irpTarget) {}

				IRPBase(const IRPBase&) = default;
				IRPBase(IRPBase&&) = default;
			public:
				virtual void OnIORequestComplete(Boolean isSuccess, UInt32 errorCode) = 0;
			public:
				ID GetIrpId() { return irpId; }

				ECallbackType GetIrpCallbackType() const { return irpCallbackType; }

				const SmartPtr<IAsyncIOBase>& GetIrpTarget() const { return irpTarget; }
			public:
				ID Property(GetIrpId) Id;

				ECallbackType Property(GetIrpCallbackType) CallbackType;

				SmartPtr<IAsyncIOBase> Property(GetIrpTarget) Target;
			public:
				virtual ~IRPBase() = 0 {}
			};

			template<typename TDataType>
			class ReadIRP final : public IRPBase {
			private:
				SmartPtr<IFinishReadCallback<TDataType>> finishReadCallback;
				TDataType buffer;
			public:
				ReadIRP(ID irpId, SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IFinishReadCallback<TDataType>>&& FinishReadCallback, TDataType&& buffer) :
					IRPBase(irpId, MoveRValue(irpTarget), ECallbackType::Read), finishReadCallback(MoveRValue(FinishReadCallback)), buffer(MoveRValue(buffer)) {}

				ReadIRP(const ReadIRP&) = default;
				ReadIRP(ReadIRP&&) = default;
				ReadIRP& operator=(const ReadIRP&) = default;
				ReadIRP& operator=(ReadIRP&&) = default;
			public:
				virtual void OnIORequestComplete(Boolean isSuccess, UInt32 errorCode) override {
					if (isSuccess) {
						finishReadCallback->OnReadFinish(MoveRValue(buffer));
					}
					else {
						finishReadCallback->OnError(errorCode);
					}
				}
			public:
				virtual ~ReadIRP() override {}
			};

			template<typename TDataType>
			class WriteIRP final : public IRPBase {
			private:
				SmartPtr<IFinishWriteCallback<TDataType>> finishWriteCallback;
				TDataType buffer;
			public:
				WriteIRP(ID irpId, SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IFinishWriteCallback<TDataType>>&& finishWriteCallback, TDataType&& buffer) :
					IRPBase(irpId, MoveRValue(irpTarget), ECallbackType::Read), finishWriteCallback(MoveRValue(finishWriteCallback)), buffer(MoveRValue(buffer)) {}

				WriteIRP(const WriteIRP&) = default;
				WriteIRP(WriteIRP&&) = default;
				WriteIRP& operator=(const WriteIRP&) = default;
				WriteIRP& operator=(WriteIRP&&) = default;
			public:
				virtual void OnIORequestComplete(Boolean isSuccess, UInt32 errorCode) override {
					if (isSuccess) {
						finishWriteCallback->OnWriteFinish(MoveRValue(buffer));
					}
					else {
						finishWriteCallback->OnError(errorCode);
					}
				}
			public:
				virtual ~WriteIRP() override {}
			};

			class ExceptionIRP final : public IRPBase {
			private:
				SmartPtr<IExceptionCallback> exceptionCallback;
			public:
				ExceptionIRP(ID irpId, SmartPtr<IAsyncIOBase>&& irpTarget, SmartPtr<IExceptionCallback>&& exceptionCallback) :
					IRPBase(irpId, MoveRValue(irpTarget), ECallbackType::Read), exceptionCallback(MoveRValue(exceptionCallback)) {}

				ExceptionIRP(const ExceptionIRP&) = default;
				ExceptionIRP(ExceptionIRP&&) = default;
				ExceptionIRP& operator=(const ExceptionIRP&) = default;
				ExceptionIRP& operator=(ExceptionIRP&&) = default;
			public:
				virtual void OnIORequestComplete(Boolean isSuccess, UInt32 errorCode) override {
					if (!isSuccess) {
						exceptionCallback->OnError(errorCode);
					}
				}
			public:
				virtual ~ExceptionIRP() override {}
			};
		}
	}
}