#include "Seed.Core.CoreLog.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#include "Seed.Core.SettingsAPI.h"

#include "Seed.Core.SettingsAPI.Default.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

using namespace Seed;
using namespace Seed::Settings;

CurrentSettings::SettingsData CurrentSettingsData;

namespace Seed {
	inline namespace Core {
		Boolean isFirstStartup = false;

		namespace Settings {
			Handle SettingsFile;

			Char ExecutableDirectory[UNICODE_STRING_MAX_CHARS];
		}

		void CurrentSettings::SaveCurrentSettings()
		{
			IO_STATUS_BLOCK io_blk = { 0 };
			LARGE_INTEGER Offset = { 0 };
			NTSTATUS status = NtWriteFile(
				SettingsFile,
				NULL,
				NULL,
				NULL,
				&io_blk,
				(void*)& CurrentSettingsData, //-V2005
				sizeof(CurrentSettingsData),
				&Offset,
				NULL
			);
			if (!NT_SUCCESS(status)) {
				toCoreLog(L"Помилка: Не можливо зберегти налаштування ядра");
			}
		}
	}
}

Boolean Seed::Core::IsFirstStartup() noexcept
{
	return isFirstStartup;
}

Boolean Seed::Core::Init::SettingsAPI() noexcept
{
	{
		Char raw[sizeof(size_t) + 1024];
		UNICODE_STRING* str = (UNICODE_STRING*)raw; //-V2005
		str->Length = 0;
		str->MaximumLength = 0;
		NtQueryInformationProcess(
			NtCurrentProcess(),
			PROCESSINFOCLASS::ProcessImageFileName,
			str,
			(sizeof(size_t) + 1024) * 2,
			NULL
		);
		SizeT last_dir = 0;
		for (SizeT i = 0; i < str->Length; i++) {
			if (str->Buffer[i] == L'\\')
				last_dir = i;
		}
		str->Length = (USHORT)(last_dir + 1); //-V2005
		str->Buffer[str->Length] = '\0';
		CStr::Copy(str->Buffer, Settings::ExecutableDirectory);
		CharStr buf = Settings::ExecutableDirectory;
		while (*buf++ = *str->Buffer++);
	}
	//Init setting file
	{
		toCoreLog(L"Ініціалізація файла конфігурацій...");
		Char raw[sizeof(size_t) + 1024];
		UNICODE_STRING* str = (UNICODE_STRING*)raw; //-V2005
		NtQueryInformationProcess(
			NtCurrentProcess(),
			PROCESSINFOCLASS::ProcessImageFileName,
			str,
			(sizeof(size_t) + 1024) * 2,
			NULL
		);
		Char FullSettingsFilePath[1024];
		CharStr buf = Settings::ExecutableDirectory;
		CStr::Copy(str->Buffer, FullSettingsFilePath);
		SizeT ExeFileNameSize = CStr::Size(str->Buffer);
		SizeT FileNameSize = CStr::Size(Settings::SettingsFileName);
		FullSettingsFilePath[ExeFileNameSize] = L':';
		for (SizeT i = 0; i < FileNameSize + 1; i++) {
			FullSettingsFilePath[ExeFileNameSize + i + 1] = Settings::SettingsFileName[i];
		}
		UNICODE_STRING FileNameStr;
		RtlInitUnicodeStringEx(&FileNameStr, FullSettingsFilePath);
		OBJECT_ATTRIBUTES attrb;
		attrb.ObjectName = &FileNameStr;
		attrb.Length = sizeof(OBJECT_ATTRIBUTES);
		attrb.RootDirectory = NULL;
		attrb.SecurityDescriptor = NULL;
		attrb.SecurityQualityOfService = NULL;
		attrb.Attributes = OBJ_CASE_INSENSITIVE;
		IO_STATUS_BLOCK io_blk = { 0 };
		NTSTATUS status = NtCreateFile(
			&SettingsFile,
			GENERIC_ALL | GENERIC_WRITE | SYNCHRONIZE,
			&attrb,
			&io_blk,
			NULL,
			FILE_ATTRIBUTE_NORMAL,
			FILE_SHARE_READ,
			FILE_CREATE,
			FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
			NULL,
			NULL);
		if (status == STATUS_OBJECT_NAME_COLLISION) {
			status = NtCreateFile(
				&SettingsFile,
				GENERIC_ALL | GENERIC_WRITE | SYNCHRONIZE,
				&attrb,
				&io_blk,
				NULL,
				FILE_ATTRIBUTE_NORMAL,
				FILE_SHARE_READ,
				FILE_OPEN,
				FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
				NULL,
				NULL);
			if (!NT_SUCCESS(status)) return false;
			status = NtReadFile(
				SettingsFile,
				NULL,
				NULL,
				NULL,
				&io_blk,
				(void*)& CurrentSettingsData, //-V2005
				sizeof(CurrentSettingsData),
				NULL,
				NULL
			);
		}
		else if (!NT_SUCCESS(status)) {
			return false;
		}
		else {
			toCoreLog(L"Перший запуск. Використання стандартних налаштуваннь ядра");
			CurrentSettingsData = Settings::Default::InitDefaultSettings();
			isFirstStartup = true;
			status = NtWriteFile(
				SettingsFile,
				NULL,
				NULL,
				NULL,
				&io_blk,
				(void*)& CurrentSettingsData, //-V2005
				sizeof(CurrentSettingsData),
				NULL,
				NULL
			);
		}
		if (!NT_SUCCESS(status)) return false;
	}
	return true;
}

void Seed::Core::Stop::SettingsAPI() noexcept
{
	CurrentSettings::SaveCurrentSettings();
	NtClose(SettingsFile);
}

#pragma warning(pop)