// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#pragma once
#include "Seed.Core.Base.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

InternalApiCall
OptimizeExport(
	void UppercaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to),
	{
		UNICODE_STRING __from;
		UNICODE_STRING __to;
		RtlInitUnicodeString(&__from, from);
		__to.MaximumLength = __to.Length = __from.Length;
		__to.Buffer = to;
		RtlUpcaseUnicodeString(&__to, &__from, 0);
	}
);

InternalApiCall
OptimizeExport(
	bool ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID& uuid),
	{
	GUID guid;
	UNICODE_STRING raw_str;
	RtlInitUnicodeStringEx(&raw_str, str);
	if (!NT_SUCCESS(RtlGUIDFromString(&raw_str, &guid))) {
		RtlFreeUnicodeString(&raw_str);
		return false;
	}
	RtlFreeUnicodeString(&raw_str);
	uuid = Seed::UUID(guid);
	return true;
	}
);

InternalApiCall
OptimizeExport(
	void ConvertUUIDtoCharStr(const Seed::Core::UUID& id, Seed::Core::CharStr buffer),
	{
		UNICODE_STRING raw_str;
		GUID rawId = id;
		RtlStringFromGUID(&rawId, &raw_str);
		Seed::CStr::Copy(raw_str.Buffer, buffer);
		RtlFreeUnicodeString(&raw_str);
	}
);

InternalApiCall
OptimizeExport(
	Seed::Core::Char UpcaseCharImp(Seed::Core::Char symb),
	{
		return RtlUpcaseUnicodeChar(symb);
	}
);

OptimizeExport(
	Seed::Core::Char DowncaseCharImp(Seed::Core::Char symb),
	{
		return RtlDowncaseUnicodeChar(symb);
	}
);

InternalApiCall
OptimizeExport(
	void DowncaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to),
	{
		UNICODE_STRING __from;
		UNICODE_STRING __to;
		RtlInitUnicodeString(&__from, from);
		__to.MaximumLength = __to.Length = __from.Length;
		__to.Buffer = to;
		RtlDowncaseUnicodeString(&__to, &__from, FALSE);
	}
);

InternalApiCall
OptimizeExport(
	Seed::Int32 CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize), {
		return RtlCompareUnicodeStrings(left, leftSize, right, rightSize, TRUE);
	}
);

#pragma warning(pop)