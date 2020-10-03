#include "Seed.Core.BTLDR.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

namespace Seed {
	inline namespace Core {
		namespace Btlrd {

			void* GetImageBase() {
				return (void*)&__ImageBase;
			}

			template<typename TFunction>
			TFunction* GetNtProcedureByName(HANDLE ntdll, TLdrGetProcedureAddress* LdrGetProcedureAddress, Seed::ConstACharStr functionName) {
				ANSI_STRING fName = { 0 };
				RtlInitAnsiString(&fName, (PSTR)functionName);
				TFunction* function = nullptr;
				LdrGetProcedureAddress(ntdll, &fName, 0, (PVOID*)&function);
				return function;
			}

			using TUpdateBtldrDataFunction = decltype(BtlrdData::UpdateDataOnReload);

			__declspec(noreturn, noinline) void BtldInPlaceUpdateInitilizer(BtlrdInPlaceUpdateData* newBtldrDataAddress) {
				HANDLE btlrdThreadHandle = nullptr;
				CLIENT_ID id = {};

				RtlCreateUserThread(
					NtCurrentProcess(),
					NULL,
					FALSE,
					0,
					Seed::Core::Settings::MainThreadStackMaxSize,
					Seed::Core::Settings::MainThreadStackStartSize,
					(PUSER_THREAD_START_ROUTINE) newBtldrDataAddress->InPlaceUpdataEntryPoint,
					newBtldrDataAddress,
					&btlrdThreadHandle,
					&id);

				NtClose(btlrdThreadHandle);

				NtTerminateThread(NtCurrentThread(), STATUS_SUCCESS);
			}

			__declspec(noreturn, noinline) void BtldInPlaceUpdateFinalizer(BtlrdInPlaceUpdateData* newBtldrDataAddress) {
				HANDLE btlrdThreadHandle = nullptr;
				CLIENT_ID id = {};

				RtlCreateUserThread(
					NtCurrentProcess(),
					NULL,
					FALSE,
					0,
					Seed::Core::Settings::MainThreadStackMaxSize,
					Seed::Core::Settings::MainThreadStackStartSize,
					(PUSER_THREAD_START_ROUTINE)newBtldrDataAddress->InPlaceUpdataEntryPoint,
					newBtldrDataAddress,
					&btlrdThreadHandle,
					&id);

				NtClose(btlrdThreadHandle);

				NtTerminateThread(NtCurrentThread(), STATUS_SUCCESS);
			}

			__declspec(noreturn, noinline) void BtldEntryPoint(HANDLE ntdll, void* vLdrGetProcedureAddress) {
				TLdrGetProcedureAddress* LdrGetProcedureAddress = (TLdrGetProcedureAddress*)vLdrGetProcedureAddress;

				BtlrdData initData = BtlrdData(
					GetImageBase(),
					&BtldEntryPoint,
					GetNtProcedureByName<TNtQuerySystemTime>(ntdll, LdrGetProcedureAddress, "NtQuerySystemTime")
				);

				BtlrdInPlaceUpdateData* newBtldrDataAddress = nullptr;

				do {
					newBtldrDataAddress = Main((void*)&initData, &BtlrdData::UpdateDataOnReload, initData.ReloadCount == 0);
				}
				while (initData.FastReboot == true && newBtldrDataAddress == nullptr);
				

				if (newBtldrDataAddress != nullptr) {
					newBtldrDataAddress->ldrLoadFunction = (Seed::UInt64)LdrGetProcedureAddress;
					newBtldrDataAddress->ntdllHandle = (Seed::UInt64)ntdll;
					newBtldrDataAddress->oldImageBase = (Seed::UInt64)GetImageBase();
					BtldInPlaceUpdateInitilizer(newBtldrDataAddress);
				}
				
				GetNtProcedureByName<TNtTerminateProcess>(ntdll, LdrGetProcedureAddress, "NtTerminateProcess")(NtCurrentProcess(), 0);
			}

			__declspec(noreturn, noinline, dllexport) void BtldEntryPoint2(HANDLE ntdll, TLdrGetProcedureAddress LdrGetProcedureAddress, BtlrdInPlaceUpdateData* btlrdInPlaceUpdateData) {
#ifndef ReleaseMode
				NtSuspendProcess(NtCurrentProcess());
#endif

				BtlrdData initData = BtlrdData(
					GetImageBase(),
					&BtldEntryPoint,
					GetNtProcedureByName<TNtQuerySystemTime>(ntdll, LdrGetProcedureAddress, "NtQuerySystemTime")
				);

				initData.ReloadCount = btlrdInPlaceUpdateData->ReloadCount + 1;
				initData.ProcessInitTime = btlrdInPlaceUpdateData->ProcessInitTime;
				void* inPlaceUpdateSection = (void*)btlrdInPlaceUpdateData->SectionAddress;

				ZwFreeVirtualMemory(NtCurrentProcess(), &inPlaceUpdateSection, NULL, MEM_RELEASE);
				RtlFreeHeap(RtlProcessHeap(), 0, btlrdInPlaceUpdateData);

				BtlrdInPlaceUpdateData* newBtldrDataAddress = nullptr;
				do {
					newBtldrDataAddress = Main((void*)&initData, &BtlrdData::UpdateDataOnReload, initData.ReloadCount == 0);
				} while (initData.FastReboot == true && newBtldrDataAddress == nullptr);

				if (newBtldrDataAddress != nullptr) {
					newBtldrDataAddress->ldrLoadFunction = (Seed::UInt64)LdrGetProcedureAddress;
					newBtldrDataAddress->ntdllHandle = (Seed::UInt64)ntdll;
					newBtldrDataAddress->oldImageBase = (Seed::UInt64)GetImageBase();
					BtldInPlaceUpdateInitilizer(newBtldrDataAddress);
				}

				GetNtProcedureByName<TNtTerminateProcess>(ntdll, LdrGetProcedureAddress, "NtTerminateProcess")(NtCurrentProcess(), 0);
			}

			__declspec(noinline) extern "C" NTSTATUS NTAPI EntryPoint2(_In_ PVOID ThreadParameter) {
				BtlrdInPlaceUpdateData* btlrdInPlaceUpdateData = (BtlrdInPlaceUpdateData*)ThreadParameter;
				UNICODE_STRING str = { 0 };
				RtlInitUnicodeString(&str, L"ntdll.dll");
				HANDLE ntdll = nullptr;
				LdrLoadDll(NULL, NULL, &str, &ntdll);
				Seed::Core::Btlrd::BtldEntryPoint2(ntdll, &LdrGetProcedureAddress, btlrdInPlaceUpdateData);

				return STATUS_SUCCESS;
			}

			void my_memmove(void* dst, const void* src, unsigned int len)
			{
				if ((uintptr_t)dst % sizeof(long) == 0 &&
					(uintptr_t)src % sizeof(long) == 0 &&
					len % sizeof(long) == 0) {
					char* d = (char*)dst;
					const char* s = (const char* )src;

					/*
					 * The reason we copy index i-1 and test i>0 is that
					 * i is unsigned -- so testing i>=0 doesn't work.
					 */

					for (unsigned int i = len; i > 0; i--) {
						d[i - 1] = s[i - 1];

					}
				}
				else {
					char* d = (char*)dst;
					const char* s = (const char*)src;

					for (unsigned int i = len; i > 0; i--) {
						d[i - 1] = s[i - 1];
					}
				}
			}

			__declspec(noinline) extern "C" NTSTATUS NTAPI BtldInPlaceUpdateTransitionProcess(
				_In_ PVOID ThreadParameter
				) {
				//Load information
				BtlrdInPlaceUpdateData* data = (BtlrdInPlaceUpdateData*)ThreadParameter;

				//init buffer for old exe file name
				char buffer[Seed::Math::Max<UInt16>() + sizeof(UNICODE_STRING)] = { 0 };
				PUNICODE_STRING str = (PUNICODE_STRING)buffer;
				str->Buffer = (PWCHAR)(buffer + sizeof(UNICODE_STRING));
				str->MaximumLength = Seed::Math::Max<UInt16>();
				str->Length = 0;

				//query file path by image base
				NTSTATUS status = GetNtProcedureByName<decltype(NtQueryInformationProcess)>(
					(HANDLE)data->ntdllHandle, 
					(TLdrGetProcedureAddress*)data->ldrLoadFunction, 
					"NtQueryInformationProcess"
				)(
					NtCurrentProcess(),
					PROCESSINFOCLASS::ProcessImageFileName,
					str,
					Seed::Math::Max<UInt16>() + sizeof(UNICODE_STRING),
					NULL
				);

				//unload image from memory
				GetNtProcedureByName<decltype(LdrUnloadDll)>(
					(HANDLE)data->ntdllHandle,
					(TLdrGetProcedureAddress*)data->ldrLoadFunction,
					"LdrUnloadDll"
					)((PVOID)data->oldImageBase);
				//if query non success then return
				if (!NT_SUCCESS(status))
					return status;

				//init variables for deleting old executable file
				HANDLE hOldExe = nullptr;
				OBJECT_ATTRIBUTES attrbs = { 0 };
				InitializeObjectAttributes(&attrbs, str, NULL, NULL, NULL);
				IO_STATUS_BLOCK ioBlk = { 0 };

				//open old executable file with marking for deleing
				GetNtProcedureByName<decltype(NtOpenFile)>(
					(HANDLE)data->ntdllHandle,
					(TLdrGetProcedureAddress*)data->ldrLoadFunction,
					"NtOpenFile"
				)(
					&hOldExe, 
					DELETE, 
					&attrbs, 
					&ioBlk, 
					FILE_SHARE_DELETE, 
					FILE_DELETE_ON_CLOSE
					);

				//if open succes than close file for deleting
				if (hOldExe != nullptr) {
					GetNtProcedureByName<decltype(NtClose)>(
						(HANDLE)data->ntdllHandle,
						(TLdrGetProcedureAddress*)data->ldrLoadFunction,
						"NtClose"
					)(&hOldExe);
					return status;
				}
				//create move information for moving updated exe to orign exe file
				auto len = sizeof(FILE_RENAME_INFORMATION);
				PFILE_RENAME_INFORMATION information = (PFILE_RENAME_INFORMATION)buffer;
				{
					auto sz = str->Length;
					my_memmove(information->FileName, str->Buffer, str->Length);
					information->FileNameLength = sz;
					information->RootDirectory = nullptr;
					information->ReplaceIfExists = FALSE;
					len += sz;
				}

				//move updated exe file to origin exe file
				status = GetNtProcedureByName<decltype(NtSetInformationFile)>(
					(HANDLE)data->ntdllHandle,
					(TLdrGetProcedureAddress*)data->ldrLoadFunction,
					"NtSetInformationFile"
				)(
					data->NewKernelFileHandle, 
					&ioBlk, 
					information, 
					(ULONG)len, 
					FileRenameInformation
				);

				if (!NT_SUCCESS(status))
					return status;

				UNICODE_STRING exeName = { 0 };

				GetNtProcedureByName<decltype(RtlInitUnicodeString)>(
					(HANDLE)data->ntdllHandle,
					(TLdrGetProcedureAddress*)data->ldrLoadFunction,
					"RtlInitUnicodeString"
					)
				(&exeName, information->FileName);
				//Get new image base
				void* newImageBase = nullptr;
				GetNtProcedureByName<decltype(LdrLoadDll)>(
					(HANDLE)data->ntdllHandle,
					(TLdrGetProcedureAddress*)data->ldrLoadFunction,
					"LdrLoadDll"
					)
				(NULL, NULL, &exeName, &newImageBase);
				//get new image entry point
				data->InPlaceUpdataEntryPoint =
					(UInt64)GetNtProcedureByName<decltype(BtldEntryPoint2)>(
						(HANDLE)newImageBase,
						(TLdrGetProcedureAddress*)data->ldrLoadFunction,
						"BtldEntryPoint2"
						);
				//goto finalizer
				BtldInPlaceUpdateFinalizer(data);
			}
		}
	}
}

