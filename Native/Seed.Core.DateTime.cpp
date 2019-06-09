// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.DateTime.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

void ConvertDateTimeToStringImpl(const Seed::Core::DateTime & time, Seed::CharStr buffer) {
	using namespace Seed;
	LARGE_INTEGER localTimeMs;
	TIME_FIELDS timeFields;
	RTL_TIME_ZONE_INFORMATION tzInfo;
	RtlQueryTimeZoneInformation(&tzInfo);
	localTimeMs.QuadPart = time.TimeFromStartEpoch;
	RtlTimeToTimeFields(&localTimeMs, &timeFields);
	buffer[3] = L'0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[2] = L'0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[1] = L'0' + timeFields.Year % 10; timeFields.Year /= 10;
	buffer[0] = L'0' + timeFields.Year % 10;
	buffer[4] = L'-';
	buffer[6] = L'0' + timeFields.Month % 10; timeFields.Month /= 10;
	buffer[5] = L'0' + timeFields.Month % 10;
	buffer[7] = L'-';
	buffer[9] = L'0' + timeFields.Day % 10; timeFields.Day /= 10;
	buffer[8] = L'0' + timeFields.Day % 10;
	buffer[10] = L' ';
	buffer[12] = L'0' + timeFields.Hour % 10; timeFields.Hour /= 10;
	buffer[11] = L'0' + timeFields.Hour % 10;
	buffer[13] = L':';
	buffer[15] = L'0' + timeFields.Minute % 10; timeFields.Minute /= 10;
	buffer[14] = L'0' + timeFields.Minute % 10;
	buffer[16] = L':';
	buffer[18] = L'0' + timeFields.Second % 10; timeFields.Second /= 10;
	buffer[17] = L'0' + timeFields.Second % 10;
	buffer[19] = L'.';
	buffer[22] = L'0' + timeFields.Milliseconds % 10; timeFields.Milliseconds /= 10;
	buffer[21] = L'0' + timeFields.Milliseconds % 10; timeFields.Milliseconds /= 10;
	buffer[20] = L'0' + timeFields.Milliseconds;
	buffer[21] = L'\0';
}

void ConvertDateToStringImpl(Seed::Core::Date date, Seed::Core::CharStr buffer) {
	buffer[3] = L'0' + date.Year % 10; date.Year /= 10;
	buffer[2] = L'0' + date.Year % 10; date.Year /= 10;
	buffer[1] = L'0' + date.Year % 10; date.Year /= 10;
	buffer[0] = L'0' + date.Year % 10;
	buffer[4] = L'-';
	buffer[6] = L'0' + date.Month % 10; date.Month /= 10;
	buffer[5] = L'0' + date.Month % 10;
	buffer[7] = L'-';
	buffer[9] = L'0' + date.Day % 10; date.Day /= 10;
	buffer[8] = L'0' + date.Day % 10;
	buffer[10] = L' ';
	buffer[12] = L'0' + date.Hour % 10; date.Hour /= 10;
	buffer[11] = L'0' + date.Hour % 10;
	buffer[13] = L':';
	buffer[15] = L'0' + date.Minutes % 10; date.Minutes /= 10;
	buffer[14] = L'0' + date.Minutes % 10;
	buffer[16] = L':';
	buffer[18] = L'0' + date.Second % 10; date.Second /= 10;
	buffer[17] = L'0' + date.Second % 10;
	buffer[19] = L'.';
	buffer[22] = L'0' + date.MiliSecond % 10; date.MiliSecond /= 10;
	buffer[21] = L'0' + date.MiliSecond % 10; date.MiliSecond /= 10;
	buffer[20] = L'0' + date.MiliSecond;
	buffer[21] = L'\0';
}