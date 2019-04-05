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
//Seed::Core::Thread::Event::Event(Seed::Boolean IsManualReset) noexcept : data(new event_data(String()))
//{
//	NTSTATUS status = NtCreateEvent(
//		&data->Handle, 
//		GENERIC_ALL, 
//		NULL, 
//		(IsManualReset) ? EVENT_TYPE::SynchronizationEvent : EVENT_TYPE::NotificationEvent, 
//		FALSE
//	);
//
//	if (status == STATUS_INSUFFICIENT_RESOURCES)
//		Throw(OutOfMemory(ExceptionPosition));
//
//	if (NT_SUCCESS(status))
//
//}
//
//#pragma warning(pop)
//
//
