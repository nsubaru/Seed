#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.UUID.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
#pragma warning(disable: 4996)

inline 
bool ConvertCharStrToUUID(Seed::Core::CharStr str, Seed::Core::UUID & uuid) {
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

inline 
void ConvertUUIDtoCharStr(const Seed::Core::UUID & id, Seed::Core::CharStr buffer) {
	UNICODE_STRING raw_str;
	GUID rawId = id;
	RtlStringFromGUID(&rawId, &raw_str);
	Seed::CStr::Copy(raw_str.Buffer, buffer);
	RtlFreeUnicodeString(&raw_str);
}

inline 
Seed::Core::Char UpcaseCharImp(Seed::Core::Char symb) {
	return RtlUpcaseUnicodeChar(symb);
}

inline
Seed::Core::Char DowncaseCharImp(Seed::Core::Char symb) {
	return RtlDowncaseUnicodeChar(symb);
}

inline
void UppercaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to) {
	UNICODE_STRING __from, __to;
	RtlInitUnicodeString(&__from, from);
	__to.MaximumLength = __to.Length = __from.Length;
	__to.Buffer = to;
	RtlUpcaseUnicodeString(&__to, &__from, FALSE);
}

inline
void DowncaseCharStrImpl(Seed::Core::CharStr from, Seed::Core::CharStr to) {
	UNICODE_STRING __from, __to;
	RtlInitUnicodeString(&__from, from);
	__to.MaximumLength = __to.Length = __from.Length;
	__to.Buffer = to;
	RtlDowncaseUnicodeString(&__to, &__from, FALSE);
}

#pragma warning(pop)