#include "Seed.Core.Security.h"
#include "Seed.Core.Security.Manage.h"
#include "Seed.Core.Security.Internal.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)
#pragma warning(disable: 4595)

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

//Dll loading\unloading verification
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

Seed::Boolean DisableRemoteImage() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessImageLoadPolicy;
	policy.ImageLoadPolicy.Flags = 0;
	policy.ImageLoadPolicy.NoLowMandatoryLabelImages = 1;
	policy.ImageLoadPolicy.NoRemoteImages = 1;
	policy.ImageLoadPolicy.PreferSystem32Images = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean DisableWin32kSystemCalls() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessSystemCallDisablePolicy;
	policy.SystemCallDisablePolicy.Flags = 0;
	policy.SystemCallDisablePolicy.DisallowWin32kSystemCalls = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean StrictHandleCheks() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessStrictHandleCheckPolicy;
	policy.StrictHandleCheckPolicy.Flags = 0;
	policy.StrictHandleCheckPolicy.HandleExceptionsPermanentlyEnabled = 1;
	policy.StrictHandleCheckPolicy.RaiseExceptionOnInvalidHandleReference = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean DisableNonSystemFonts() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessFontDisablePolicy;
	policy.FontDisablePolicy.Flags = 0;
	policy.FontDisablePolicy.DisableNonSystemFonts = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean DisableDynamicCode() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessDynamicCodePolicy;
	policy.DynamicCodePolicy.Flags = 0;
	policy.DynamicCodePolicy.AllowRemoteDowngrade = 0;
	policy.DynamicCodePolicy.AuditProhibitDynamicCode = 0;
	policy.DynamicCodePolicy.AllowThreadOptOut = 0;
	policy.DynamicCodePolicy.ProhibitDynamicCode = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean DisableDllExtensionPolicy() {
	PROCESS_MITIGATION_POLICY_INFORMATION policy;
	policy.Policy = ProcessExtensionPointDisablePolicy;
	policy.ExtensionPointDisablePolicy.DisableExtensionPoints = 1;
	NTSTATUS status =
		NtSetInformationProcess(
			NtCurrentProcess(),
			ProcessMitigationPolicy,
			&policy,
			sizeof(PROCESS_MITIGATION_POLICY_INFORMATION)
		);
	return NT_SUCCESS(status);
}

Seed::Boolean Seed::Core::Init::Protection() noexcept
{
	if (!DisableWin32kSystemCalls())
		return false;
	if (!DisableNonSystemFonts())
		return false;
	if (!DisableDllExtensionPolicy())
		return false;
	if (!DisableRemoteImage())
		return false;
	if (!DisableDynamicCode())
		return false;

	if (!InitModuleFilter())
		return false;

	return true;
}

void Seed::Core::Stop::Protection() noexcept
{
	StopModuleFilter();
}

#pragma warning(pop)