struct TLogFile {
	HANDLE handle = nullptr;
	IO_STATUS_BLOCK ioBlock = { 0 };
	Seed::Threading::SRWLock lock = Seed::Threading::SRWLock();
};

Seed::Core::Btlrd::BtlrdInPlaceUpdateData* Main(void* initData, void(*UpdateInitData)(void*), bool isFirstboot) {
	TLogFile logFileStructData = TLogFile();
	Seed::Core::Stage1(&logFileStructData, initData, isFirstboot);
	UpdateInitData(initData);
	return nullptr;
}

extern void*& InitData();

Seed::UInt64 Seed::Core::Btlrd::GetReloadsCount()
{
	return ((Seed::Core::Btlrd::BtlrdData*)InitData())->ReloadCount;
}

Seed::UInt64 Seed::Core::Btlrd::GetProcessInitTime()
{
	return ((Seed::Core::Btlrd::BtlrdData*)InitData())->ProcessInitTime;
}

Seed::UInt64 Seed::Core::Btlrd::GetLastReloadTime()
{
	return ((Seed::Core::Btlrd::BtlrdData*)InitData())->LastReloadTime;
}

void Seed::Core::Btlrd::SetFastReboot()
{
	((Seed::Core::Btlrd::BtlrdData*)InitData())->FastReboot = true;
}
