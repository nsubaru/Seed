#pragma once
#include "Seed.Core.Testing.TestResult.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Testing
		{
			class SeedKernelCall TestPackResult : IDisposable, public Serilization::ISerilizable<TestPackResult> {
			private:
				struct Data {
					String Name;
					Containers::List<TestResult> Results;
					Time::TimeRange TestTimeRange;
					RefCountT RefCount;

					Data(const String& name, const Containers::List<TestResult>& results, Time::TimeRange testTimeRange)
						: Name(name), Results(results), TestTimeRange(testTimeRange), RefCount(1) {}

					Data(String&& name, const Containers::List<TestResult>& results, Time::TimeRange testTimeRange)
						: Name(MoveRValue(name)), Results(results), TestTimeRange(testTimeRange), RefCount(1) {}

					Data(const String& name, Containers::List<TestResult>& results, Time::TimeRange testTimeRange)
						: Name(name), Results(MoveRValue(results)), TestTimeRange(testTimeRange), RefCount(1) {}

					Data(String&& name, Containers::List<TestResult>&& results, Time::TimeRange testTimeRange)
						: Name(MoveRValue(name)), Results(MoveRValue(results)), TestTimeRange(testTimeRange), RefCount(1) {}
				};
			public:
				DefStructMetaData(Data,
					DefStructFieldData(Data, Name),
					DefStructFieldData(Data, TestTimeRange),
					DefStructFieldData(Data, Results)
				);
			private:
				Data* This;
			private:
				virtual Byte* GetDataPtr() const override {
					return reinterpret_cast<Byte*>(This);
				}
			private:
				virtual void Dispose() override {}
			public:
				TestPackResult() 
					: This(new Data(String(), {}, {})) {}

				TestPackResult(const String& name, const Containers::List<TestResult>& results, Time::TimeRange testTimeRange)
					: This(new Data(name, results, testTimeRange)) {}

				TestPackResult(String&& name, const Containers::List<TestResult>& results, Time::TimeRange testTimeRange)
					: This(new Data(MoveRValue(name), results, testTimeRange)) {}

				TestPackResult(const String& name, Containers::List<TestResult>&& results, Time::TimeRange testTimeRange)
					: This(new Data(name, MoveRValue(results), testTimeRange)) {}

				TestPackResult(String&& name, Containers::List<TestResult>&& results, Time::TimeRange testTimeRange)
					: This(new Data(MoveRValue(name), MoveRValue(results), testTimeRange)) {}
			public:
				const String& GetName() const {
					return This->Name;
				}
			public:
				const Containers::List<TestResult>& GetResults() const {
					return This->Results;
				}
			public:
				Time::DateTime GetStartTime() const {
					return This->TestTimeRange.StartTime;
				}
			public:
				Time::DateTime GetEndTime() const {
					return This->TestTimeRange.StartTime.Ticks + This->TestTimeRange.Durration.Ticks;
				}
			public:
				Time::TimeSpan GetDuration() const {
					return This->TestTimeRange.Durration;
				}
			public:
				Time::TimeRange GetTestTimeRange() const {
					return This->TestTimeRange;
				}
			public:
				SizeT GetSucessCount() const {
					return This->Results.GetCountIf([] (const TestResult& result) InlineLambda
					{
						return result.Status == TestResult::EStatus::Success;
					});
				}
			public:
				SizeT GetFailCount() const {
					return This->Results.GetCountIf([] (const TestResult& result) InlineLambda
					{
						return result.Status == TestResult::EStatus::Failure;
					});
				}
			public:
				SizeT GetExceptionCount() const {
					return This->Results.GetCountIf([] (const TestResult& result) InlineLambda
					{
						return result.Status == TestResult::EStatus::Exception;
					});
				}
			public:
				SizeT GetCanceledCount() const {
					return This->Results.GetCountIf([](const TestResult& result) InlineLambda
					{
						return result.Status == TestResult::EStatus::Canceled;
					});
				}
			public:
				SizeT GetExecutedCount() const {
					return This->Results.GetCountIf([](const TestResult& result) InlineLambda
					{
						return result.Status != TestResult::EStatus::Canceled;
					});
				}
			public:
				Boolean GetIsAllSuccess() const {
					return GetFailCount() == 0 && GetExceptionCount() == 0;
				}
			public:
				Boolean GetIsAllExecuted() const {
					return GetCanceledCount() == 0;
				}
			public:
				String Property(GetName) Name;
				Time::DateTime Property(GetStartTime) StartTime;
				Time::DateTime Property(GetEndTime) EndTime;
				Time::TimeSpan Property(GetDuration) Duration;
				Time::TimeRange Property(GetTestTimeRange) TestTimeRange;
				SizeT Property(GetSucessCount) SucessCount;
				SizeT Property(GetFailCount) FailCount;
				SizeT Property(GetExceptionCount) ExceptionCount;
				SizeT Property(GetCanceledCount) CanceledCount;
				SizeT Property(GetExecutedCount) ExecutedCount;
				Boolean Property(GetIsAllSuccess) IsAllSucess;
				Boolean Property(GetIsAllExecuted) IsAllExecuted;
				Containers::List<TestResult> Property(GetResults) Results;
			public:
				RefCountClass(TestPackResult, This)
			};
		}
	}
}