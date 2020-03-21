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
					DateTime StartTime;
					DateTime EndTime;
					RefCountT RefCount;

					Data(const String& name, const Containers::List<TestResult>& results, DateTime startTime, DateTime endTime)
						: Name(name), Results(results), StartTime(startTime), EndTime(endTime), RefCount(1) {}

					Data(String&& name, const Containers::List<TestResult>& results, DateTime startTime, DateTime endTime)
						: Name(MoveRValue(name)), Results(results), StartTime(startTime), EndTime(endTime), RefCount(1) {}

					Data(const String& name, Containers::List<TestResult>& results, DateTime startTime, DateTime endTime)
						: Name(name), Results(MoveRValue(results)), StartTime(startTime), EndTime(endTime), RefCount(1) {}

					Data(String&& name, Containers::List<TestResult>&& results, DateTime startTime, DateTime endTime)
						: Name(MoveRValue(name)), Results(MoveRValue(results)), StartTime(startTime), EndTime(endTime), RefCount(1) {}
				};
			public:
				DefStructMetaData(Data,
					DefStructFieldData(Data, Name),
					DefStructFieldData(Data, StartTime),
					DefStructFieldData(Data, EndTime),
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
				TestPackResult(const String& name, const Containers::List<TestResult>& results, DateTime startTime, DateTime endTime)
					: This(new Data(name, results, startTime, endTime)) {}

				TestPackResult(String&& name, const Containers::List<TestResult>& results, DateTime startTime, DateTime endTime)
					: This(new Data(MoveRValue(name), results, startTime, endTime)) {}

				TestPackResult(const String& name, Containers::List<TestResult>&& results, DateTime startTime, DateTime endTime)
					: This(new Data(name, MoveRValue(results), startTime, endTime)) {}

				TestPackResult(String&& name, Containers::List<TestResult>&& results, DateTime startTime, DateTime endTime)
					: This(new Data(MoveRValue(name), MoveRValue(results), startTime, endTime)) {}
			public:
				const String& GetName() const {
					return This->Name;
				}
			public:
				const Containers::List<TestResult>& GetResults() const {
					return This->Results;
				}
			public:
				DateTime GetStartTime() const {
					return This->StartTime;
				}
			public:
				DateTime GetEndTime() const {
					return This->EndTime;
				}
			public:
				DateTime GetDuration() const {
					return This->EndTime - This->StartTime;
				}
			public:
				SizeT GetSucessCount() const {
					return This->Results.GetCountIf([] (const TestResult& result)
					{
						return result.Status == TestResult::EStatus::Success;
					});
				}
			public:
				SizeT GetFailCount() const {
					return This->Results.GetCountIf([] (const TestResult& result)
					{
						return result.Status == TestResult::EStatus::Failure;
					});
				}
			public:
				SizeT GetExceptionCount() const {
					return This->Results.GetCountIf([] (const TestResult& result)
					{
						return result.Status == TestResult::EStatus::Exception;
					});
				}
			public:
				Boolean GetIsAllSucess() const {
					return GetFailCount() == 0 && GetExceptionCount() == 0;
				}
			public:
				String Property(GetName) Name;
				DateTime Property(GetStartTime) StartTime;
				DateTime Property(GetEndTime) EndTime;
				DateTime Property(GetDuration) Duration;
				SizeT Property(GetSucessCount) SucessCount;
				SizeT Property(GetFailCount) FailCount;
				SizeT Property(GetExceptionCount) ExceptionCount;
				Boolean Property(GetIsAllSucess) IsAllSucess;
				Containers::List<TestResult> Property(GetResults) Results;
			public:
				RefCountClass(TestPackResult, This)
			};
		}
	}
}