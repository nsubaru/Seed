#pragma once
#include "Seed.Core.Stage1.h"

//Allow complex unnamed object, unnamed object often appear as iterator 
//or when function have side-effect for example starting thread or task
#pragma warning(disable: 26444)

namespace Seed {
	inline namespace Core {
		namespace TaskSheduler {
			///<summary>Інтерфейс задачі в планувальнику задач</summary>
			interface ITask
			{
				///<summary>Метод який викличеться при наступлені часу</summary>
				///<param name = "Now">Час виклику</param>
				void Execute(const DateTime& Now) volatile;
			};
			//Задача в планувальнику задач
			//При завершені роботи всі задачі автоматично знищаться
			class SeedKernelCall Job : protected IDisposable {
			public:
				//Дані задачі
				struct __task {
					volatile Int32 PeriodLenght = 0;

					volatile Int32 TimerProximity = 0;

					Int32 PeriodsCount = 1;

					//Покажчик на колбек
					volatile ITask* What = nullptr;

					volatile Handle hTask = nullptr;
					
					volatile DateTime LastStartTime = 0;

					volatile DateTime StartTime = 0;

					volatile Boolean IsExcecute = false;

					volatile Boolean IsCanceled = false; 

					volatile ID TID = 0;

					RefCountT ExucutedCount = 0;
					RefCountT RemainingPeriodsCount = 0;
					RefCountT RefCount = 1;

					__task(Int32 TimerProximity) :
						TimerProximity(TimerProximity)
					{}
				};
			private:
				//Дані цієї задачі
				__task* This;
				//Внутрішінй конструктор
				Job(__task* This) :This(This) { ++This->RefCount; }
			protected:
				virtual void Dispose() override;
			public:
				RefCountClass(Job, This);
				//Вертає true якщо задача скасована
				
				Boolean GetIsExcecute() { return This->IsExcecute; }
				Boolean GetIsCanceled() { return This->IsCanceled; }

				DateTime GetStartTime() { return const_cast<DateTime&>(This->StartTime); }
				void SetStarTime(volatile DateTime& NewStartTime) { This->StartTime = NewStartTime; }
				DateTime GetLastStartTime() { return const_cast<DateTime&>(This->LastStartTime); }
				
				DateTime GetPeriodLenght() { return This->PeriodLenght * DateTime::SecondInNtTime; }
				void SetPeriodLenght(DateTime PeriodLenght);
				DateTime GetTimerProximity() { return This->TimerProximity; }
				void SetTimerProximity(DateTime NewTimerProximity);
				
				Int64 GetPeriodsCount() { return This->PeriodsCount; }
				void SetPeriodsCount(Int32 NewPeriodsCount);

				Int64 GetExucutedCount() { return This->ExucutedCount; }
				Int64 GetRemainingExecutionCount() { return This->RemainingPeriodsCount; }

				const ITask& GetCurrentTask() { return const_cast<const ITask&>(*This->What); }

				Boolean Property(GetIsExcecute) IsExcecute;
				Boolean Property(GetIsCanceled) IsCanceled;
				DateTime Property(GetPeriodLenght, SetPeriodLenght) PeriodLenght;
				DateTime Property(GetTimerProximity, SetTimerProximity) TimerProximity;
				Int64 Property(GetExucutedCount) ExucutedCount;
				Int64 Property(GetRemainingExecutionCount) RemainingExecutionCount;

				//Скасовує задачу
				void Cancel();
				
				friend class Task;

				// Inherited via IDisposable
				
			};

			class Task {
			private:
				Job::__task * Data;
			public:
				Task(const Task&) = delete;
				Task& operator=(const Task&) = delete;
				Task(Task&&) = default;
				Task& operator=(Task&&) = default;
			public:
				Task() : Data(new Job::__task(CurrentSettings().KernelService.TaskSheduler.DefaultTimerProximity)) {}
			public:
				Task& SetStartTime(DateTime StartTime, Boolean IsRelative) { Data->StartTime.TimeFromStartEpoch = (IsRelative) ? (StartTime + DateTime::GetCurrentSystemTimeData()).GetMsFrom1600() : StartTime.GetMsFrom1600(); return *this; }
				Task& SetTimerProximity(DateTime TimerProximity) { Data->TimerProximity = static_cast<Int32>(TimerProximity.GetMsFrom1600()); return *this; }
				Task& SetTimerPeriods(DateTime PeriodLenght) { Data->PeriodLenght = static_cast<Int32>(PeriodLenght.GetMsFrom1600()); return *this; }
				Task& SetTimerPeriods(DateTime PeriodLenght, Int32 PeriodsCount) { Data->PeriodLenght = static_cast<Int32>(PeriodLenght.GetMsFrom1600()); Data->PeriodsCount = PeriodsCount; return *this; }
				Task& SetTask(ITask*&& Task) { if (Data->What != nullptr) delete Data->What; Data->What = Task; Task = nullptr; return *this; }
				
				Job Shedule();

				[[maybe_unused]]
				static Task NewTask() { return Task(); }
			};
		}
	}
}