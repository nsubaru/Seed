#include "Seed.Core.SecurityReferenceMonitor.h"
#include "Seed.Core.SecurityReferenceMonitor.Internals.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

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

Seed::Boolean InitProcessSecurity() {
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

	return true;
}

//--------------------------------------------------------------------//
//Security Authority                                                  //
//--------------------------------------------------------------------//

__declspec(thread) Seed::Security::Token * PrimaryToken;
__declspec(thread) Seed::Security::Token * ImpersonatedToken;

Seed::Security::ISecurityAuthority * GlobalSecurityAuthority;

void Seed::Core::Security::TryRequestRight(UserRights right, const Char * const Method)
{
	if (ImpersonatedToken == nullptr) {
		if (PrimaryToken != nullptr) {
			PrimaryToken->ElevateRight(GlobalSecurityAuthority, right, Method);
		}
		else {
			return;
		}
	}
	else {
		ImpersonatedToken->ElevateRight(GlobalSecurityAuthority, right, Method);
	}
}

void Seed::Core::Security::RemoveRight(UserRights right)
{
	if (ImpersonatedToken == nullptr) {
		if (PrimaryToken != nullptr) {
			PrimaryToken->RemoveRight(GlobalSecurityAuthority, right);
		}
		else {
			return;
		}
	}
	else {
		ImpersonatedToken->RemoveRight(GlobalSecurityAuthority, right);
	}
}

Seed::Boolean Seed::Core::Security::RequireUserRight(
	UserRights right, 
	const AChar * const Method, 
	const Char * const Description)
{
	if (ImpersonatedToken == nullptr) {
		if (PrimaryToken != nullptr) {
			PrimaryToken->CheckRight(GlobalSecurityAuthority, right, Method, Description);
		}
		else {
			return;
		}
	}
	else {
		ImpersonatedToken->CheckRight(GlobalSecurityAuthority, right, Method, Description);
	}
}

void Seed::Core::Security::SecurableObject::AddACEToDescriptor(ACE & ace)
{
	(*(Security::SecurityDescriptor*)this->This->SecurityDescriptor) += ace;
}

void Seed::Core::Security::SecurableObject::RemoveACEFromDescriptor(SizeT Index)
{
	(*(Security::SecurityDescriptor*)this->This->SecurityDescriptor) -= Index;
}

const Seed::Core::Security::ACE & Seed::Core::Security::SecurableObject::GetACEbyIndex(SizeT Index)
{
	return (*(Security::SecurityDescriptor*)this->This->SecurityDescriptor)[Index];
}

Seed::SizeT Seed::Core::Security::SecurableObject::GetACECount()
{
	return (*(Security::SecurityDescriptor*)this->This->SecurityDescriptor).ACECount;
}

void Seed::Core::Security::SecurableObject::VerifyAcess(ACE_Right Right)
{

	if (ImpersonatedToken == nullptr) {
		if (PrimaryToken != nullptr) {
			if (!
				(*(Security::SecurityDescriptor*)this->This->SecurityDescriptor).VerifyAccess(PrimaryToken->GetUser(), Right)
				)
				return;
		}
		else {
			return;
		}
	}
	else {
		if (!
			(*(Security::SecurityDescriptor*)this->This->SecurityDescriptor).VerifyAccess(ImpersonatedToken->GetUser(), Right)
			)
			return;
	}
}

void Seed::Core::Security::SecurableObject::Dtor()
{
	delete (Security::SecurityDescriptor*)this->This->SecurityDescriptor;
}

Seed::Core::Security::SecurableObject::SecurableObject(UserIDType & OwnerID)
{
	This = new Data;
	This->SecurityDescriptor = new Security::SecurityDescriptor(OwnerID);
	This->RefCount = 1;
}

Seed::Core::Security::SecurableObject & Seed::Core::Security::SecurableObject::GetSecurityObjectDescriptor()
{
	Seed::Core::Security::RequireUserRight(UserRights::ReadSecurityDescriptor, __FUNCSIG__, L"Get security object descriptor");
	return *this;
}

Seed::Core::Security::SecurableObject::SecurableObject(const SecurableObject & Orig)
{
	Seed::Core::Security::RequireUserRight(UserRights::ReadSecurityDescriptor, __FUNCSIG__, L"Create copy of security object descriptor");
}
