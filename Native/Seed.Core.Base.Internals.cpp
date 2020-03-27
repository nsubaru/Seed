#include "Seed.Core.Base.Internals.h"
#include "Seed.Core.Base.CStr.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)

void Seed::Core::Internals::UppercaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to) {
	UNICODE_STRING __from;
	UNICODE_STRING __to;
	RtlInitUnicodeString(&__from, from);
	__to.MaximumLength = __to.Length = __from.Length;
	__to.Buffer = to;
	RtlUpcaseUnicodeString(&__to, &__from, 0);
}

bool Seed::Core::Internals::ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID& uuid) {
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

void Seed::Core::Internals::ConvertUUIDtoCharStr(const Seed::Core::UUID& id, Seed::Core::CharStr buffer) {
	UNICODE_STRING raw_str;
	GUID rawId = id;
	RtlStringFromGUID(&rawId, &raw_str);
	Seed::CStr::Copy(raw_str.Buffer, buffer);
	RtlFreeUnicodeString(&raw_str);
}

Seed::Core::Char Seed::Core::Internals::UpcaseCharImp(Seed::Core::Char symb) {
	return RtlUpcaseUnicodeChar(symb);
}

Seed::Core::Char Seed::Core::Internals::DowncaseCharImp(Seed::Core::Char symb) {
	return RtlDowncaseUnicodeChar(symb);
}

void Seed::Core::Internals::DowncaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to) {
	UNICODE_STRING __from;
	UNICODE_STRING __to;
	RtlInitUnicodeString(&__from, from);
	__to.MaximumLength = __to.Length = __from.Length;
	__to.Buffer = to;
	RtlDowncaseUnicodeString(&__to, &__from, FALSE);
}

Seed::Int32 Seed::Core::Internals::CompareString(Seed::CharStr left, Seed::CharStr right, Seed::SizeT leftSize, Seed::SizeT rightSize) {
	return RtlCompareUnicodeStrings(left, leftSize, right, rightSize, FALSE);
}

#pragma warning(pop)