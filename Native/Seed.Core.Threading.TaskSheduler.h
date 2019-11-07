#pragma once
#include "Seed.Core.Threading.Base.h"
#include "Seed.Core.DateTime.h"

namespace Seed {
	inline namespace Core {
		namespace Threading {
			interface ITask;
			class SheduledJob;
			class SheduledJobBuilder;
			class TaskSheduler;

			interface ITask
			{
				void Execute(DateTime startTimeUtc) volatile;
			};

			class SeedKernelCall TaskSheduler final : protected IDisposable{
				struct Data {
				///<summary>Керуюча структура пула потоків</summary>
				ImplSpecificT pool;
				///<summary>Настройки середовищп для пула потоків</summary>
				ImplSpecificT env;
				///<summary>Настройка очищення задач</summary>
				ImplSpecificT cleanupData;
				///<summary>Лічильник силок</summary>
				RefCountT RefCount;
				///<summary>Кількість задач у пулі</summary>
				RefCountT TaskCount;
				///<summary>Ім'я пула задач</summary>
				String Name;

				SRWLock lock;

				Boolean IsTerminating;

				Data(const String& Name) :
					Name(Name),
					pool(nullptr),
					env(nullptr),
					cleanupData(nullptr),
					IsTerminating(false),
					lock(),
					RefCount(1),
					TaskCount(0)
				{}

				Data(String&& Name) :
					Name(MoveRValue(Name)),
					pool(nullptr),
					env(nullptr),
					cleanupData(nullptr),
					IsTerminating(false),
					lock(),
					RefCount(1),
					TaskCount(0)
				{}
			};
		private:
			Data* This;
		private:
			void InitTaskSheduler();
			TaskSheduler(Data* This) :This(This) { ++This->RefCount; }
		private:
			friend SRWLock GetPoolLock(SheduledJob* thread);
			friend Boolean IsPoolTerminating(SheduledJob* thread);
		protected:
			virtual void Dispose() override final;
		public:
			TaskSheduler() : This(new Data("Unnamed Task Sheduler")) { InitTaskSheduler(); }
			TaskSheduler(const String& Name) : This(new Data(Name)) { InitTaskSheduler(); }
			TaskSheduler(String&& Name) : This(new Data(MoveRValue(Name))) { InitTaskSheduler(); }
			[[nodiscard]] SheduledJobBuilder NewTask(ITask*&& task);

			SizeT GetTaskCount() { return this->This->TaskCount; }
			SizeT Property(GetTaskCount) TaskCount;
		public:
			RefCountClass(TaskSheduler, This)
		public:
			friend class SheduledJob;
			friend class SheduledJobBuilder;
			};

			class SheduledJob final : IDisposable {
			private:
				struct Data {
					volatile Int32 PeriodLenght;

					volatile Int32 TimerProximity;

					volatile Int32 PeriodsCount;

					volatile ITask* Task;

					volatile Handle hTask;

					volatile DateTime LastStartTime;

					volatile DateTime StartTime;

					volatile EThreadabledStates State;

					volatile ID TID;

					Containers::ConcurencySensitiveList<SmartPtr<IThreadable>>::Iterator This;

					TaskSheduler::Data* sheduler;

					Handle hThread;

					String Name;

					RefCountT ExucutedCount;
					RefCountT RemainingPeriodsCount;
					RefCountT RefCount;

					Data(ITask* Task, TaskSheduler::Data* sheduler) :
						Task(Task),
						PeriodLenght(0),
						TimerProximity(0),
						PeriodsCount(1),
						hTask(nullptr),
						LastStartTime(0),
						StartTime(0),
						State(EThreadabledStates::NotStarted),
						TID(0),
						sheduler(sheduler),
						hThread(nullptr),
						Name(L"Unnamed Task"),
						ExucutedCount(0),
						RemainingPeriodsCount(0),
						RefCount(1),
						This()
					{}
				};
			private:
				Data* This;
			private:
				SheduledJob(Data* This) : This(This) { ++This->RefCount; }
			private:
				friend void RegisterThread(SheduledJob* thread);
				friend void UnRegisterThread(SheduledJob* thread);
				friend volatile ITask& GetJobEntryPoint(SheduledJob* thread);
				friend SRWLock GetPoolLock(SheduledJob* thread);
				friend Boolean IsPoolTerminating(SheduledJob* thread);
			protected:
				virtual void Dispose() override final;
			public:
				TaskSheduler& GetThreadPool() { return TaskSheduler(This->sheduler); }
				EThreadabledStates GetCurrentState() { return This->State; }
				SheduledJob& Interupt();
				SheduledJob& Join();
				SheduledJob& Suspend();
				SheduledJob& Join(DateTime timeOut);
			public:
				const String& GetName() { return This->Name; }
				void SetName(const String& NewName) { This->Name = NewName; }
				void SetName(String&& NewName) { This->Name = MoveRValue(NewName); }
			public:
				RefCountClass(SheduledJob, This)
			public:
				friend class SheduledJobBuilder;
				friend class ThreadableSheduledTask;
			};

			class SeedKernelCall SheduledJobBuilder final {
			private:
				SheduledJob::Data* This;
				Boolean IsExcecutionTimeRelative;
			private:
				SheduledJobBuilder(TaskSheduler& sheduler, ITask*&& task) : This(new SheduledJob::Data(MoveRValue(task), sheduler.This)), IsExcecutionTimeRelative(false) {}
			public:
				SheduledJobBuilder(const SheduledJobBuilder&) = delete;
				SheduledJobBuilder(SheduledJobBuilder&& orig) : This(orig.This), IsExcecutionTimeRelative(orig.IsExcecutionTimeRelative) { orig.This = nullptr; }
				SheduledJobBuilder& operator=(const SheduledJobBuilder&) = delete;
				SheduledJobBuilder& operator=(SheduledJobBuilder&& orig) { Swap(this->This, orig.This); return *this; }
				~SheduledJobBuilder()
				{
					if (This != nullptr) {
						if (--This->RefCount == 0) {
							if (This->Task != nullptr) { //-V809
								delete This->Task;
							}
							delete This;
						}
						This = nullptr;
					}
				}
			public:
				SheduledJobBuilder& SetName(const String& Name) { This->Name = Name; return *this; }
				SheduledJobBuilder& SetName(String&& Name) { This->Name = MoveRValue(Name); return *this; }
				SheduledJobBuilder& SetExecutionTime(DateTime time, Boolean IsExcecutionTimeRelative) { This->StartTime = time; this->IsExcecutionTimeRelative = IsExcecutionTimeRelative; return *this; } //-V813
				SheduledJobBuilder& SetLoopDuration(DateTime time) { This->PeriodLenght = (Int32)time.TimeFromStartEpoch; return *this; } //-V813
				SheduledJobBuilder& SetLoopsCount(Int32 Count) { This->PeriodsCount = Count; This->RemainingPeriodsCount = Count; return *this; }
				SheduledJobBuilder& SetTimerProximity(DateTime proximity) { This->TimerProximity = (Int32)proximity.TimeFromStartEpoch; return *this; } //-V813
				[[maybe_unused]] SheduledJob Shedule();
				void Cancel() { this->~SheduledJobBuilder(); }

			public:
				friend class TaskSheduler;
			};

			inline SheduledJobBuilder TaskSheduler::NewTask(ITask*&& task)
			{
				return SheduledJobBuilder(*this, MoveRValue(task));
			}
		}
	}
}