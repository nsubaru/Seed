//#include "Seed.Core.Threading.h"
//
//#include "Seed.Core.Init.h"
//#include "Seed.Core.Stop.h"
//
//#include "Seed.Core.SettingsAPI.h"
//
//#include "Seed.Core.CoreLog.h"
//
//#define SEED_RT_ALLOW_INTERNAL_NT_API
//#include "Seed.Core.Base.NTAPI.h"
//
//#pragma warning(push)
//#pragma warning(disable: 4996)
//
//Seed::Core::Thread::GuardLock::GuardLock() noexcept
//{
//	Handle event;
//	NtCreateEvent(&event, GENERIC_ALL, NULL, EVENT_TYPE::SynchronizationEvent, TRUE);
//	RTL_CRITICAL_SECTION Section = { 0 };
//	RtlInitializeCriticalSectionAndSpinCount(reinterpret_cast<::RTL_CRITICAL_SECTION*>(&Section), 5000);
//	data = new guardlock_data(event, Section);
//}
//
//Seed::Core::Thread::Event::Event(Seed::Boolean IsManualReset) noexcept
//{
//	void * handle;
//	NtCreateEvent(
//		&handle,
//		GENERIC_ALL,
//		NULL,
//		(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
//		FALSE);
//	this->data = new event_data(handle, L"");
//	this->data->RefCount++;
//}
//
//Seed::Core::Thread::Event::Event(const Seed::String & name, Seed::Boolean IsManualReset) noexcept
//{
//	OBJECT_ATTRIBUTES atrrbs;
//	UNICODE_STRING str;
//	RtlInitUnicodeStringEx(&str, (CharStr)name);
//	InitializeObjectAttributes(&atrrbs, &str, NULL, NULL, NULL);
//	void * handle;
//	NtCreateEvent(
//		&handle,
//		GENERIC_ALL,
//		NULL,
//		(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
//		FALSE);
//	this->data = new event_data(handle, name);
//	this->data->RefCount++;
//}
//
//Seed::Core::Thread::Event::Event(Seed::String && name, Seed::Boolean IsManualReset) noexcept
//{
//	OBJECT_ATTRIBUTES atrrbs;
//	UNICODE_STRING str;
//	RtlInitUnicodeStringEx(&str, (CharStr)name);
//	InitializeObjectAttributes(&atrrbs, &str, NULL, NULL, NULL);
//	void * handle;
//	NtCreateEvent(
//		&handle,
//		GENERIC_ALL,
//		NULL,
//		(IsManualReset) ? EVENT_TYPE::NotificationEvent : EVENT_TYPE::SynchronizationEvent,
//		FALSE);
//	this->data = new event_data(handle, name);
//	this->data->RefCount++;
//}
//
//#pragma region Event
//
//void Seed::Core::Thread::Event::Wait () {
//	NtWaitForSingleObject(this->data->Handle, TRUE, NULL);
//}
//
//void Seed::Core::Thread::Event::Wait(Seed::SizeT TimeOutMs) {
//
//}
//
//void Seed::Core::Thread::Event::Signale() {}
//
//void Seed::Core::Thread::Event::Set() {}
//
//void Seed::Core::Thread::Event::Reset() {}
//
//Seed::SizeT Seed::Core::Thread::Event::EventObjCount() {
//	return SizeT ();
//}
//
//#pragma endregion
//
//#pragma region Critical section
//
//void Seed::Core::Thread::GuardLock::Dispose() {
//	RtlDeleteCriticalSection(reinterpret_cast<::PRTL_CRITICAL_SECTION>(&data->CriticalSection));
//	NtClose(data->EventHandle);
//}
//
//#pragma endregion
//
//#pragma warning(pop)