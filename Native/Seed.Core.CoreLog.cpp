// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include "Seed.Core.Threading.LockPrimitives.h"

#include "Seed.Core.CoreLog.h"
#include "Seed.Core.CoreLog.Managed.h"

#include "Seed.Core.Init.h"
#include "Seed.Core.Stop.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)
//toCoreLog always have valid text to input as it's internal API call
//So we ignore warning about posible realeasing unheld lock as in lock only syscall NtWriteFile
#pragma warning(disable: 26117)
//toCoreLog always have valid text to input as it's internal API call as in lock only syscall NtWriteFile
//So we ignore warning about posible falling realising lock
#pragma warning(disable: 26115)
//toCoreLog store log msg in stack because logging start on very early startup steps and on that steps memory API not awailable
//so we must use stack as storage for msg
#pragma warning(disable: 6262)

struct TLogFile {
	HANDLE handle = nullptr;
	IO_STATUS_BLOCK ioBlock = { 0 };
	Seed::Threading::SRWLock lock = Seed::Threading::SRWLock();
};

//Статична функція отримання керуючої структури логування
static TLogFile& GetLogFile() {
	static TLogFile LogFileStructData;
	return LogFileStructData;
}
namespace Seed
{
	inline namespace Core
	{
		extern void SendLogMsgToEvent(const Seed::Char* Value, Seed::SizeT Lenght);
	}
}


//Дефайн отримання керуючої структури логування
#define LogFile GetLogFile()

void FormatCurrentTime(Seed::Char* buffer) {
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
	UNICODE_STRING* str = (UNICODE_STRING*)raw; //-V2005 //-V1032 //-V2533
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
		GENERIC_ALL | SYNCHRONIZE,
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
		(void*)UTF16_MARKER, //-V2005 //-V2533
		sizeof(char) * 2,
		NULL,
		NULL);
	if (!NT_SUCCESS(status)) return false;
	return Seed::Core::toCoreLog(L"Запуск ядра...");
}

SeedKernelCall InternalApiCall Seed::Boolean Seed::Core::toCoreLog(const Seed::Char* Value, Seed::SizeT Lenght) {
	if (Lenght > static_cast<SizeT>(Math::Max<Int16>()) - 22ULL)
		return false;
	Char raw[Math::Max<Int16>()] = { 0 };
	FormatCurrentTime(raw);
	raw[21] = L' ';
	CStr::Copy(Value, raw + 22);
	SizeT size = CStr::Size(raw);
	CStr::Copy(L"\r\n", raw + size);
	NTSTATUS status = STATUS_SUCCESS;
	Seed::Core::SendLogMsgToEvent(raw, size);
	size += 2; size *= sizeof(wchar_t);
	LogFile.lock.ExclusiveLock([&] ()
	{
		status = NtWriteFile( //-V107
			LogFile.handle,
			NULL,
			NULL,
			NULL,
			&LogFile.ioBlock,
			(void*) raw, //-V2005 //-V2533
			(ULONG) size, //-V2005 //-V202 //-V2533
			NULL,
			NULL);
	});
	if (!NT_SUCCESS(status)) return false;
	else return true;
}

void Seed::Core::Stop::Loging() noexcept
{
	toCoreLog(L"Зупинка логування...");
	NtClose(LogFile.handle);
}

bool SeedKernelCall Seed::Core::toCoreLog(const Seed::Containers::Array<Seed::ExceptionString>& lines)
{
	if (lines.Count == 0) return true;
	try
	{
		SizeT worldCount = 0;
		for (SizeT i = 0; i < lines.Count; i++)
			worldCount += lines[i].Lenght;
		ExceptionString data(worldCount);
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
			data = ExceptionString(raw) + data;
			
			Seed::Core::SendLogMsgToEvent((Seed::ConstCharStr) data, data.Lenght);

			data +=  NextLine();
			SizeT bytesCount = data.Lenght;
			bytesCount *= sizeof(wchar_t);

			NTSTATUS status = STATUS_SUCCESS;
			LogFile.lock.ExclusiveLock([&] ()
			{
				status = NtWriteFile( //-V107
					LogFile.handle,
					NULL,
					NULL,
					NULL,
					&LogFile.ioBlock,
					(void*) (Seed::ConstCharStr) data, //-V2005 //-V2533
					(ULONG) bytesCount, //-V2005 //-V202 //-V2533
					NULL,
					NULL);
			});
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

#include "Seed.Core.Services.CoreLogger.h"

Seed::Containers::List<Seed::String> Seed::Services::CoreLogger::ReadLog() {
	Char* buffer = nullptr;
	try
	{
		NTSTATUS status = STATUS_SUCCESS;

		LogFile.lock.ShareLock([&] ()
		{
			FILE_STANDARD_INFORMATION logFileInfo;
			status = NtQueryInformationFile(
				LogFile.handle,
				&LogFile.ioBlock,
				&logFileInfo,
				sizeof(FILE_STANDARD_INFORMATION),
				FileStandardInformation
				);
			if (!NT_SUCCESS(status))
			{
				Throw(NtException(ExceptionPosition, status));
			}
			SizeT bufferSize = logFileInfo.EndOfFile.QuadPart;
			buffer = new Char[bufferSize];
			Memory::MemBlkFill(buffer, bufferSize, 0);
			LARGE_INTEGER offset = { 0 };
			offset.QuadPart = sizeof(char) * 2;

			status = NtReadFile(LogFile.handle, NULL, NULL, NULL, &LogFile.ioBlock, buffer, (ULONG) bufferSize, &offset, NULL);
		});

		if (!NT_SUCCESS(status))
		{
			Throw(NtException(ExceptionPosition, status));
		}
		SizeT textSize = 0;
		auto ptr = buffer;
		while (*ptr != 0)
		{
			textSize++;
			ptr++;
		}
		textSize++;
		auto ret = String(buffer, textSize).Split(L"\r\n");
		if(ret.LastIt != nullptr)
			ret.Remove(ret.LastIt);
		delete[] buffer;
		return ret;
	}
	catch (...)
	{
		delete[] buffer;
		ReThrow;
	}
}

//#pragma comment(linker, "/include:_tls_used")
//#pragma section("INIT$XLB", read)
//
//void NTAPI TLSCallback(PVOID DllHandle, DWORD Reason, PVOID Reserved)
//{
//	UNREFERENCED_PARAMETER(Reserved);
//
//	if (LogFile.handle != nullptr)
//	{
//		Seed::toCoreLog(L"TLS callback called");
//	}
//}
//
//extern "C" __declspec(allocate("INIT$XLB")) PIMAGE_TLS_CALLBACK g_pAddressOfTLSCallback = TLSCallback;
//#pragma comment(linker, "/include:g_pAddressOfTLSCallback")
//
//#pragma warning(pop)