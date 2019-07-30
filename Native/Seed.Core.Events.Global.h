//#pragma once
//#include "Seed.Core.Base.h"
//#include "Seed.Core.Containers.List.h"
//#include "Seed.Core.Containers.Map.h"
//#include "Seed.Core.Containers.Dynamic.h"
//
//namespace Seed {
//	inline namespace Core {
//		namespace Events {
//			typedef void(*EventCallbackF)(const Dynamic& EventData, const Dynamic& AdditionalEventData);
//
//			interface IEventCallback {
//				void OnEvent(const Dynamic & EventData, const Dynamic & AdditionalEventData);
//			};
//
//			interface IEvent
//			{
//				const UInt64 getEventCallbacksCount();
//				const Boolean getIsAsync();
//
//				UInt64 Property(getEventCallbacksCount) EventCallbacksCount;
//				Boolean Property(getIsAsync) IsAsync;
//
//				void RegisterEventCallback(IEventCallback*&& Callback, TypeInfo&& Id);
//				void RegisterEventCallback(EventCallbackF CallbackFunction);
//			};
//
//			interface IGlobalEvent : IEvent
//			{
//				const String& getName();
//
//				String Property(getName) Name;
//			};
//
//			interface IGlobalEventSource : IGlobalEvent {
//				void FireEvent(const Dynamic& AdditionalEventData);
//			};
//
//			class GlobalEventSourceHolder : protected IDisposable{
//			private:
//				struct data {
//					IGlobalEventSource* GlobalEventSource;
//					RefCountT RefCount;
//					data(IGlobalEventSource*&& GlobalEventSource)
//						:GlobalEventSource(GlobalEventSource), RefCount(1) 
//					{
//						GlobalEventSource = nullptr;
//					}
//				};
//			private:
//				data* This;
//			private:
//				GlobalEventSourceHolder(IGlobalEventSource*&& GlobalEventSource) :This(new data(MoveRValue(GlobalEventSource))) {}
//			protected:
//				virtual void Dispose() { UnRegisterEvent(this->Value); }
//			public:
//				RefCountClass(GlobalEventSourceHolder, This);
//				IGlobalEventSource& getValue() { return *This->GlobalEventSource; }
//				IGlobalEventSource& Property(getValue) Value;
//			};
//
//			class GlobalEvent: public IGlobalEventSource {
//			private:
//				List<EventCallbackF> CallbackList;
//				Map<TypeInfo, IEventCallback*> CallbackClassList;
//				String Name;
//				Boolean isAsync;
//			public:
//				virtual const UInt64 getEventCallbacksCount() override { return CallbackList.Count + CallbackClassList.Count; }
//				virtual const Boolean getIsAsync() override { return isAsync; }
//				virtual void RegisterEventCallback(IEventCallback*&& Callback, TypeInfo&& Id) override;
//				virtual void RegisterEventCallback(EventCallbackF CallbackFunction) override;
//				virtual const String& getName() override;
//				virtual void FireEvent(const Dynamic& AdditionalEventData) override;
//			};
//
//			void RegisterEventCallback(const String& EventName, IEventCallback*&& Callback, TypeInfo&& Id);
//			
//			IGlobalEventSource& RegisterEvent(const String& EventName, Boolean IsAsync);
//			IGlobalEventSource& GetEventSourceByInformation(String& EventName);
//			IGlobalEvent& GetEventInformationByName(const String& EventName);
//			void UnRegisterEvent(IGlobalEventSource& Event);
//		}
//	}
//}