#pragma once
#include "typeinfo"
#include "Seed.Core.Base.h"
#include "Seed.Core.String.h"
#include "Seed.Core.Containers.Concurency.List.h"
#include "Seed.Core.Exceptions.h"
#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"
#include "Seed.Core.Serilization.h"

namespace Seed
{
	inline namespace Core
	{
		namespace Testing
		{
			class TestResult : IDisposable, public Serilization::ISerilizable<TestResult> {
			public:
				enum class EStatus : UInt8 {
					Success, Failure, Exception
				};
			private:
				struct Data {
					EStatus Status;
					String Name;
					String Message;
					Time::TimeSpan Durration;

					RefCountT RefCount;

					Data(EStatus status, const String& name, const String& message, Time::TimeSpan durration) :
						Status(status), Name(name), Message(message), Durration(durration), RefCount(1) {}

					Data(EStatus status, String&& name, const String& message, Time::TimeSpan durration) :
						Status(status), Name(MoveRValue(name)), Message(message), Durration(durration), RefCount(1) {}

					Data(EStatus status, const String& name, String&& message, Time::TimeSpan durration) :
						Status(status), Name(name), Message(MoveRValue(message)), Durration(durration), RefCount(1) {}

					Data(EStatus status, String&& name, String&& message, Time::TimeSpan durration) :
						Status(status), Name(MoveRValue(name)), Message(MoveRValue(message)), Durration(durration), RefCount(1) {}

				};
			public:
				DefStructMetaData(Data,
					DefStructFieldData(Data, Name),
					DefStructFieldData(Data, Status),
					DefStructFieldData(Data, Message),
					DefStructFieldData(Data, Durration)
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
				TestResult(const String& testName, EStatus status, const String& message, Time::TimeSpan durration)
					: This(new Data(status, testName, message, durration)) {}
				TestResult(String&& testName, EStatus status, const String& message, Time::TimeSpan durration)
					: This(new Data(status, MoveRValue(testName), message, durration)) {}
				TestResult(const String& testName, EStatus status, String&& message, Time::TimeSpan durration)
					: This(new Data(status, testName, message, durration)) {}
				TestResult(String&& testName, EStatus status, String&& message, Time::TimeSpan durration)
					: This(new Data(status, MoveRValue(testName), MoveRValue(message), durration)) {}

				EStatus GetStatus() const {
					return This->Status;
				}

				const String& GetMessage() const {
					return This->Message;
				}

				const String& GetName() const {
					return This->Name;
				}

				Time::TimeSpan GetDurration() const {
					return This->Durration;
				}

				EStatus Property(GetStatus) Status;
				String Property(GetMessage) Message;
				String Property(GetName) Name;
				Time::TimeSpan Property(GetDurration) Durration;
			public:
				RefCountClass(TestResult, This)
			};
		}
	}
}