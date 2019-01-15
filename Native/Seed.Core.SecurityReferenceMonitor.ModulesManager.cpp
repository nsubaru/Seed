#include "Seed.Core.Base.h"
#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

extern IMAGE_DOS_HEADER __ImageBase;

constexpr Seed::SizeT AllowedDllCout = 4;

const wchar_t* const AllowedDLL[AllowedDllCout] = {
	L"ntdll.dll", L"kernel32.dll", L"bcrypt.dll", L"bcryptprimitives.dll"
};

HANDLE ntdllHandle;

void * LdrDllNotificationHandleCokies;

NTSTATUS(NTAPI *pLdrRegisterDllNotification)(
	_In_     ULONG                          Flags,
	_In_     PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
	_In_opt_ PVOID                          Context,
	_Out_    PVOID                          *Cookie
	);

NTSTATUS(NTAPI *pLdrUnregisterDllNotification)(
	_In_ PVOID Cookie
	);

VOID CALLBACK LdrDllNotification(
	_In_     ULONG                       NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA  NotificationData,
	_In_opt_ PVOID                       Context
);

Seed::Boolean InitModuleFilter() {
	NTSTATUS status;
	UNICODE_STRING ntdll;
	ANSI_STRING func_name;
	RtlInitUnicodeStringEx(&ntdll, (PWSTR)AllowedDLL[0]);
	RtlInitAnsiStringEx(&func_name, (PCSZ)"LdrRegisterDllNotification");
	LdrGetDllHandle(NULL, NULL, &ntdll, &ntdllHandle);
	LdrGetProcedureAddress(ntdllHandle, &func_name, NULL, (void**)&pLdrRegisterDllNotification);
	status = pLdrRegisterDllNotification(0, LdrDllNotification, NULL, &LdrDllNotificationHandleCokies);
	return NT_SUCCESS(status);
}

void StopModuleFilter() noexcept {
	ANSI_STRING func_name;
	RtlInitAnsiStringEx(&func_name, (PCSZ)"LdrUnregisterDllNotification");
	LdrGetProcedureAddress(ntdllHandle, &func_name, NULL, (void**)&pLdrUnregisterDllNotification);
	pLdrUnregisterDllNotification(LdrDllNotificationHandleCokies);
}

VOID CALLBACK LdrDllNotification(
	_In_     ULONG                       NotificationReason,
	_In_     PLDR_DLL_NOTIFICATION_DATA  NotificationData,
	_In_opt_ PVOID                       Context
) {
	if (NotificationReason == LDR_DLL_NOTIFICATION_REASON_LOADED) {
		UNICODE_STRING str;
		for (Seed::SizeT i = 0; i < AllowedDllCout; i++) {
			RtlInitUnicodeString(&str, (PWSTR)AllowedDLL[i]);
			if (RtlCompareUnicodeString(NotificationData->Loaded.BaseDllName, &str, TRUE) == 0)
				return;
		}
		LdrUnloadDll(NotificationData->Loaded.DllBase);
		
		//TODO: Notify load module, security checks, additional verify
	}
	else {
		//TODO: Notifi unload module
	}
}