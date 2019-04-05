#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

struct {
	HANDLE handle;
	IO_STATUS_BLOCK ioBlock;
	RTL_CRITICAL_SECTION lock;
} LogFile;

void FormatCurrentTime(Seed::Char * buffer) {
	using namespace Seed;
	LARGE_INTEGER timeMs, localTimeMs;
	TIME_FIELDS timeFields;
	NtQuerySystemTime(&timeMs);
	RTL_TIME_ZONE_INFORMATION tzInfo;
	RtlQueryTimeZoneInformation(&tzInfo);
	localTimeMs.QuadPart = timeMs.QuadPart - (tzInfo.Bias * 600000000LL);
	RtlTimeToTimeFields(&localTimeMs, &timeFields);
	buffer[3] = '0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[2] = '0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[1] = '0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[0] = '0' + timeFields.Year % 10;
	buffer[4] = '-';
	buffer[6] = '0' + timeFields.Month % 10; timeFields.Month /= 10;
	buffer[5] = '0' + timeFields.Month % 10;
	buffer[7] = '-';
	buffer[9] = '0' + timeFields.Day % 10; timeFields.Day /= 10;
	buffer[8] = '0' + timeFields.Day % 10;
	buffer[10] = ' ';
	buffer[12] = '0' + timeFields.Hour % 10; timeFields.Hour /= 10;
	buffer[11] = '0' + timeFields.Hour % 10;
	buffer[13] = '-';
	buffer[15] = '0' + timeFields.Minute % 10; timeFields.Minute /= 10;
	buffer[14] = '0' + timeFields.Minute % 10;
	buffer[16] = '-';
	buffer[18] = '0' + timeFields.Second % 10; timeFields.Second /= 10;
	buffer[17] = '0' + timeFields.Second % 10;
	buffer[19] = '.';
	buffer[22] = '0' + timeFields.Milliseconds % 10; timeFields.Milliseconds /= 10;
	buffer[21] = '0' + timeFields.Milliseconds % 10; timeFields.Milliseconds /= 10;
	buffer[20] = '0' + timeFields.Milliseconds;
}

Seed::Boolean Seed::Core::Init::Loging() noexcept
{
	BOOLEAN state;
	RtlAdjustPrivilege(SE_TIME_ZONE_PRIVILEGE, TRUE, FALSE, &state);
	Char raw[sizeof(size_t) + 1024];
	UNICODE_STRING * str = (UNICODE_STRING*)raw; //-V2005
	UNICODE_STRING name;
	{
		str->Length = 0;
		str->MaximumLength = 0;
		NtQueryInformationProcess(NtCurrentProcess(), PROCESSINFOCLASS::ProcessImageFileName, str, (sizeof(size_t) + 1024) * 2, NULL);
		SizeT last_dir = 0;
		for (SizeT i = 0; i < str->Length; i++) {
			if (str->Buffer[i] == L'\\')
				last_dir = i;
		}
		Char raw[1000];
		FormatCurrentTime(raw);
		CStr::Copy(L" Seed.CoreLog.log", raw + 23);
		CStr::Copy(raw, str->Buffer + last_dir + 1);
		RtlInitUnicodeStringEx(&name, str->Buffer);
	}
	OBJECT_ATTRIBUTES attrb;
	attrb.ObjectName = &name;
	attrb.Length = sizeof(OBJECT_ATTRIBUTES);
	attrb.RootDirectory = NULL;
	attrb.SecurityDescriptor = NULL;
	attrb.SecurityQualityOfService = NULL;
	attrb.Attributes = OBJ_CASE_INSENSITIVE;
	NTSTATUS status = NtCreateFile(
		&LogFile.handle,
		GENERIC_WRITE | SYNCHRONIZE,
		&attrb,
		&LogFile.ioBlock,
		NULL,
		FILE_ATTRIBUTE_NORMAL,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_CREATE,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_WRITE_THROUGH,
		NULL,
		NULL);
	if (!NT_SUCCESS(status)) return false;
	constexpr unsigned char UTF16_MARKER[2] = { 0xFF,0xFE };
	status = NtWriteFile(
		LogFile.handle,
		NULL,
		NULL,
		NULL,
		&LogFile.ioBlock,
		(void*)UTF16_MARKER, //-V2005
		sizeof(char) * 2,
		NULL,
		NULL);
	RtlInitializeCriticalSection(&LogFile.lock);
	if (!NT_SUCCESS(status)) return false;
	return Seed::Core::toCoreLog(L"Starting core...");
}

SeedKernelCall InternalApiCall Seed::Boolean Seed::Core::toCoreLog(const Seed::Char * Value, Seed::SizeT Lenght)
{
	if (Lenght > Math::Max<Int16>() - 22)
		return false;
	Char raw[Math::Max<Int16>()];
	FormatCurrentTime(raw);
	raw[21] = L' ';
	CStr::Copy(Value, raw + 22);
	SizeT size = CStr::Size(raw);
	CStr::Copy(L"\r\n", raw + size);
	size += 2; size *= sizeof(wchar_t);
	RtlEnterCriticalSection(&LogFile.lock);
	NTSTATUS status = NtWriteFile( //-V107
		LogFile.handle,
		NULL,
		NULL,
		NULL,
		&LogFile.ioBlock,
		(void*)raw, //-V2005
		(ULONG)size, //-V2005 //-V202
		NULL,
		NULL);
	RtlLeaveCriticalSection(&LogFile.lock);
	if (!NT_SUCCESS(status)) return false;
	else return true;
}

void Seed::Core::Stop::Loging() noexcept
{
	toCoreLog(L"Зупинка логування...");
	NtClose(LogFile.handle);
	RtlDeleteCriticalSection(&LogFile.lock);
}

bool SeedKernelCall Seed::Core::toCoreLog(const Seed::Array<Seed::String>& lines)
{
	if (lines.Count == 0) return true;
	try
	{
		SizeT worldCount = 0;
		for (SizeT i = 0; i < lines.Count; i++)
			worldCount += lines[i].Size;
		String data(worldCount);
		data += lines[0];
		if (lines.Count > 1)
		{
			for (SizeT i = 1; i < lines.Count; i++)
			{
				data += String(L"\r\n                      ") + lines[i];
			}
		}
		{
			Char raw[23];
			FormatCurrentTime(raw);
			raw[21] = L' ';
			raw[22] = L'\0';
			data = String(raw) + data + NextLine();
			SizeT bytesCount = data.Size;
			bytesCount *= sizeof(wchar_t);
			RtlEnterCriticalSection(&LogFile.lock);
			NTSTATUS status = NtWriteFile( //-V107
				LogFile.handle,
				NULL,
				NULL,
				NULL,
				&LogFile.ioBlock,
				(void*)(Seed::ConstCharStr) data, //-V2005
				(ULONG)bytesCount, //-V2005 //-V202
				NULL,
				NULL);
			RtlLeaveCriticalSection(&LogFile.lock);
			if (!NT_SUCCESS(status)) return false;
			else return true;
		}
	}
	catch (Seed::Exception & ex)
	{
		toCoreLog(L"Internal coreLog exeption: ");
		toCoreLog(ex.Name.Str, ex.Name.Size);
		toCoreLog(L"Passed data(only first line): ");
		toCoreLog(lines[0]);
		return false;
	}
	return true;
}

#pragma warning(pop)