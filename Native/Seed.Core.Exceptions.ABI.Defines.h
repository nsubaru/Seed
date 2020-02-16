#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"

#ifndef __INTELLISENSE__
#ifndef DECLSPEC_GUARDNOCF
#if (_MSC_FULL_VER >= 170065501) || defined(_D1VERSIONLKG171_)
#define DECLSPEC_GUARDNOCF  __declspec(guard(nocf))
#else
#define DECLSPEC_GUARDNOCF
#endif
#endif

#ifndef DECLSPEC_GUARD_SUPPRESS
#if (_MSC_FULL_VER >= 181040116) || defined(_D1VERSIONLKG171_)
#define DECLSPEC_GUARD_SUPPRESS  __declspec(guard(suppress))
#else
#define DECLSPEC_GUARD_SUPPRESS
#endif
#endif
#else
#ifndef DECLSPEC_GUARDNOCF
#define DECLSPEC_GUARDNOCF
#endif

#ifndef DECLSPEC_GUARD_SUPPRESS
#define DECLSPEC_GUARD_SUPPRESS
#endif
#endif // !__INTELLISENSE__



constexpr Seed::UInt64 EH_EXCEPTION_NUMBER = ('msc' | 0xE0000000);	// The NT Exception # that we use

// As magic numbers increase, we have to keep track of the versions that we are
// backwards compatible with.  The top 3 bits of the magic number DWORD are
// used for other purposes, so while the magic number started as a YYYYMMDD
// date, it can't represent the year 2000 or later.  Old CRTs also recognize
// unknown magic numbers with a >= test.  Therefore, we just increment the
// the magic number by one every time we need another.
//
// EH_MAGIC_NUMBER1     The original, still used in exception records for
//                      native or mixed C++ thrown objects.
// EH_MAGIC_NUMBER2     Used in the FuncInfo if exception specifications are
//                      supported and FuncInfo::pESTypeList is valid.
// EH_MAGIC_NUMBER3     Used in the FuncInfo if FuncInfo::EHFlags is valid, so
//                      we can check if the function was compiled -EHs or -EHa.

constexpr Seed::UInt64 EH_MAGIC_NUMBER1 = 0x19930520;
constexpr Seed::UInt64 EH_MAGIC_NUMBER2 = 0x19930521;
constexpr Seed::UInt64 EH_MAGIC_NUMBER3 = 0x19930522;

// We use a different magic number in the thrown object's exception record to
// indicate it arose from a pure region.  Pure throws can't be caught by native
// or mixed code, and vice-versa.  Note that the pure magic number is only 7
// digits, not 8, so it will be less than any of the native EH magic numbers
// and won't be detected as an unknown newer magic number by the native EH
// handlers.

constexpr auto EH_PURE_MAGIC_NUMBER1 = 0x01994000;

constexpr auto FI_EHS_FLAG = 0x00000001;
constexpr auto FI_DYNSTKALIGN_FLAG = 0x00000002;
constexpr auto FI_EHNOEXCEPT_FLAG = 0x00000004;

static_assert(!((EH_MAGIC_NUMBER2 <= EH_MAGIC_NUMBER1) || (EH_MAGIC_NUMBER3 <= EH_MAGIC_NUMBER2)), 
	"New magic number must be greater than the old one");

static_assert(!((EH_MAGIC_NUMBER1 & 0xE0000000) || (EH_MAGIC_NUMBER2 & 0xE0000000) || (EH_MAGIC_NUMBER3 & 0xE0000000)),
	"Magic number too big -- must fit into 29 bits");

static_assert(!(EH_PURE_MAGIC_NUMBER1 >= EH_MAGIC_NUMBER1),
	"Pure EH magic number must be less than native one");

constexpr auto EH_MAGIC_HAS_ES = EH_MAGIC_NUMBER2;	// Magic number is greater or equal than that
													// indicates presence of exception specification

constexpr auto EH_EXCEPTION_PARAMETERS = 4;			// Number of parameters in exception record

//
// Current state of a function.
// -1 is the 'blank' state, i.e. there is nothing to unwind, no try blocks active.
//

using __ehstate_t = Seed::Int32 ;		// The type of a state index
constexpr __ehstate_t INVALID_CATCH_SPECIFIC_STATE = -2;
constexpr __ehstate_t EH_EMPTY_STATE = -1;

constexpr auto SCOPE_INDEX_SHIFT_VALUE = 2;

constexpr auto CT_IsSimpleType		= 0x00000001;		// type is a simple type
constexpr auto CT_ByReferenceOnly	= 0x00000002;		// type must be caught by reference
constexpr auto CT_HasVirtualBase	= 0x00000004;		// type is a class with virtual bases
constexpr auto CT_IsWinRTHandle		= 0x00000008;		// type is a winrt handle
constexpr auto CT_IsStdBadAlloc		= 0x00000010;		// type is a std::bad_alloc

constexpr auto TI_IsConst			= 0x00000001;		// thrown object has const qualifier
constexpr auto TI_IsVolatile		= 0x00000002;		// thrown object has volatile qualifier
constexpr auto TI_IsUnaligned		= 0x00000004;		// thrown object has unaligned qualifier
constexpr auto TI_IsPure			= 0x00000008;		// object thrown from a pure module
constexpr auto TI_IsWinRT			= 0x00000010;		// object thrown is a WinRT Exception

constexpr auto HT_IsConst			= 0x00000001;		// type referenced is 'const' qualified
constexpr auto HT_IsVolatile		= 0x00000002;		// type referenced is 'volatile' qualified
constexpr auto HT_IsUnaligned		= 0x00000004;		// type referenced is 'unaligned' qualified
constexpr auto HT_IsReference		= 0x00000008;		// catch type is by reference
constexpr auto HT_IsResumable		= 0x00000010;		// the catch may choose to resume (Reserved)
constexpr auto HT_IsStdDotDot		= 0x00000040;		// the catch is std C++ catch(...) which is supposed to catch only C++ exceptions
constexpr auto HT_IsBadAllocCompat	= 0x00000080;		// the WinRT type can catch a std::bad_alloc
constexpr auto HT_IsComplusEh		= 0x80000000;		// Is handling within complus EH