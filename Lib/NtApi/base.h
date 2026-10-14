#pragma once
#ifndef SEED_RT_ALLOW_INTERNAL_NT_API
#error "Internal NT API not allowed"
#endif // !SEED_RT_ALLOW_INTERNAL_NT_API

#ifdef _WINDOWS_
#define SEED_RT_WINDOWS_HEADERS_INCLUDED
#endif

#ifndef SEED_RT_WINDOWS_HEADERS_INCLUDED
#include "sal.h"
#if __has_include("ntintsafe.h")
#include "ntintsafe.h"
#endif
#include "intrin.h"

#pragma warning(push)
//This file containe typedefs for Win API and NT API without annotations on not fully annotated as
//Seed C++ Runtime Implementation not use SAL annotations
#pragma warning(disable: 28301)

#define _WINNT_
#define _AMD64_

#define DECLSPEC_ALIGN(x) __declspec(align(x))
#define MEMORY_ALLOCATION_ALIGNMENT 16

typedef unsigned long long ULONG_PTR; //-V677
typedef long LONG, * PLONG; //-V677

typedef const char* PCSTR; //-V677
typedef unsigned char UCHAR; //-V677

typedef unsigned long ULONG; //-V677
typedef ULONG* PULONG; //-V677
typedef unsigned short USHORT; //-V677
typedef USHORT* PUSHORT; //-V677
typedef unsigned char UCHAR; //-V677
typedef UCHAR* PUCHAR; //-V677
typedef _Null_terminated_ char* PSZ;

#define MAX_PATH          260

#define DUMMYSTRUCTNAME
#define DUMMYSTRUCTNAME2
#define DUMMYSTRUCTNAME3
#define DUMMYSTRUCTNAME4
#define DUMMYSTRUCTNAME5
#define DUMMYSTRUCTNAME6

#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYUNIONNAME5
#define DUMMYUNIONNAME6
#define DUMMYUNIONNAME7
#define DUMMYUNIONNAME8
#define DUMMYUNIONNAME9

#define DUMMYUNIONNAME

#define far
#define near

#define FAR                 far
#define NEAR                near
#define CONST               const

typedef unsigned long       DWORD; //-V677
typedef int                 BOOL; //-V677
typedef unsigned char       BYTE; //-V677
typedef unsigned short      WORD; //-V677
typedef float               FLOAT; //-V677
typedef FLOAT* PFLOAT; //-V677
typedef BOOL near* PBOOL; //-V677
typedef BOOL far* LPBOOL; //-V677
typedef BYTE near* PBYTE; //-V677
typedef BYTE far* LPBYTE; //-V677
typedef int near* PINT; //-V677
typedef int far* LPINT; //-V677
typedef WORD near* PWORD; //-V677
typedef WORD far* LPWORD; //-V677
typedef long far* LPLONG; //-V677
typedef DWORD near* PDWORD; //-V677
typedef DWORD far* LPDWORD; //-V677
typedef void far* LPVOID; //-V677
typedef CONST void far* LPCVOID; //-V677

typedef signed char         INT8, * PINT8;
typedef signed short        INT16, * PINT16;
typedef signed int          INT32, * PINT32;
typedef signed __int64      INT64, * PINT64;
typedef unsigned char       UINT8, * PUINT8;
typedef unsigned short      UINT16, * PUINT16;
typedef unsigned int        UINT32, * PUINT32;
typedef unsigned __int64    UINT64, * PUINT64;

typedef int                 INT; //-V677
typedef unsigned int        UINT; //-V677
typedef unsigned int* PUINT; //-V677

typedef __int64 LONGLONG; //-V677
typedef unsigned __int64 ULONGLONG; //-V677

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER, * PLARGE_INTEGER;

#define UNALIGNED __unaligned

typedef wchar_t WCHAR; //-V677

typedef WCHAR* PWCHAR, * LPWCH, * PWCH; //-V677
typedef CONST WCHAR* LPCWCH, * PCWCH;

typedef _Null_terminated_ WCHAR* NWPSTR, * LPWSTR, * PWSTR; //-V677
typedef _Null_terminated_ PWSTR* PZPWSTR;
typedef _Null_terminated_ CONST PWSTR* PCZPWSTR;
typedef _Null_terminated_ WCHAR UNALIGNED* LPUWSTR, * PUWSTR;
typedef _Null_terminated_ CONST WCHAR* LPCWSTR, * PCWSTR; //-V677
typedef _Null_terminated_ PCWSTR* PZPCWSTR;
typedef _Null_terminated_ CONST PCWSTR* PCZPCWSTR;
typedef _Null_terminated_ CONST WCHAR UNALIGNED* LPCUWSTR, * PCUWSTR;

typedef _NullNull_terminated_ WCHAR* PZZWSTR;
typedef _NullNull_terminated_ CONST WCHAR* PCZZWSTR;
typedef _NullNull_terminated_ WCHAR UNALIGNED* PUZZWSTR;
typedef _NullNull_terminated_ CONST WCHAR UNALIGNED* PCUZZWSTR;

typedef  WCHAR* PNZWCH;
typedef  CONST WCHAR* PCNZWCH;
typedef  WCHAR UNALIGNED* PUNZWCH;
typedef  CONST WCHAR UNALIGNED* PCUNZWCH;

#define VOID void
typedef char CHAR; //-V677
typedef short SHORT; //-V677
typedef long LONG; //-V677
typedef int INT; //-V677

typedef CONST WCHAR* LPCWCHAR, * PCWCHAR;
typedef CONST WCHAR UNALIGNED* LPCUWCHAR, * PCUWCHAR;

typedef CHAR* PCHAR, * LPCH, * PCH; //-V677
typedef CONST CHAR* LPCCH, * PCCH;

typedef _Null_terminated_ CHAR* NPSTR, * LPSTR, * PSTR; //-V677
typedef _Null_terminated_ PSTR* PZPSTR;
typedef _Null_terminated_ CONST PSTR* PCZPSTR;
typedef _Null_terminated_ CONST CHAR* LPCSTR, * PCSTR; //-V677
typedef _Null_terminated_ PCSTR* PZPCSTR;
typedef _Null_terminated_ CONST PCSTR* PCZPCSTR;

typedef _NullNull_terminated_ CHAR* PZZSTR;
typedef _NullNull_terminated_ CONST CHAR* PCZZSTR;

typedef  CHAR* PNZCH;
typedef  CONST CHAR* PCNZCH;

typedef void* HANDLE; //-V677
typedef void* PVOID; //-V677

typedef unsigned __int64 ULONG64, * PULONG64; //-V677
typedef unsigned __int64 DWORD64, * PDWORD64; //-V677

#define DECLSPEC_IMPORT __declspec(dllimport)

#define NTSYSAPI     DECLSPEC_IMPORT
#define NTSYSCALLAPI DECLSPEC_IMPORT

#define NTAPI __stdcall

#if !defined(_M_CEE_PURE)
#define NTAPI_INLINE    NTAPI
#else
#define NTAPI_INLINE
#endif

typedef BYTE  BOOLEAN; //-V677
typedef BOOLEAN* PBOOLEAN; //-V677

typedef struct _SINGLE_LIST_ENTRY {
    struct _SINGLE_LIST_ENTRY* Next;
} SINGLE_LIST_ENTRY, * PSINGLE_LIST_ENTRY;

#define RESTRICTED_POINTER

typedef struct _LIST_ENTRY {
    struct _LIST_ENTRY* Flink;
    struct _LIST_ENTRY* Blink;
} LIST_ENTRY, * PLIST_ENTRY, * RESTRICTED_POINTER PRLIST_ENTRY;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } DUMMYSTRUCTNAME;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER* PULARGE_INTEGER;

typedef __int64 LONG_PTR, * PLONG_PTR; //-V677
typedef unsigned __int64 ULONG_PTR, * PULONG_PTR; //-V677

typedef ULONG_PTR SIZE_T, * PSIZE_T; //-V677
typedef LONG_PTR SSIZE_T, * PSSIZE_T; //-V677

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK* PACCESS_MASK;

typedef HANDLE* PHANDLE; //-V677

#define FALSE               0
#define TRUE                1

#ifdef __cplusplus

// Define operator overloads to enable bit operations on enum values that are
// used to define flags. Use DEFINE_ENUM_FLAG_OPERATORS(YOUR_TYPE) to enable these
// operators on YOUR_TYPE.

// Moved here from objbase.w.

// Templates are defined here in order to avoid a dependency on C++ <type_traits> header file,
// or on compiler-specific constructs.
extern "C++" {

    template <size_t S>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE;

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<1>
    {
        typedef INT8 type;
    };

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<2>
    {
        typedef INT16 type;
    };

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<4>
    {
        typedef INT32 type;
    };

    template <>
    struct _ENUM_FLAG_INTEGER_FOR_SIZE<8>
    {
        typedef INT64 type;
    };

    // used as an approximation of std::underlying_type<T>
    template <class T>
    struct _ENUM_FLAG_SIZED_INTEGER
    {
        typedef typename _ENUM_FLAG_INTEGER_FOR_SIZE<sizeof(T)>::type type;
    };

}

#ifdef __cplusplus
#define EXTERN_C       extern "C"
#define EXTERN_C_START extern "C" {
#define EXTERN_C_END   }

#if _MSC_VER >= 1900
#define WIN_NOEXCEPT noexcept
#else
#define WIN_NOEXCEPT throw()
#endif

// 'noexcept' on typedefs is invalid prior to C++17
#if _MSVC_LANG >= 201703
#define WIN_NOEXCEPT_PFN noexcept
#else
#define WIN_NOEXCEPT_PFN
#endif
#else
#define EXTERN_C       extern
#define EXTERN_C_START
#define EXTERN_C_END
#define WIN_NOEXCEPT
#define WIN_NOEXCEPT_PFN
#endif

#if _MSC_VER >= 1900
#define _ENUM_FLAG_CONSTEXPR constexpr
#else
#define _ENUM_FLAG_CONSTEXPR
#endif

#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
extern "C++" { \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) WIN_NOEXCEPT { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) | ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) WIN_NOEXCEPT { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) |= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) WIN_NOEXCEPT { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) & ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) WIN_NOEXCEPT { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) &= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator ~ (ENUMTYPE a) WIN_NOEXCEPT { return ENUMTYPE(~((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a)); } \
inline _ENUM_FLAG_CONSTEXPR ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) WIN_NOEXCEPT { return ENUMTYPE(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)a) ^ ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) WIN_NOEXCEPT { return (ENUMTYPE &)(((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type &)a) ^= ((_ENUM_FLAG_SIZED_INTEGER<ENUMTYPE>::type)b)); } \
}
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif

#ifndef __GUID_DEFINED
#define __GUID_DEFINED
typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[8];
} GUID;
#endif // !__GUID_DEFINED

typedef GUID* LPGUID;
typedef const GUID* LPCGUID;

typedef ULONG_PTR  KAFFINITY;

typedef struct _GROUP_AFFINITY {
    KAFFINITY Mask;
    USHORT    Group;
    USHORT    Reserved[3];
} GROUP_AFFINITY, * PGROUP_AFFINITY;

typedef PVOID PACCESS_TOKEN;
typedef PVOID PSID;
typedef PVOID PCLAIMS_BLOB;

typedef struct _LUID {
    DWORD LowPart;
    LONG HighPart;
} LUID, * PLUID;

typedef struct _GENERIC_MAPPING {
    ACCESS_MASK GenericRead;
    ACCESS_MASK GenericWrite;
    ACCESS_MASK GenericExecute;
    ACCESS_MASK GenericAll;
} GENERIC_MAPPING;
typedef GENERIC_MAPPING* PGENERIC_MAPPING;

typedef enum _FIRMWARE_TYPE {
    FirmwareTypeUnknown,
    FirmwareTypeBios,
    FirmwareTypeUefi,
    FirmwareTypeMax
} FIRMWARE_TYPE, * PFIRMWARE_TYPE;

#define ANYSIZE_ARRAY 1

#define PROCESSOR_INTEL_386     386
#define PROCESSOR_INTEL_486     486
#define PROCESSOR_INTEL_PENTIUM 586
#define PROCESSOR_INTEL_IA64    2200
#define PROCESSOR_AMD_X8664     8664
#define PROCESSOR_MIPS_R4000    4000    // incl R4101 & R3910 for Windows CE
#define PROCESSOR_ALPHA_21064   21064
#define PROCESSOR_PPC_601       601
#define PROCESSOR_PPC_603       603
#define PROCESSOR_PPC_604       604
#define PROCESSOR_PPC_620       620
#define PROCESSOR_HITACHI_SH3   10003   // Windows CE
#define PROCESSOR_HITACHI_SH3E  10004   // Windows CE
#define PROCESSOR_HITACHI_SH4   10005   // Windows CE
#define PROCESSOR_MOTOROLA_821  821     // Windows CE
#define PROCESSOR_SHx_SH3       103     // Windows CE
#define PROCESSOR_SHx_SH4       104     // Windows CE
#define PROCESSOR_STRONGARM     2577    // Windows CE - 0xA11
#define PROCESSOR_ARM720        1824    // Windows CE - 0x720
#define PROCESSOR_ARM820        2080    // Windows CE - 0x820
#define PROCESSOR_ARM920        2336    // Windows CE - 0x920
#define PROCESSOR_ARM_7TDMI     70001   // Windows CE
#define PROCESSOR_OPTIL         0x494f  // MSIL

#define PROCESSOR_ARCHITECTURE_INTEL            0
#define PROCESSOR_ARCHITECTURE_MIPS             1
#define PROCESSOR_ARCHITECTURE_ALPHA            2
#define PROCESSOR_ARCHITECTURE_PPC              3
#define PROCESSOR_ARCHITECTURE_SHX              4
#define PROCESSOR_ARCHITECTURE_ARM              5
#define PROCESSOR_ARCHITECTURE_IA64             6
#define PROCESSOR_ARCHITECTURE_ALPHA64          7
#define PROCESSOR_ARCHITECTURE_MSIL             8
#define PROCESSOR_ARCHITECTURE_AMD64            9
#define PROCESSOR_ARCHITECTURE_IA32_ON_WIN64    10
#define PROCESSOR_ARCHITECTURE_NEUTRAL          11
#define PROCESSOR_ARCHITECTURE_ARM64            12
#define PROCESSOR_ARCHITECTURE_ARM32_ON_WIN64   13
#define PROCESSOR_ARCHITECTURE_IA32_ON_ARM64    14

#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xFFFF

#define PF_FLOATING_POINT_PRECISION_ERRATA       0
#define PF_FLOATING_POINT_EMULATED               1
#define PF_COMPARE_EXCHANGE_DOUBLE               2
#define PF_MMX_INSTRUCTIONS_AVAILABLE            3
#define PF_PPC_MOVEMEM_64BIT_OK                  4
#define PF_ALPHA_BYTE_INSTRUCTIONS               5
#define PF_XMMI_INSTRUCTIONS_AVAILABLE           6
#define PF_3DNOW_INSTRUCTIONS_AVAILABLE          7
#define PF_RDTSC_INSTRUCTION_AVAILABLE           8
#define PF_PAE_ENABLED                           9
#define PF_XMMI64_INSTRUCTIONS_AVAILABLE        10
#define PF_SSE_DAZ_MODE_AVAILABLE               11
#define PF_NX_ENABLED                           12
#define PF_SSE3_INSTRUCTIONS_AVAILABLE          13
#define PF_COMPARE_EXCHANGE128                  14
#define PF_COMPARE64_EXCHANGE128                15
#define PF_CHANNELS_ENABLED                     16
#define PF_XSAVE_ENABLED                        17
#define PF_ARM_VFP_32_REGISTERS_AVAILABLE       18
#define PF_ARM_NEON_INSTRUCTIONS_AVAILABLE      19
#define PF_SECOND_LEVEL_ADDRESS_TRANSLATION     20
#define PF_VIRT_FIRMWARE_ENABLED                21
#define PF_RDWRFSGSBASE_AVAILABLE               22
#define PF_FASTFAIL_AVAILABLE                   23
#define PF_ARM_DIVIDE_INSTRUCTION_AVAILABLE     24
#define PF_ARM_64BIT_LOADSTORE_ATOMIC           25
#define PF_ARM_EXTERNAL_CACHE_AVAILABLE         26
#define PF_ARM_FMAC_INSTRUCTIONS_AVAILABLE      27
#define PF_RDRAND_INSTRUCTION_AVAILABLE         28
#define PF_ARM_V8_INSTRUCTIONS_AVAILABLE        29
#define PF_ARM_V8_CRYPTO_INSTRUCTIONS_AVAILABLE 30
#define PF_ARM_V8_CRC32_INSTRUCTIONS_AVAILABLE  31
#define PF_RDTSCP_INSTRUCTION_AVAILABLE         32
#define PF_RDPID_INSTRUCTION_AVAILABLE          33

//
// Known extended CPU state feature BITs
//
// 0    x87
// 1    SSE
// 2    AVX
// 3    BNDREGS (B0.LB-B3.LB B0.UB-B3.UB)
// 4    BNDCSR  (BNDCFGU + BNDSTATUS)       Persistent
// 5    KMASK   (KMASK [63:0][0-7])
// 6    ZMM_H   (ZMM_H[511:256][0-15])
// 7    ZMM     (ZMM[511:0][16-31])
// 8    IPT                                 Supervisor
//
// 11   CET_U                               Supervisor
//
// 62   LWP                                 Persistent
//
// 63   RZ0                                 Reserved
//

#define XSTATE_LEGACY_FLOATING_POINT        (0)
#define XSTATE_LEGACY_SSE                   (1)
#define XSTATE_GSSE                         (2)
#define XSTATE_AVX                          (XSTATE_GSSE)
#define XSTATE_MPX_BNDREGS                  (3)
#define XSTATE_MPX_BNDCSR                   (4)
#define XSTATE_AVX512_KMASK                 (5)
#define XSTATE_AVX512_ZMM_H                 (6)
#define XSTATE_AVX512_ZMM                   (7)
#define XSTATE_IPT                          (8)
#define XSTATE_CET_U                        (11)
#define XSTATE_LWP                          (62)
#define MAXIMUM_XSTATE_FEATURES             (64)

//
// Known extended CPU state feature MASKs
//

#define XSTATE_MASK_LEGACY_FLOATING_POINT   (1ui64 << (XSTATE_LEGACY_FLOATING_POINT))
#define XSTATE_MASK_LEGACY_SSE              (1ui64 << (XSTATE_LEGACY_SSE))
#define XSTATE_MASK_LEGACY                  (XSTATE_MASK_LEGACY_FLOATING_POINT | \
                                             XSTATE_MASK_LEGACY_SSE)

#define XSTATE_MASK_GSSE                    (1ui64 << (XSTATE_GSSE))
#define XSTATE_MASK_AVX                     (XSTATE_MASK_GSSE)
#define XSTATE_MASK_MPX                     ((1ui64 << (XSTATE_MPX_BNDREGS)) | \
                                             (1ui64 << (XSTATE_MPX_BNDCSR)))

#define XSTATE_MASK_AVX512                  ((1ui64 << (XSTATE_AVX512_KMASK)) | \
                                             (1ui64 << (XSTATE_AVX512_ZMM_H)) | \
                                             (1ui64 << (XSTATE_AVX512_ZMM)))

#define XSTATE_MASK_IPT                     (1ui64 << (XSTATE_IPT))
#define XSTATE_MASK_CET_U                   (1ui64 << (XSTATE_CET_U))
#define XSTATE_MASK_LWP                     (1ui64 << (XSTATE_LWP))

#define XSTATE_MASK_ALLOWED                 (XSTATE_MASK_LEGACY | \
                                             XSTATE_MASK_AVX | \
                                             XSTATE_MASK_MPX | \
                                             XSTATE_MASK_AVX512 | \
                                             XSTATE_MASK_IPT | \
                                             XSTATE_MASK_CET_U | \
                                             XSTATE_MASK_LWP)

#define XSTATE_MASK_PERSISTENT              ((1ui64 << (XSTATE_MPX_BNDCSR)) | \
                                             XSTATE_MASK_LWP)

#define XSTATE_MASK_USER_VISIBLE_SUPERVISOR (XSTATE_MASK_CET_U)

//
// Flags associated with compaction mask
//

#define XSTATE_COMPACTION_ENABLE            (63)
#define XSTATE_COMPACTION_ENABLE_MASK       (1ui64 << (XSTATE_COMPACTION_ENABLE))

#define XSTATE_ALIGN_BIT                    (1)
#define XSTATE_ALIGN_MASK                   (1ui64 << (XSTATE_ALIGN_BIT))

#define XSTATE_CONTROLFLAG_XSAVEOPT_MASK    (1)
#define XSTATE_CONTROLFLAG_XSAVEC_MASK      (2)
#define XSTATE_CONTROLFLAG_VALID_MASK       (XSTATE_CONTROLFLAG_XSAVEOPT_MASK | \
                                             XSTATE_CONTROLFLAG_XSAVEC_MASK)

//
// Extended processor state configuration
//

typedef struct _XSTATE_FEATURE {
    DWORD Offset;
    DWORD Size;
} XSTATE_FEATURE, * PXSTATE_FEATURE;

typedef struct _XSTATE_CONFIGURATION {
    // Mask of all enabled features
    DWORD64 EnabledFeatures;

    // Mask of volatile enabled features
    DWORD64 EnabledVolatileFeatures;

    // Total size of the save area for user states
    DWORD Size;

    // Control Flags
    union {
        DWORD ControlFlags;
        struct
        {
            DWORD OptimizedSave : 1;
            DWORD CompactionEnabled : 1;
            DWORD ExtendedFeatureDisable : 1;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    // List of features
    XSTATE_FEATURE Features[MAXIMUM_XSTATE_FEATURES];

    // Mask of all supervisor features
    DWORD64 EnabledSupervisorFeatures;

    // Mask of features that require start address to be 64 byte aligned
    DWORD64 AlignedFeatures;

    // Total size of the save area for user and supervisor states
    DWORD AllFeatureSize;

    // List which holds size of each user and supervisor state supported by CPU
    DWORD AllFeatures[MAXIMUM_XSTATE_FEATURES];

    // Mask of all supervisor features that are exposed to user-mode
    DWORD64 EnabledUserVisibleSupervisorFeatures;

    // Mask of features that can be disabled via XFD
    DWORD64 ExtendedFeatureDisableFeatures;

    // Total size of the save area for non-large user and supervisor states
    DWORD AllNonLargeFeatureSize;

    // The maximum supported ARM64 SVE vector length that can be used in the
    // current environment, in bytes.
    WORD   MaxSveVectorLength;

    WORD   Spare1;

} XSTATE_CONFIGURATION, * PXSTATE_CONFIGURATION;

#define FIELD_OFFSET(type, field)    ((LONG)(LONG_PTR)&(((type *)0)->field))
#define UFIELD_OFFSET(type, field)    ((DWORD)(LONG_PTR)&(((type *)0)->field))

#define FORCEINLINE __forceinline

typedef char CCHAR; //-V677
typedef DWORD LCID; //-V677
typedef PDWORD PLCID; //-V677
typedef WORD   LANGID; //-V677

typedef DWORD SECURITY_INFORMATION, * PSECURITY_INFORMATION;

#define OWNER_SECURITY_INFORMATION                  (0x00000001L)
#define GROUP_SECURITY_INFORMATION                  (0x00000002L)
#define DACL_SECURITY_INFORMATION                   (0x00000004L)
#define SACL_SECURITY_INFORMATION                   (0x00000008L)
#define LABEL_SECURITY_INFORMATION                  (0x00000010L)
#define ATTRIBUTE_SECURITY_INFORMATION              (0x00000020L)
#define SCOPE_SECURITY_INFORMATION                  (0x00000040L)
#define PROCESS_TRUST_LABEL_SECURITY_INFORMATION    (0x00000080L)
#define ACCESS_FILTER_SECURITY_INFORMATION          (0x00000100L)
#define BACKUP_SECURITY_INFORMATION                 (0x00010000L)

#define PROTECTED_DACL_SECURITY_INFORMATION         (0x80000000L)
#define PROTECTED_SACL_SECURITY_INFORMATION         (0x40000000L)
#define UNPROTECTED_DACL_SECURITY_INFORMATION       (0x20000000L)
#define UNPROTECTED_SACL_SECURITY_INFORMATION       (0x10000000L)

//
// Call target should be made valid only if it is suppressed export.
// What this flag means is that it can *only* be used on a cell which is
// currently in the CFG export suppressed state (only considered for export
// suppressed processes and not legacy CFG processes!), and it is also
// allowed to be used even if the process is a restricted (i.e. no ACG) process.
//

#define CFG_CALL_TARGET_CONVERT_EXPORT_SUPPRESSED_TO_VALID  (0x00000004)

typedef struct _CFG_CALL_TARGET_INFO {
    ULONG_PTR Offset;
    ULONG_PTR Flags;
} CFG_CALL_TARGET_INFO, * PCFG_CALL_TARGET_INFO;

typedef union DECLSPEC_ALIGN(16) _SLIST_HEADER {
    struct {  // original struct
        ULONGLONG Alignment;
        ULONGLONG Region;
    } DUMMYSTRUCTNAME;
    struct {  // x64 16-byte header
        ULONGLONG Depth : 16;
        ULONGLONG Sequence : 48;
        ULONGLONG Reserved : 4;
        ULONGLONG NextEntry : 60; // last 4 bits are always 0's
    } HeaderX64;
} SLIST_HEADER, * PSLIST_HEADER;

//
// The type SINGLE_LIST_ENTRY is not suitable for use with SLISTs.  For
// WIN64, an entry on an SLIST is required to be 16-byte aligned, while a
// SINGLE_LIST_ENTRY structure has only 8 byte alignment.
//
// Therefore, all SLIST code should use the SLIST_ENTRY type instead of the
// SINGLE_LIST_ENTRY type.
//

#pragma warning(push)
#pragma warning(disable:4324)   // structure padded due to align()

typedef struct DECLSPEC_ALIGN(16) _SLIST_ENTRY {
    struct _SLIST_ENTRY* Next;
} SLIST_ENTRY, * PSLIST_ENTRY;

#pragma warning(pop)

#define FLS_MAXIMUM_AVAILABLE 128
#define TLS_MINIMUM_AVAILABLE 64

// Exception disposition return values
typedef enum _EXCEPTION_DISPOSITION
{
    ExceptionContinueExecution,
    ExceptionContinueSearch,
    ExceptionNestedException,
    ExceptionCollidedUnwind
} EXCEPTION_DISPOSITION;

typedef
_Function_class_(EXCEPTION_ROUTINE)
EXCEPTION_DISPOSITION
NTAPI
EXCEPTION_ROUTINE(
    _Inout_ struct _EXCEPTION_RECORD* ExceptionRecord,
    _In_ PVOID EstablisherFrame,
    _Inout_ struct _CONTEXT* ContextRecord,
    _In_ PVOID DispatcherContext
);

typedef EXCEPTION_ROUTINE* PEXCEPTION_ROUTINE;

typedef struct _EXCEPTION_REGISTRATION_RECORD {
    struct _EXCEPTION_REGISTRATION_RECORD* Next;
    PEXCEPTION_ROUTINE Handler;
} EXCEPTION_REGISTRATION_RECORD;

typedef EXCEPTION_REGISTRATION_RECORD* PEXCEPTION_REGISTRATION_RECORD;

typedef struct _NT_TIB {
    struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList;
    PVOID StackBase;
    PVOID StackLimit;
    PVOID SubSystemTib;
#if defined(_MSC_EXTENSIONS)
    union {
        PVOID FiberData;
        DWORD Version;
    };
#else
    PVOID FiberData;
#endif
    PVOID ArbitraryUserPointer;
    struct _NT_TIB* Self;
} NT_TIB;
typedef NT_TIB* PNT_TIB;

#define ALL_PROCESSOR_GROUPS        0xffff

//
// Structure to represent a system wide processor number. It contains a
// group number and relative processor number within the group.
//

typedef struct _PROCESSOR_NUMBER {
    WORD   Group;
    BYTE  Number;
    BYTE  Reserved;
} PROCESSOR_NUMBER, * PPROCESSOR_NUMBER;

typedef struct _LDT_ENTRY {
    WORD    LimitLow;
    WORD    BaseLow;
    union {
        struct {
            BYTE    BaseMid;
            BYTE    Flags1;     // Declare as bytes to avoid alignment
            BYTE    Flags2;     // Problems.
            BYTE    BaseHi;
        } Bytes;
        struct {
            DWORD   BaseMid : 8;
            DWORD   Type : 5;
            DWORD   Dpl : 2;
            DWORD   Pres : 1;
            DWORD   LimitHi : 4;
            DWORD   Sys : 1;
            DWORD   Reserved_0 : 1;
            DWORD   Default_Big : 1;
            DWORD   Granularity : 1;
            DWORD   BaseHi : 8;
        } Bits;
    } HighWord;
} LDT_ENTRY, * PLDT_ENTRY;

typedef enum _PROCESS_MITIGATION_POLICY {
    ProcessDEPPolicy,
    ProcessASLRPolicy,
    ProcessDynamicCodePolicy,
    ProcessStrictHandleCheckPolicy,
    ProcessSystemCallDisablePolicy,
    ProcessMitigationOptionsMask,
    ProcessExtensionPointDisablePolicy,
    ProcessControlFlowGuardPolicy,
    ProcessSignaturePolicy,
    ProcessFontDisablePolicy,
    ProcessImageLoadPolicy,
    ProcessSystemCallFilterPolicy,
    ProcessPayloadRestrictionPolicy,
    ProcessChildProcessPolicy,
    ProcessSideChannelIsolationPolicy,
    MaxProcessMitigationPolicy
} PROCESS_MITIGATION_POLICY, * PPROCESS_MITIGATION_POLICY;

//
// N.B.  High entropy mode is read only and can only be set at creation time
//       and not via the ProcessMitigationPolicy APIs.
//

typedef struct _PROCESS_MITIGATION_ASLR_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD EnableBottomUpRandomization : 1;
            DWORD EnableForceRelocateImages : 1;
            DWORD EnableHighEntropy : 1;
            DWORD DisallowStrippedImages : 1;
            DWORD ReservedFlags : 28;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_ASLR_POLICY, * PPROCESS_MITIGATION_ASLR_POLICY;

typedef struct _PROCESS_MITIGATION_DEP_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD Enable : 1;
            DWORD DisableAtlThunkEmulation : 1;
            DWORD ReservedFlags : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
    BOOLEAN Permanent;
} PROCESS_MITIGATION_DEP_POLICY, * PPROCESS_MITIGATION_DEP_POLICY;

typedef struct _PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD RaiseExceptionOnInvalidHandleReference : 1;
            DWORD HandleExceptionsPermanentlyEnabled : 1;
            DWORD ReservedFlags : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY, * PPROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY;

typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD DisallowWin32kSystemCalls : 1;
            DWORD AuditDisallowWin32kSystemCalls : 1;
            DWORD ReservedFlags : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY, * PPROCESS_MITIGATION_SYSTEM_CALL_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD DisableExtensionPoints : 1;
            DWORD ReservedFlags : 31;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY, * PPROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_DYNAMIC_CODE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD ProhibitDynamicCode : 1;
            DWORD AllowThreadOptOut : 1;
            DWORD AllowRemoteDowngrade : 1;
            DWORD AuditProhibitDynamicCode : 1;
            DWORD ReservedFlags : 28;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_DYNAMIC_CODE_POLICY, * PPROCESS_MITIGATION_DYNAMIC_CODE_POLICY;

typedef struct _PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD EnableControlFlowGuard : 1;
            DWORD EnableExportSuppression : 1;
            DWORD StrictMode : 1;
            DWORD ReservedFlags : 29;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY, * PPROCESS_MITIGATION_CONTROL_FLOW_GUARD_POLICY;

typedef struct _PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD MicrosoftSignedOnly : 1;
            DWORD StoreSignedOnly : 1;
            DWORD MitigationOptIn : 1;
            DWORD AuditMicrosoftSignedOnly : 1;
            DWORD AuditStoreSignedOnly : 1;
            DWORD ReservedFlags : 27;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY, * PPROCESS_MITIGATION_BINARY_SIGNATURE_POLICY;

typedef struct _PROCESS_MITIGATION_FONT_DISABLE_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD DisableNonSystemFonts : 1;
            DWORD AuditNonSystemFontLoading : 1;
            DWORD ReservedFlags : 30;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_FONT_DISABLE_POLICY, * PPROCESS_MITIGATION_FONT_DISABLE_POLICY;

typedef struct _PROCESS_MITIGATION_IMAGE_LOAD_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD NoRemoteImages : 1;
            DWORD NoLowMandatoryLabelImages : 1;
            DWORD PreferSystem32Images : 1;
            DWORD AuditNoRemoteImages : 1;
            DWORD AuditNoLowMandatoryLabelImages : 1;
            DWORD ReservedFlags : 27;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_IMAGE_LOAD_POLICY, * PPROCESS_MITIGATION_IMAGE_LOAD_POLICY;

typedef struct _PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD FilterId : 4;
            DWORD ReservedFlags : 28;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY, * PPROCESS_MITIGATION_SYSTEM_CALL_FILTER_POLICY;

typedef struct _PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD EnableExportAddressFilter : 1;
            DWORD AuditExportAddressFilter : 1;

            DWORD EnableExportAddressFilterPlus : 1;
            DWORD AuditExportAddressFilterPlus : 1;

            DWORD EnableImportAddressFilter : 1;
            DWORD AuditImportAddressFilter : 1;

            DWORD EnableRopStackPivot : 1;
            DWORD AuditRopStackPivot : 1;

            DWORD EnableRopCallerCheck : 1;
            DWORD AuditRopCallerCheck : 1;

            DWORD EnableRopSimExec : 1;
            DWORD AuditRopSimExec : 1;

            DWORD ReservedFlags : 20;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY, * PPROCESS_MITIGATION_PAYLOAD_RESTRICTION_POLICY;

typedef struct _PROCESS_MITIGATION_CHILD_PROCESS_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD NoChildProcessCreation : 1;
            DWORD AuditNoChildProcessCreation : 1;
            DWORD AllowSecureProcessCreation : 1;
            DWORD ReservedFlags : 29;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_CHILD_PROCESS_POLICY, * PPROCESS_MITIGATION_CHILD_PROCESS_POLICY;

typedef struct _PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY {
    union {
        DWORD Flags;
        struct {
            //
            // Prevent branch target pollution cross-SMT-thread in user mode.
            //

            DWORD SmtBranchTargetIsolation : 1;

            //
            // Isolate this process into a distinct security domain, even from
            // other processes running as the same security context.  This
            // prevents branch target injection cross-process (normally such
            // branch target injection is only inhibited across different
            // security contexts).
            //
            // Page combining is limited to processes within the same security
            // domain.  This flag thus also effectively limits the process to
            // only being able to combine internally to the process itself,
            // except for common pages (unless further restricted by the
            // DisablePageCombine policy).
            //

            DWORD IsolateSecurityDomain : 1;

            //
            // Disable all page combining for this process, even internally to
            // the process itself, except for common pages (zeroes or ones).
            //

            DWORD DisablePageCombine : 1;

            //
            // Memory Disambiguation Disable.
            //

            DWORD SpeculativeStoreBypassDisable : 1;

            DWORD ReservedFlags : 28;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY, * PPROCESS_MITIGATION_SIDE_CHANNEL_ISOLATION_POLICY;

typedef struct _PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY {
    union {
        DWORD Flags;
        struct {
            DWORD EnableUserShadowStack : 1;
            DWORD AuditUserShadowStack : 1;
            DWORD SetContextIpValidation : 1;
            DWORD AuditSetContextIpValidation : 1;
            DWORD EnableUserShadowStackStrictMode : 1;
            DWORD BlockNonCetBinaries : 1;
            DWORD BlockNonCetBinariesNonEhcont : 1;
            DWORD AuditBlockNonCetBinaries : 1;
            DWORD CetDynamicApisOutOfProcOnly : 1;
            DWORD SetContextIpValidationRelaxedMode : 1;
            DWORD ReservedFlags : 22;

        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} PROCESS_MITIGATION_USER_SHADOW_STACK_POLICY, * PPROCESS_MITIGATION_USER_SHADOW_STACK_POLICY;

#define MAX_HW_COUNTERS 16

typedef struct DECLSPEC_ALIGN(16) _M128A {
    ULONGLONG Low;
    LONGLONG High;
} M128A, * PM128A;

//
// Format of data for (F)XSAVE/(F)XRSTOR instruction
//

typedef struct DECLSPEC_ALIGN(16) _XSAVE_FORMAT {
    WORD   ControlWord;
    WORD   StatusWord;
    BYTE  TagWord;
    BYTE  Reserved1;
    WORD   ErrorOpcode;
    DWORD ErrorOffset;
    WORD   ErrorSelector;
    WORD   Reserved2;
    DWORD DataOffset;
    WORD   DataSelector;
    WORD   Reserved3;
    DWORD MxCsr;
    DWORD MxCsr_Mask;
    M128A FloatRegisters[8];

#if defined(_WIN64)

    M128A XmmRegisters[16];
    BYTE  Reserved4[96];

#else

    M128A XmmRegisters[8];
    BYTE  Reserved4[224];

#endif
} XSAVE_FORMAT, * PXSAVE_FORMAT;

typedef XSAVE_FORMAT XMM_SAVE_AREA32, * PXMM_SAVE_AREA32;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) it is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//
// The flags field within this record controls the contents of a CONTEXT
// record.
//
// If the context record is used as an input parameter, then for each
// portion of the context record controlled by a flag whose value is
// set, it is assumed that that portion of the context record contains
// valid context. If the context record is being used to modify a threads
// context, then only that portion of the threads context is modified.
//
// If the context record is used as an output parameter to capture the
// context of a thread, then only those portions of the thread's context
// corresponding to set flags will be returned.
//
// CONTEXT_CONTROL specifies SegSs, Rsp, SegCs, Rip, and EFlags.
//
// CONTEXT_INTEGER specifies Rax, Rcx, Rdx, Rbx, Rbp, Rsi, Rdi, and R8-R15.
//
// CONTEXT_SEGMENTS specifies SegDs, SegEs, SegFs, and SegGs.
//
// CONTEXT_FLOATING_POINT specifies Xmm0-Xmm15.
//
// CONTEXT_DEBUG_REGISTERS specifies Dr0-Dr3 and Dr6-Dr7.
//

typedef struct DECLSPEC_ALIGN(16) _CONTEXT {
    //
    // Register parameter home addresses.
    //
    // N.B. These fields are for convience - they could be used to extend the
    //      context record in the future.
    //

    DWORD64 P1Home;
    DWORD64 P2Home;
    DWORD64 P3Home;
    DWORD64 P4Home;
    DWORD64 P5Home;
    DWORD64 P6Home;

    //
    // Control flags.
    //

    DWORD ContextFlags;
    DWORD MxCsr;

    //
    // Segment Registers and processor flags.
    //

    WORD   SegCs;
    WORD   SegDs;
    WORD   SegEs;
    WORD   SegFs;
    WORD   SegGs;
    WORD   SegSs;
    DWORD EFlags;

    //
    // Debug registers
    //

    DWORD64 Dr0;
    DWORD64 Dr1;
    DWORD64 Dr2;
    DWORD64 Dr3;
    DWORD64 Dr6;
    DWORD64 Dr7;

    //
    // Integer registers.
    //

    DWORD64 Rax;
    DWORD64 Rcx;
    DWORD64 Rdx;
    DWORD64 Rbx;
    DWORD64 Rsp;
    DWORD64 Rbp;
    DWORD64 Rsi;
    DWORD64 Rdi;
    DWORD64 R8;
    DWORD64 R9;
    DWORD64 R10;
    DWORD64 R11;
    DWORD64 R12;
    DWORD64 R13;
    DWORD64 R14;
    DWORD64 R15;

    //
    // Program counter.
    //

    DWORD64 Rip;

    //
    // Floating point state.
    //

    union {
        XMM_SAVE_AREA32 FltSave;
        struct {
            M128A Header[2];
            M128A Legacy[8];
            M128A Xmm0;
            M128A Xmm1;
            M128A Xmm2;
            M128A Xmm3;
            M128A Xmm4;
            M128A Xmm5;
            M128A Xmm6;
            M128A Xmm7;
            M128A Xmm8;
            M128A Xmm9;
            M128A Xmm10;
            M128A Xmm11;
            M128A Xmm12;
            M128A Xmm13;
            M128A Xmm14;
            M128A Xmm15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    //
    // Vector registers.
    //

    M128A VectorRegister[26];
    DWORD64 VectorControl;

    //
    // Special debug control registers.
    //

    DWORD64 DebugControl;
    DWORD64 LastBranchToRip;
    DWORD64 LastBranchFromRip;
    DWORD64 LastExceptionToRip;
    DWORD64 LastExceptionFromRip;
} CONTEXT, * PCONTEXT;

typedef enum _SECURITY_IMPERSONATION_LEVEL {
    SecurityAnonymous,
    SecurityIdentification,
    SecurityImpersonation,
    SecurityDelegation
} SECURITY_IMPERSONATION_LEVEL, * PSECURITY_IMPERSONATION_LEVEL;

typedef BOOLEAN SECURITY_CONTEXT_TRACKING_MODE,
* PSECURITY_CONTEXT_TRACKING_MODE;

//
// Quality Of Service
//

typedef struct _SECURITY_QUALITY_OF_SERVICE {
    DWORD Length;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    SECURITY_CONTEXT_TRACKING_MODE ContextTrackingMode;
    BOOLEAN EffectiveOnly;
} SECURITY_QUALITY_OF_SERVICE, * PSECURITY_QUALITY_OF_SERVICE;

#define SECURITY_MAX_IMPERSONATION_LEVEL SecurityDelegation
#define SECURITY_MIN_IMPERSONATION_LEVEL SecurityAnonymous
#define DEFAULT_IMPERSONATION_LEVEL SecurityImpersonation
#define VALID_IMPERSONATION_LEVEL(L) (((L) >= SECURITY_MIN_IMPERSONATION_LEVEL) && ((L) <= SECURITY_MAX_IMPERSONATION_LEVEL))

#define SECURITY_DYNAMIC_TRACKING      (TRUE)
#define SECURITY_STATIC_TRACKING       (FALSE)

typedef struct _IO_COUNTERS {
    ULONGLONG  ReadOperationCount;
    ULONGLONG  WriteOperationCount;
    ULONGLONG  OtherOperationCount;
    ULONGLONG ReadTransferCount;
    ULONGLONG WriteTransferCount;
    ULONGLONG OtherTransferCount;
} IO_COUNTERS;

typedef struct _JOB_SET_ARRAY {
    HANDLE JobHandle;   // Handle to job object to insert
    DWORD MemberLevel;  // Level of this job in the set. Must be > 0. Can be sparse.
    DWORD Flags;        // Unused. Must be zero
} JOB_SET_ARRAY, * PJOB_SET_ARRAY;

#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters
typedef struct _EXCEPTION_RECORD {
    DWORD    ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD* ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} EXCEPTION_RECORD;

//
// Define access rights to files and directories
//

//
// The FILE_READ_DATA and FILE_WRITE_DATA constants are also defined in
// devioctl.h as FILE_READ_ACCESS and FILE_WRITE_ACCESS. The values for these
// constants *MUST* always be in sync.
// The values are redefined in devioctl.h because they must be available to
// both DOS and NT.
//

#define FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define FILE_LIST_DIRECTORY       ( 0x0001 )    // directory

#define FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define FILE_ADD_FILE             ( 0x0002 )    // directory

#define FILE_APPEND_DATA          ( 0x0004 )    // file
#define FILE_ADD_SUBDIRECTORY     ( 0x0004 )    // directory
#define FILE_CREATE_PIPE_INSTANCE ( 0x0004 )    // named pipe

#define FILE_READ_EA              ( 0x0008 )    // file & directory

#define FILE_WRITE_EA             ( 0x0010 )    // file & directory

#define FILE_EXECUTE              ( 0x0020 )    // file
#define FILE_TRAVERSE             ( 0x0020 )    // directory

#define FILE_DELETE_CHILD         ( 0x0040 )    // directory

#define FILE_READ_ATTRIBUTES      ( 0x0080 )    // all

#define FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // all

#define FILE_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x1FF)

#define FILE_GENERIC_READ         (STANDARD_RIGHTS_READ     |\
                                   FILE_READ_DATA           |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_READ_EA             |\
                                   SYNCHRONIZE)

#define FILE_GENERIC_WRITE        (STANDARD_RIGHTS_WRITE    |\
                                   FILE_WRITE_DATA          |\
                                   FILE_WRITE_ATTRIBUTES    |\
                                   FILE_WRITE_EA            |\
                                   FILE_APPEND_DATA         |\
                                   SYNCHRONIZE)

#define FILE_GENERIC_EXECUTE      (STANDARD_RIGHTS_EXECUTE  |\
                                   FILE_READ_ATTRIBUTES     |\
                                   FILE_EXECUTE             |\
                                   SYNCHRONIZE)

#define FILE_FLAG_OVERLAPPED            0x40000000

// end_access
#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002
#define FILE_SHARE_DELETE               0x00000004
#define FILE_ATTRIBUTE_READONLY             0x00000001
#define FILE_ATTRIBUTE_HIDDEN               0x00000002
#define FILE_ATTRIBUTE_SYSTEM               0x00000004
#define FILE_ATTRIBUTE_DIRECTORY            0x00000010
#define FILE_ATTRIBUTE_ARCHIVE              0x00000020
#define FILE_ATTRIBUTE_DEVICE               0x00000040
#define FILE_ATTRIBUTE_NORMAL               0x00000080
#define FILE_ATTRIBUTE_TEMPORARY            0x00000100
#define FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
#define FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
#define FILE_ATTRIBUTE_COMPRESSED           0x00000800
#define FILE_ATTRIBUTE_OFFLINE              0x00001000
#define FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
#define FILE_ATTRIBUTE_ENCRYPTED            0x00004000
#define FILE_ATTRIBUTE_INTEGRITY_STREAM     0x00008000
#define FILE_ATTRIBUTE_VIRTUAL              0x00010000
#define FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000
#define FILE_ATTRIBUTE_EA                   0x00040000
#define FILE_ATTRIBUTE_PINNED               0x00080000
#define FILE_ATTRIBUTE_UNPINNED             0x00100000
#define FILE_ATTRIBUTE_RECALL_ON_OPEN       0x00040000
#define FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000
#define TREE_CONNECT_ATTRIBUTE_PRIVACY      0x00004000
#define TREE_CONNECT_ATTRIBUTE_INTEGRITY    0x00008000
#define TREE_CONNECT_ATTRIBUTE_GLOBAL       0x00000004
#define TREE_CONNECT_ATTRIBUTE_PINNED       0x00000002
#define FILE_ATTRIBUTE_STRICTLY_SEQUENTIAL  0x20000000
#define FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001
#define FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002
#define FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004
#define FILE_NOTIFY_CHANGE_SIZE         0x00000008
#define FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010
#define FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020
#define FILE_NOTIFY_CHANGE_CREATION     0x00000040
#define FILE_NOTIFY_CHANGE_SECURITY     0x00000100
#define FILE_ACTION_ADDED                   0x00000001
#define FILE_ACTION_REMOVED                 0x00000002
#define FILE_ACTION_MODIFIED                0x00000003
#define FILE_ACTION_RENAMED_OLD_NAME        0x00000004
#define FILE_ACTION_RENAMED_NEW_NAME        0x00000005
#define MAILSLOT_NO_MESSAGE             ((DWORD)-1)
#define MAILSLOT_WAIT_FOREVER           ((DWORD)-1)
#define FILE_CASE_SENSITIVE_SEARCH          0x00000001
#define FILE_CASE_PRESERVED_NAMES           0x00000002
#define FILE_UNICODE_ON_DISK                0x00000004
#define FILE_PERSISTENT_ACLS                0x00000008
#define FILE_FILE_COMPRESSION               0x00000010
#define FILE_VOLUME_QUOTAS                  0x00000020
#define FILE_SUPPORTS_SPARSE_FILES          0x00000040
#define FILE_SUPPORTS_REPARSE_POINTS        0x00000080
#define FILE_SUPPORTS_REMOTE_STORAGE        0x00000100
#define FILE_RETURNS_CLEANUP_RESULT_INFO    0x00000200
#define FILE_SUPPORTS_POSIX_UNLINK_RENAME   0x00000400

#define FILE_VOLUME_IS_COMPRESSED           0x00008000
#define FILE_SUPPORTS_OBJECT_IDS            0x00010000
#define FILE_SUPPORTS_ENCRYPTION            0x00020000
#define FILE_NAMED_STREAMS                  0x00040000
#define FILE_READ_ONLY_VOLUME               0x00080000
#define FILE_SEQUENTIAL_WRITE_ONCE          0x00100000
#define FILE_SUPPORTS_TRANSACTIONS          0x00200000
#define FILE_SUPPORTS_HARD_LINKS            0x00400000
#define FILE_SUPPORTS_EXTENDED_ATTRIBUTES   0x00800000
#define FILE_SUPPORTS_OPEN_BY_FILE_ID       0x01000000
#define FILE_SUPPORTS_USN_JOURNAL           0x02000000
#define FILE_SUPPORTS_INTEGRITY_STREAMS     0x04000000
#define FILE_SUPPORTS_BLOCK_REFCOUNTING     0x08000000
#define FILE_SUPPORTS_SPARSE_VDL            0x10000000
#define FILE_DAX_VOLUME                     0x20000000
#define FILE_SUPPORTS_GHOSTING              0x40000000

#define FILE_INVALID_FILE_ID               ((LONGLONG)-1LL)
typedef struct _FILE_ID_128 {
    BYTE  Identifier[16];
} FILE_ID_128, * PFILE_ID_128;

typedef struct _SID_IDENTIFIER_AUTHORITY {
    BYTE  Value[6];
} SID_IDENTIFIER_AUTHORITY, * PSID_IDENTIFIER_AUTHORITY;

typedef struct _SID {
    BYTE  Revision;
    BYTE  SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
#ifdef MIDL_PASS
    [size_is(SubAuthorityCount)] DWORD SubAuthority[*];
#else // MIDL_PASS
    DWORD SubAuthority[ANYSIZE_ARRAY];
#endif // MIDL_PASS
} SID, * PISID;

#define DEVICE_TYPE DWORD

#define FILE_DEVICE_BEEP                0x00000001
#define FILE_DEVICE_CD_ROM              0x00000002
#define FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define FILE_DEVICE_CONTROLLER          0x00000004
#define FILE_DEVICE_DATALINK            0x00000005
#define FILE_DEVICE_DFS                 0x00000006
#define FILE_DEVICE_DISK                0x00000007
#define FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define FILE_DEVICE_FILE_SYSTEM         0x00000009
#define FILE_DEVICE_INPORT_PORT         0x0000000a
#define FILE_DEVICE_KEYBOARD            0x0000000b
#define FILE_DEVICE_MAILSLOT            0x0000000c
#define FILE_DEVICE_MIDI_IN             0x0000000d
#define FILE_DEVICE_MIDI_OUT            0x0000000e
#define FILE_DEVICE_MOUSE               0x0000000f
#define FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define FILE_DEVICE_NAMED_PIPE          0x00000011
#define FILE_DEVICE_NETWORK             0x00000012
#define FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define FILE_DEVICE_NULL                0x00000015
#define FILE_DEVICE_PARALLEL_PORT       0x00000016
#define FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define FILE_DEVICE_PRINTER             0x00000018
#define FILE_DEVICE_SCANNER             0x00000019
#define FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define FILE_DEVICE_SERIAL_PORT         0x0000001b
#define FILE_DEVICE_SCREEN              0x0000001c
#define FILE_DEVICE_SOUND               0x0000001d
#define FILE_DEVICE_STREAMS             0x0000001e
#define FILE_DEVICE_TAPE                0x0000001f
#define FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define FILE_DEVICE_TRANSPORT           0x00000021
#define FILE_DEVICE_UNKNOWN             0x00000022
#define FILE_DEVICE_VIDEO               0x00000023
#define FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define FILE_DEVICE_WAVE_IN             0x00000025
#define FILE_DEVICE_WAVE_OUT            0x00000026
#define FILE_DEVICE_8042_PORT           0x00000027
#define FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define FILE_DEVICE_BATTERY             0x00000029
#define FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define FILE_DEVICE_MODEM               0x0000002b
#define FILE_DEVICE_VDM                 0x0000002c
#define FILE_DEVICE_MASS_STORAGE        0x0000002d
#define FILE_DEVICE_SMB                 0x0000002e
#define FILE_DEVICE_KS                  0x0000002f
#define FILE_DEVICE_CHANGER             0x00000030
#define FILE_DEVICE_SMARTCARD           0x00000031
#define FILE_DEVICE_ACPI                0x00000032
#define FILE_DEVICE_DVD                 0x00000033
#define FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define FILE_DEVICE_DFS_VOLUME          0x00000036
#define FILE_DEVICE_SERENUM             0x00000037
#define FILE_DEVICE_TERMSRV             0x00000038
#define FILE_DEVICE_KSEC                0x00000039
#define FILE_DEVICE_FIPS                0x0000003A
#define FILE_DEVICE_INFINIBAND          0x0000003B
#define FILE_DEVICE_VMBUS               0x0000003E
#define FILE_DEVICE_CRYPT_PROVIDER      0x0000003F
#define FILE_DEVICE_WPD                 0x00000040
#define FILE_DEVICE_BLUETOOTH           0x00000041
#define FILE_DEVICE_MT_COMPOSITE        0x00000042
#define FILE_DEVICE_MT_TRANSPORT        0x00000043
#define FILE_DEVICE_BIOMETRIC           0x00000044
#define FILE_DEVICE_PMI                 0x00000045
#define FILE_DEVICE_EHSTOR              0x00000046
#define FILE_DEVICE_DEVAPI              0x00000047
#define FILE_DEVICE_GPIO                0x00000048
#define FILE_DEVICE_USBEX               0x00000049
#define FILE_DEVICE_CONSOLE             0x00000050
#define FILE_DEVICE_NFP                 0x00000051
#define FILE_DEVICE_SYSENV              0x00000052
#define FILE_DEVICE_VIRTUAL_BLOCK       0x00000053
#define FILE_DEVICE_POINT_OF_SERVICE    0x00000054
#define FILE_DEVICE_STORAGE_REPLICATION 0x00000055
#define FILE_DEVICE_TRUST_ENV           0x00000056
#define FILE_DEVICE_UCM                 0x00000057
#define FILE_DEVICE_UCMTCPCI            0x00000058
#define FILE_DEVICE_PERSISTENT_MEMORY   0x00000059
#define FILE_DEVICE_NVDIMM              0x0000005a
#define FILE_DEVICE_HOLOGRAPHIC         0x0000005b
#define FILE_DEVICE_SDFXHCI             0x0000005c
#define FILE_DEVICE_UCMUCSI             0x0000005d

//
// Macro definition for defining IOCTL and FSCTL function control codes.  Note
// that function codes 0-2047 are reserved for Microsoft Corporation, and
// 2048-4095 are reserved for customers.
//

#define CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

//
// Macro to extract device type out of the device io control code
//
#define DEVICE_TYPE_FROM_CTL_CODE(ctrlCode)     (((DWORD)(ctrlCode & 0xffff0000)) >> 16) //-V1003

//
// Macro to extract buffering method out of the device io control code
//
#define METHOD_FROM_CTL_CODE(ctrlCode)          ((DWORD)(ctrlCode & 3)) //-V1003

//
// Define the method codes for how buffers are passed for I/O and FS controls
//

#define METHOD_BUFFERED                 0
#define METHOD_IN_DIRECT                1
#define METHOD_OUT_DIRECT               2
#define METHOD_NEITHER                  3

//
// Define some easier to comprehend aliases:
//   METHOD_DIRECT_TO_HARDWARE (writes, aka METHOD_IN_DIRECT)
//   METHOD_DIRECT_FROM_HARDWARE (reads, aka METHOD_OUT_DIRECT)
//

#define METHOD_DIRECT_TO_HARDWARE       METHOD_IN_DIRECT
#define METHOD_DIRECT_FROM_HARDWARE     METHOD_OUT_DIRECT

//
// Define the access check value for any access
//
//
// The FILE_READ_ACCESS and FILE_WRITE_ACCESS constants are also defined in
// ntioapi.h as FILE_READ_DATA and FILE_WRITE_DATA. The values for these
// constants *MUST* always be in sync.
//
//
// FILE_SPECIAL_ACCESS is checked by the NT I/O system the same as FILE_ANY_ACCESS.
// The file systems, however, may add additional access checks for I/O and FS controls
// that use this value.
//

#define FILE_ANY_ACCESS                 0
#define FILE_SPECIAL_ACCESS    (FILE_ANY_ACCESS)
#define FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

typedef enum _SYSTEM_POWER_STATE {
    PowerSystemUnspecified = 0,
    PowerSystemWorking = 1,
    PowerSystemSleeping1 = 2,
    PowerSystemSleeping2 = 3,
    PowerSystemSleeping3 = 4,
    PowerSystemHibernate = 5,
    PowerSystemShutdown = 6,
    PowerSystemMaximum = 7
} SYSTEM_POWER_STATE, * PSYSTEM_POWER_STATE;

typedef enum _DEVICE_POWER_STATE {
    PowerDeviceUnspecified = 0,
    PowerDeviceD0,
    PowerDeviceD1,
    PowerDeviceD2,
    PowerDeviceD3,
    PowerDeviceMaximum
} DEVICE_POWER_STATE, * PDEVICE_POWER_STATE;

#define POINTER_64 __ptr64

typedef void* POINTER_64 PVOID64;

typedef union _FILE_SEGMENT_ELEMENT {
    PVOID64 Buffer;
    ULONGLONG Alignment;
}FILE_SEGMENT_ELEMENT, * PFILE_SEGMENT_ELEMENT;

typedef enum {
    SystemPowerPolicyAc,
    SystemPowerPolicyDc,
    VerifySystemPolicyAc,
    VerifySystemPolicyDc,
    SystemPowerCapabilities,
    SystemBatteryState,
    SystemPowerStateHandler,
    ProcessorStateHandler,
    SystemPowerPolicyCurrent,
    AdministratorPowerPolicy,
    SystemReserveHiberFile,
    ProcessorInformation,
    SystemPowerInformation,
    ProcessorStateHandler2,
    LastWakeTime,                                   // Compare with KeQueryInterruptTime()
    LastSleepTime,                                  // Compare with KeQueryInterruptTime()
    SystemExecutionState,
    SystemPowerStateNotifyHandler,
    ProcessorPowerPolicyAc,
    ProcessorPowerPolicyDc,
    VerifyProcessorPowerPolicyAc,
    VerifyProcessorPowerPolicyDc,
    ProcessorPowerPolicyCurrent,
    SystemPowerStateLogging,
    SystemPowerLoggingEntry,
    SetPowerSettingValue,
    NotifyUserPowerSetting,
    PowerInformationLevelUnused0,
    SystemMonitorHiberBootPowerOff,
    SystemVideoState,
    TraceApplicationPowerMessage,
    TraceApplicationPowerMessageEnd,
    ProcessorPerfStates,
    ProcessorIdleStates,
    ProcessorCap,
    SystemWakeSource,
    SystemHiberFileInformation,
    TraceServicePowerMessage,
    ProcessorLoad,
    PowerShutdownNotification,
    MonitorCapabilities,
    SessionPowerInit,
    SessionDisplayState,
    PowerRequestCreate,
    PowerRequestAction,
    GetPowerRequestList,
    ProcessorInformationEx,
    NotifyUserModeLegacyPowerEvent,
    GroupPark,
    ProcessorIdleDomains,
    WakeTimerList,
    SystemHiberFileSize,
    ProcessorIdleStatesHv,
    ProcessorPerfStatesHv,
    ProcessorPerfCapHv,
    ProcessorSetIdle,
    LogicalProcessorIdling,
    UserPresence,
    PowerSettingNotificationName,
    GetPowerSettingValue,
    IdleResiliency,
    SessionRITState,
    SessionConnectNotification,
    SessionPowerCleanup,
    SessionLockState,
    SystemHiberbootState,
    PlatformInformation,
    PdcInvocation,
    MonitorInvocation,
    FirmwareTableInformationRegistered,
    SetShutdownSelectedTime,
    SuspendResumeInvocation,
    PlmPowerRequestCreate,
    ScreenOff,
    CsDeviceNotification,
    PlatformRole,
    LastResumePerformance,
    DisplayBurst,
    ExitLatencySamplingPercentage,
    RegisterSpmPowerSettings,
    PlatformIdleStates,
    ProcessorIdleVeto,                              // Deprecated.
    PlatformIdleVeto,                               // Deprecated.
    SystemBatteryStatePrecise,
    ThermalEvent,
    PowerRequestActionInternal,
    BatteryDeviceState,
    PowerInformationInternal,
    ThermalStandby,
    SystemHiberFileType,
    PhysicalPowerButtonPress,
    QueryPotentialDripsConstraint,
    EnergyTrackerCreate,
    EnergyTrackerQuery,
    UpdateBlackBoxRecorder,
    PowerInformationLevelMaximum
} POWER_INFORMATION_LEVEL;

typedef DWORD EXECUTION_STATE, * PEXECUTION_STATE;

typedef enum {
    LT_DONT_CARE,
    LT_LOWEST_LATENCY
} LATENCY_TIME;

typedef enum {
    PowerActionNone = 0,
    PowerActionReserved,
    PowerActionSleep,
    PowerActionHibernate,
    PowerActionShutdown,
    PowerActionShutdownReset,
    PowerActionShutdownOff,
    PowerActionWarmEject,
    PowerActionDisplayOff
} POWER_ACTION, * PPOWER_ACTION;

#define NULL 0

#define UNREFERENCED_PARAMETER(P)          (P)
#define DBG_UNREFERENCED_PARAMETER(P)      (P)
#define DBG_UNREFERENCED_LOCAL_VARIABLE(V) (V)

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION* CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD   CreatorBackTraceIndexHigh;
    WORD   SpareWORD;
} RTL_CRITICAL_SECTION_DEBUG, * PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, * PRTL_RESOURCE_DEBUG;

//
// These flags define the upper byte of the critical section SpinCount field
//
#define RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO         0x01000000
#define RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN          0x02000000
#define RTL_CRITICAL_SECTION_FLAG_STATIC_INIT           0x04000000
#define RTL_CRITICAL_SECTION_FLAG_RESOURCE_TYPE         0x08000000
#define RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO      0x10000000
#define RTL_CRITICAL_SECTION_ALL_FLAG_BITS              0xFF000000
#define RTL_CRITICAL_SECTION_FLAG_RESERVED              (RTL_CRITICAL_SECTION_ALL_FLAG_BITS & (~(RTL_CRITICAL_SECTION_FLAG_NO_DEBUG_INFO | RTL_CRITICAL_SECTION_FLAG_DYNAMIC_SPIN | RTL_CRITICAL_SECTION_FLAG_STATIC_INIT | RTL_CRITICAL_SECTION_FLAG_RESOURCE_TYPE | RTL_CRITICAL_SECTION_FLAG_FORCE_DEBUG_INFO)))

//
// These flags define possible values stored in the Flags field of a critsec debuginfo.
//
#define RTL_CRITICAL_SECTION_DEBUG_FLAG_STATIC_INIT     0x00000001

#pragma pack(push, 8)

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION, * PRTL_CRITICAL_SECTION;

#pragma pack(pop)

typedef struct _RTL_SRWLOCK {
    PVOID Ptr;
} RTL_SRWLOCK, * PRTL_SRWLOCK;
#define RTL_SRWLOCK_INIT {0}
typedef struct _RTL_CONDITION_VARIABLE {
    PVOID Ptr;
} RTL_CONDITION_VARIABLE, * PRTL_CONDITION_VARIABLE;
#define RTL_CONDITION_VARIABLE_INIT {0}
#define RTL_CONDITION_VARIABLE_LOCKMODE_SHARED  0x1

typedef struct _RTL_BARRIER {
    DWORD Reserved1;
    DWORD Reserved2;
    ULONG_PTR Reserved3[2];
    DWORD Reserved4;
    DWORD Reserved5;
} RTL_BARRIER, * PRTL_BARRIER;

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                         ACL  and  ACE                              //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//
//  Define an ACL and the ACE format.  The structure of an ACL header
//  followed by one or more ACEs.  Pictorally the structure of an ACL header
//  is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +-------------------------------+---------------+---------------+
//      |            AclSize            |      Sbz1     |  AclRevision  |
//      +-------------------------------+---------------+---------------+
//      |              Sbz2             |           AceCount            |
//      +-------------------------------+-------------------------------+
//
//  The current AclRevision is defined to be ACL_REVISION.
//
//  AclSize is the size, in bytes, allocated for the ACL.  This includes
//  the ACL header, ACES, and remaining free space in the buffer.
//
//  AceCount is the number of ACES in the ACL.
//

// begin_wdm
// This is the *current* ACL revision

#define ACL_REVISION     (2)
#define ACL_REVISION_DS  (4)

// This is the history of ACL revisions.  Add a new one whenever
// ACL_REVISION is updated

#define ACL_REVISION1   (1)
#define MIN_ACL_REVISION ACL_REVISION2
#define ACL_REVISION2   (2)
#define ACL_REVISION3   (3)
#define ACL_REVISION4   (4)
#define MAX_ACL_REVISION ACL_REVISION4

typedef struct _ACL {
    BYTE  AclRevision;
    BYTE  Sbz1;
    WORD   AclSize;
    WORD   AceCount;
    WORD   Sbz2;
} ACL;
typedef ACL* PACL;

// end_wdm
// begin_ntifs

//
//  The structure of an ACE is a common ace header followed by ace type
//  specific data.  Pictorally the structure of the common ace header is
//  as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------+-------+-------+---------------+---------------+
//      |            AceSize            |    AceFlags   |     AceType   |
//      +---------------+-------+-------+---------------+---------------+
//
//  AceType denotes the type of the ace, there are some predefined ace
//  types
//
//  AceSize is the size, in bytes, of ace.
//
//  AceFlags are the Ace flags for audit and inheritance, defined shortly.

typedef struct _ACE_HEADER {
    BYTE  AceType;
    BYTE  AceFlags;
    WORD   AceSize;
} ACE_HEADER;
typedef ACE_HEADER* PACE_HEADER;

//
//  The following are the predefined ace types that go into the AceType
//  field of an Ace header.
//

#define ACCESS_MIN_MS_ACE_TYPE                  (0x0)
#define ACCESS_ALLOWED_ACE_TYPE                 (0x0)
#define ACCESS_DENIED_ACE_TYPE                  (0x1)
#define SYSTEM_AUDIT_ACE_TYPE                   (0x2)
#define SYSTEM_ALARM_ACE_TYPE                   (0x3)
#define ACCESS_MAX_MS_V2_ACE_TYPE               (0x3)

#define ACCESS_ALLOWED_COMPOUND_ACE_TYPE        (0x4)
#define ACCESS_MAX_MS_V3_ACE_TYPE               (0x4)

#define ACCESS_MIN_MS_OBJECT_ACE_TYPE           (0x5)
#define ACCESS_ALLOWED_OBJECT_ACE_TYPE          (0x5)
#define ACCESS_DENIED_OBJECT_ACE_TYPE           (0x6)
#define SYSTEM_AUDIT_OBJECT_ACE_TYPE            (0x7)
#define SYSTEM_ALARM_OBJECT_ACE_TYPE            (0x8)
#define ACCESS_MAX_MS_OBJECT_ACE_TYPE           (0x8)

#define ACCESS_MAX_MS_V4_ACE_TYPE               (0x8)
#define ACCESS_MAX_MS_ACE_TYPE                  (0x8)

#define ACCESS_ALLOWED_CALLBACK_ACE_TYPE        (0x9)
#define ACCESS_DENIED_CALLBACK_ACE_TYPE         (0xA)
#define ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE (0xB)
#define ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE  (0xC)
#define SYSTEM_AUDIT_CALLBACK_ACE_TYPE          (0xD)
#define SYSTEM_ALARM_CALLBACK_ACE_TYPE          (0xE)
#define SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE   (0xF)
#define SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE   (0x10)

#define SYSTEM_MANDATORY_LABEL_ACE_TYPE         (0x11)
#define SYSTEM_RESOURCE_ATTRIBUTE_ACE_TYPE      (0x12)
#define SYSTEM_SCOPED_POLICY_ID_ACE_TYPE        (0x13)
#define SYSTEM_PROCESS_TRUST_LABEL_ACE_TYPE     (0x14)
#define SYSTEM_ACCESS_FILTER_ACE_TYPE           (0x15)
#define ACCESS_MAX_MS_V5_ACE_TYPE               (0x15)

//
//  The following are the inherit flags that go into the AceFlags field
//  of an Ace header.
//

#define OBJECT_INHERIT_ACE                (0x1)
#define CONTAINER_INHERIT_ACE             (0x2)
#define NO_PROPAGATE_INHERIT_ACE          (0x4)
#define INHERIT_ONLY_ACE                  (0x8)
#define INHERITED_ACE                     (0x10)
#define VALID_INHERIT_FLAGS               (0x1F)

//  The following are the currently defined ACE flags that go into the
//  AceFlags field of an ACE header.  Each ACE type has its own set of
//  AceFlags.
//
//

//
// ACCESS_ALLOWED_ACE_TYPE
//
// These control whether the ACE is critical and cannot be removed.
//
// CRITICAL_ACE_FLAG - used only with access allowed ACE types to
// indicate that the ACE is critical and cannot be removed.
//

#define CRITICAL_ACE_FLAG              (0x20)

//
//  SYSTEM_AUDIT and SYSTEM_ALARM AceFlags
//
//  These control the signaling of audit and alarms for success or failure.
//
//  SUCCESSFUL_ACCESS_ACE_FLAG - used only with system audit and alarm ACE
//  types to indicate that a message is generated for successful accesses.
//
//  FAILED_ACCESS_ACE_FLAG - used only with system audit and alarm ACE types
//  to indicate that a message is generated for failed accesses.
//


//
//  SYSTEM_ACCESS_FILTER_ACE AceFlags
//
//  These control the behaviour of SYSTEM_ACCESS_FILTER_ACE .
//
//  TRUST_PROTECTED_FILTER_ACE_FLAG - used only with SYSTEM_FILTERING_ACE_TYPE
//  ACEs to indicate that this ACE may not be deleted/modified except when the,
//  the current Trust Level dominates the one specified in the Ace SID.
//  If this flag is set then the SID in the ACE should be a valid TrustLevelSid.
//

#define TRUST_PROTECTED_FILTER_ACE_FLAG  (0x40)


//
//  We'll define the structure of the predefined ACE types.  Pictorally
//  the structure of the predefined ACE's is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------+-------+-------+---------------+---------------+
//      |    AceFlags   | Resd  |Inherit|    AceSize    |     AceType   |
//      +---------------+-------+-------+---------------+---------------+
//      |                              Mask                             |
//      +---------------------------------------------------------------+
//      |                                                               |
//      +                                                               +
//      |                                                               |
//      +                              Sid                              +
//      |                                                               |
//      +                                                               +
//      |                                                               |
//      +---------------------------------------------------------------+
//
//  Mask is the access mask associated with the ACE.  This is either the
//  access allowed, access denied, audit, or alarm mask.
//
//  Sid is the Sid associated with the ACE.
//

//  The following are the four predefined ACE types.

//  Examine the AceType field in the Header to determine
//  which structure is appropriate to use for casting.


typedef struct _ACCESS_ALLOWED_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} ACCESS_ALLOWED_ACE;

typedef ACCESS_ALLOWED_ACE* PACCESS_ALLOWED_ACE;

typedef struct _ACCESS_DENIED_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} ACCESS_DENIED_ACE;
typedef ACCESS_DENIED_ACE* PACCESS_DENIED_ACE;

typedef struct _SYSTEM_AUDIT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} SYSTEM_AUDIT_ACE;
typedef SYSTEM_AUDIT_ACE* PSYSTEM_AUDIT_ACE;

typedef struct _SYSTEM_ALARM_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} SYSTEM_ALARM_ACE;
typedef SYSTEM_ALARM_ACE* PSYSTEM_ALARM_ACE;

typedef struct _SYSTEM_RESOURCE_ATTRIBUTE_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Sid followed by CLAIM_SECURITY_ATTRIBUTE_RELATIVE_V1 structure
} SYSTEM_RESOURCE_ATTRIBUTE_ACE, * PSYSTEM_RESOURCE_ATTRIBUTE_ACE;

typedef struct _SYSTEM_SCOPED_POLICY_ID_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} SYSTEM_SCOPED_POLICY_ID_ACE, * PSYSTEM_SCOPED_POLICY_ID_ACE;

typedef struct _SYSTEM_MANDATORY_LABEL_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} SYSTEM_MANDATORY_LABEL_ACE, * PSYSTEM_MANDATORY_LABEL_ACE;

typedef struct _SYSTEM_PROCESS_TRUST_LABEL_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
} SYSTEM_PROCESS_TRUST_LABEL_ACE, * PSYSTEM_PROCESS_TRUST_LABEL_ACE;

typedef struct _SYSTEM_ACCESS_FILTER_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Filter Condition follows the SID
} SYSTEM_ACCESS_FILTER_ACE, * PSYSTEM_ACCESS_FILTER_ACE;

#define SYSTEM_MANDATORY_LABEL_NO_WRITE_UP         0x1
#define SYSTEM_MANDATORY_LABEL_NO_READ_UP          0x2
#define SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP       0x4

#define SYSTEM_MANDATORY_LABEL_VALID_MASK (SYSTEM_MANDATORY_LABEL_NO_WRITE_UP   | \
                                           SYSTEM_MANDATORY_LABEL_NO_READ_UP    | \
                                           SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP)

// Placeholder value that allows all ranges
#define SYSTEM_PROCESS_TRUST_LABEL_VALID_MASK      0x00ffffff
#define SYSTEM_PROCESS_TRUST_NOCONSTRAINT_MASK     0xffffffff
#define SYSTEM_ACCESS_FILTER_VALID_MASK            0x00ffffff
#define SYSTEM_ACCESS_FILTER_NOCONSTRAINT_MASK     0xffffffff
// end_ntifs


typedef struct _ACCESS_ALLOWED_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
} ACCESS_ALLOWED_OBJECT_ACE, * PACCESS_ALLOWED_OBJECT_ACE;

typedef struct _ACCESS_DENIED_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
} ACCESS_DENIED_OBJECT_ACE, * PACCESS_DENIED_OBJECT_ACE;

typedef struct _SYSTEM_AUDIT_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
} SYSTEM_AUDIT_OBJECT_ACE, * PSYSTEM_AUDIT_OBJECT_ACE;

typedef struct _SYSTEM_ALARM_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
} SYSTEM_ALARM_OBJECT_ACE, * PSYSTEM_ALARM_OBJECT_ACE;

//
// Callback ace support in post Win2000.
// Resource managers can put their own data after Sidstart + Length of the sid
//

typedef struct _ACCESS_ALLOWED_CALLBACK_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Opaque resource manager specific data
} ACCESS_ALLOWED_CALLBACK_ACE, * PACCESS_ALLOWED_CALLBACK_ACE;

typedef struct _ACCESS_DENIED_CALLBACK_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Opaque resource manager specific data
} ACCESS_DENIED_CALLBACK_ACE, * PACCESS_DENIED_CALLBACK_ACE;

typedef struct _SYSTEM_AUDIT_CALLBACK_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Opaque resource manager specific data
} SYSTEM_AUDIT_CALLBACK_ACE, * PSYSTEM_AUDIT_CALLBACK_ACE;

typedef struct _SYSTEM_ALARM_CALLBACK_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD SidStart;
    // Opaque resource manager specific data
} SYSTEM_ALARM_CALLBACK_ACE, * PSYSTEM_ALARM_CALLBACK_ACE;

typedef struct _ACCESS_ALLOWED_CALLBACK_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
    // Opaque resource manager specific data
} ACCESS_ALLOWED_CALLBACK_OBJECT_ACE, * PACCESS_ALLOWED_CALLBACK_OBJECT_ACE;

typedef struct _ACCESS_DENIED_CALLBACK_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
    // Opaque resource manager specific data
} ACCESS_DENIED_CALLBACK_OBJECT_ACE, * PACCESS_DENIED_CALLBACK_OBJECT_ACE;

typedef struct _SYSTEM_AUDIT_CALLBACK_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
    // Opaque resource manager specific data
} SYSTEM_AUDIT_CALLBACK_OBJECT_ACE, * PSYSTEM_AUDIT_CALLBACK_OBJECT_ACE;

typedef struct _SYSTEM_ALARM_CALLBACK_OBJECT_ACE {
    ACE_HEADER Header;
    ACCESS_MASK Mask;
    DWORD Flags;
    GUID ObjectType;
    GUID InheritedObjectType;
    DWORD SidStart;
    // Opaque resource manager specific data
} SYSTEM_ALARM_CALLBACK_OBJECT_ACE, * PSYSTEM_ALARM_CALLBACK_OBJECT_ACE;

//
// Currently define Flags for "OBJECT" ACE types.
//

#define ACE_OBJECT_TYPE_PRESENT           0x1
#define ACE_INHERITED_OBJECT_TYPE_PRESENT 0x2


//
//  The following declarations are used for setting and querying information
//  about and ACL.  First are the various information classes available to
//  the user.
//

#define SUCCESSFUL_ACCESS_ACE_FLAG       (0x40)
#define FAILED_ACCESS_ACE_FLAG           (0x80)

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                             SECURITY_DESCRIPTOR                    //
//                                                                    //
////////////////////////////////////////////////////////////////////////
//
//  Define the Security Descriptor and related data types.
//  This is an opaque data structure.
//

// begin_wdm
//
// Current security descriptor revision value
//

#define SECURITY_DESCRIPTOR_REVISION     (1)
#define SECURITY_DESCRIPTOR_REVISION1    (1)

// end_wdm
// begin_ntifs

#define SECURITY_DESCRIPTOR_MIN_LENGTH   (sizeof(SECURITY_DESCRIPTOR))


typedef WORD   SECURITY_DESCRIPTOR_CONTROL, * PSECURITY_DESCRIPTOR_CONTROL;

#define SE_OWNER_DEFAULTED               (0x0001)
#define SE_GROUP_DEFAULTED               (0x0002)
#define SE_DACL_PRESENT                  (0x0004)
#define SE_DACL_DEFAULTED                (0x0008)
#define SE_SACL_PRESENT                  (0x0010)
#define SE_SACL_DEFAULTED                (0x0020)
#define SE_DACL_AUTO_INHERIT_REQ         (0x0100)
#define SE_SACL_AUTO_INHERIT_REQ         (0x0200)
#define SE_DACL_AUTO_INHERITED           (0x0400)
#define SE_SACL_AUTO_INHERITED           (0x0800)
#define SE_DACL_PROTECTED                (0x1000)
#define SE_SACL_PROTECTED                (0x2000)
#define SE_RM_CONTROL_VALID              (0x4000)
#define SE_SELF_RELATIVE                 (0x8000)

//
//  Where:
//
//      SE_OWNER_DEFAULTED - This boolean flag, when set, indicates that the
//          SID pointed to by the Owner field was provided by a
//          defaulting mechanism rather than explicitly provided by the
//          original provider of the security descriptor.  This may
//          affect the treatment of the SID with respect to inheritence
//          of an owner.
//
//      SE_GROUP_DEFAULTED - This boolean flag, when set, indicates that the
//          SID in the Group field was provided by a defaulting mechanism
//          rather than explicitly provided by the original provider of
//          the security descriptor.  This may affect the treatment of
//          the SID with respect to inheritence of a primary group.
//
//      SE_DACL_PRESENT - This boolean flag, when set, indicates that the
//          security descriptor contains a discretionary ACL.  If this
//          flag is set and the Dacl field of the SECURITY_DESCRIPTOR is
//          null, then a null ACL is explicitly being specified.
//
//      SE_DACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the DaclPresent flag is not set.
//
//      SE_SACL_PRESENT - This boolean flag, when set,  indicates that the
//          security descriptor contains a system ACL pointed to by the
//          Sacl field.  If this flag is set and the Sacl field of the
//          SECURITY_DESCRIPTOR is null, then an empty (but present)
//          ACL is being specified.
//
//      SE_SACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Sacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the SaclPresent flag is not set.
//
//      SE_SELF_RELATIVE - This boolean flag, when set, indicates that the
//          security descriptor is in self-relative form.  In this form,
//          all fields of the security descriptor are contiguous in memory
//          and all pointer fields are expressed as offsets from the
//          beginning of the security descriptor.  This form is useful
//          for treating security descriptors as opaque data structures
//          for transmission in communication protocol or for storage on
//          secondary media.
//
//
//
// Pictorially the structure of a security descriptor is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------------------------------------------------------+
//      |            Control            |Reserved1 (SBZ)|   Revision    |
//      +---------------------------------------------------------------+
//      |                            Owner                              |
//      +---------------------------------------------------------------+
//      |                            Group                              |
//      +---------------------------------------------------------------+
//      |                            Sacl                               |
//      +---------------------------------------------------------------+
//      |                            Dacl                               |
//      +---------------------------------------------------------------+
//
// In general, this data structure should be treated opaquely to ensure future
// compatibility.
//
//

typedef struct _SECURITY_DESCRIPTOR_RELATIVE {
    BYTE  Revision;
    BYTE  Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    DWORD Owner;
    DWORD Group;
    DWORD Sacl;
    DWORD Dacl;
} SECURITY_DESCRIPTOR_RELATIVE, * PISECURITY_DESCRIPTOR_RELATIVE;

typedef struct _SECURITY_DESCRIPTOR {
    BYTE  Revision;
    BYTE  Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    PSID Owner;
    PSID Group;
    PACL Sacl;
    PACL Dacl;

} SECURITY_DESCRIPTOR, * PSECURITY_DESCRIPTOR;

typedef enum _ACL_INFORMATION_CLASS {
    AclRevisionInformation = 1,
    AclSizeInformation
} ACL_INFORMATION_CLASS;

typedef GUID* PGUID;

typedef enum _HARDWARE_COUNTER_TYPE {
    PMCCounter,
    MaxHardwareCounterType
} HARDWARE_COUNTER_TYPE, * PHARDWARE_COUNTER_TYPE;

extern "C" NTSYSCALLAPI size_t NTAPI strlen(const char* str);
extern "C" NTSYSCALLAPI size_t NTAPI wcslen(const wchar_t* wcs);

#define DECLSPEC_NORETURN   __declspec(noreturn)

#define STDAPICALLTYPE          __stdcall
#define STDAPIVCALLTYPE         __cdecl

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define WOW64_SIZE_OF_80387_REGISTERS      80

#define WOW64_MAXIMUM_SUPPORTED_EXTENSION     512

typedef struct _WOW64_FLOATING_SAVE_AREA {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[WOW64_SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} WOW64_FLOATING_SAVE_AREA;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//
#pragma pack(push,4)
typedef struct _WOW64_CONTEXT {
    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    DWORD ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    WOW64_FLOATING_SAVE_AREA FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    DWORD   Esp;
    DWORD   SegSs;

    //
    // This section is specified/returned if the ContextFlags word
    // contains the flag CONTEXT_EXTENDED_REGISTERS.
    // The format and contexts are processor specific
    //

    BYTE    ExtendedRegisters[WOW64_MAXIMUM_SUPPORTED_EXTENSION];
} WOW64_CONTEXT;

typedef WOW64_CONTEXT* PWOW64_CONTEXT;

typedef EXCEPTION_RECORD* PEXCEPTION_RECORD;

#pragma pack(pop)

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT ContextRecord;
} EXCEPTION_POINTERS, * PEXCEPTION_POINTERS;

typedef LONG(NTAPI* PVECTORED_EXCEPTION_HANDLER)(
    struct _EXCEPTION_POINTERS* ExceptionInfo
    );

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY {
    DWORD BeginAddress;
    DWORD EndAddress;
    union {
        DWORD UnwindInfoAddress;
        DWORD UnwindData;
    } DUMMYUNIONNAME;
} _IMAGE_RUNTIME_FUNCTION_ENTRY, * _PIMAGE_RUNTIME_FUNCTION_ENTRY;

//
// Scope table structure definition.
//

typedef struct _SCOPE_TABLE_AMD64 {
    DWORD Count;
    struct {
        DWORD BeginAddress;
        DWORD EndAddress;
        DWORD HandlerAddress;
        DWORD JumpTarget;
    } ScopeRecord[1];
} SCOPE_TABLE_AMD64, * PSCOPE_TABLE_AMD64;

typedef struct _IMAGE_RUNTIME_FUNCTION_ENTRY RUNTIME_FUNCTION, * PRUNTIME_FUNCTION;
typedef SCOPE_TABLE_AMD64 SCOPE_TABLE, * PSCOPE_TABLE;

typedef
_Function_class_(GET_RUNTIME_FUNCTION_CALLBACK)
PRUNTIME_FUNCTION
GET_RUNTIME_FUNCTION_CALLBACK(
    _In_ DWORD64 ControlPc,
    _In_opt_ PVOID Context
);
typedef GET_RUNTIME_FUNCTION_CALLBACK* PGET_RUNTIME_FUNCTION_CALLBACK;

typedef struct _IMAGE_RELOC {
    union {
        struct {
            WORD wOffset : 12;
            WORD wType : 4;
        };
        WORD wData;
    };
} IMAGE_RELOC, * PIMAGE_RELOC;

typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;

typedef struct _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, * PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES    16

typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD        Magic;
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
    ULONGLONG   ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    DWORD       LoaderFlags;
    DWORD       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, * PIMAGE_OPTIONAL_HEADER64;

typedef struct _IMAGE_NT_HEADERS64 {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, * PIMAGE_NT_HEADERS64;

typedef IMAGE_NT_HEADERS64                  IMAGE_NT_HEADERS;
typedef PIMAGE_NT_HEADERS64                 PIMAGE_NT_HEADERS;

// Subsystem Values

#define IMAGE_SUBSYSTEM_UNKNOWN              0   // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE               1   // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI          2   // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI          3   // Image runs in the Windows character subsystem.
#define IMAGE_SUBSYSTEM_OS2_CUI              5   // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI            7   // image runs in the Posix character subsystem.
#define IMAGE_SUBSYSTEM_NATIVE_WINDOWS       8   // image is a native Win9x driver.
#define IMAGE_SUBSYSTEM_WINDOWS_CE_GUI       9   // Image runs in the Windows CE subsystem.
#define IMAGE_SUBSYSTEM_EFI_APPLICATION      10  //
#define IMAGE_SUBSYSTEM_EFI_BOOT_SERVICE_DRIVER  11   //
#define IMAGE_SUBSYSTEM_EFI_RUNTIME_DRIVER   12  //
#define IMAGE_SUBSYSTEM_EFI_ROM              13
#define IMAGE_SUBSYSTEM_XBOX                 14
#define IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION 16
#define IMAGE_SUBSYSTEM_XBOX_CODE_CATALOG    17

// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

//
// x64 relocations
//
#define IMAGE_REL_AMD64_ABSOLUTE        0x0000  // Reference is absolute, no relocation is necessary
#define IMAGE_REL_AMD64_ADDR64          0x0001  // 64-bit address (VA).
#define IMAGE_REL_AMD64_ADDR32          0x0002  // 32-bit address (VA).
#define IMAGE_REL_AMD64_ADDR32NB        0x0003  // 32-bit address w/o image base (RVA).
#define IMAGE_REL_AMD64_REL32           0x0004  // 32-bit relative address from byte following reloc
#define IMAGE_REL_AMD64_REL32_1         0x0005  // 32-bit relative address from byte distance 1 from reloc
#define IMAGE_REL_AMD64_REL32_2         0x0006  // 32-bit relative address from byte distance 2 from reloc
#define IMAGE_REL_AMD64_REL32_3         0x0007  // 32-bit relative address from byte distance 3 from reloc
#define IMAGE_REL_AMD64_REL32_4         0x0008  // 32-bit relative address from byte distance 4 from reloc
#define IMAGE_REL_AMD64_REL32_5         0x0009  // 32-bit relative address from byte distance 5 from reloc
#define IMAGE_REL_AMD64_SECTION         0x000A  // Section index
#define IMAGE_REL_AMD64_SECREL          0x000B  // 32 bit offset from base of section containing target
#define IMAGE_REL_AMD64_SECREL7         0x000C  // 7 bit unsigned offset from base of section containing target
#define IMAGE_REL_AMD64_TOKEN           0x000D  // 32 bit metadata token
#define IMAGE_REL_AMD64_SREL32          0x000E  // 32 bit signed span-dependent value emitted into object
#define IMAGE_REL_AMD64_PAIR            0x000F
#define IMAGE_REL_AMD64_SSPAN32         0x0010  // 32 bit signed span-dependent value applied at link time
#define IMAGE_REL_AMD64_EHANDLER        0x0011
#define IMAGE_REL_AMD64_IMPORT_BR       0x0012  // Indirect branch to an import
#define IMAGE_REL_AMD64_IMPORT_CALL     0x0013  // Indirect call to an import
#define IMAGE_REL_AMD64_CFG_BR          0x0014  // Indirect branch to a CFG check
#define IMAGE_REL_AMD64_CFG_BR_REX      0x0015  // Indirect branch to a CFG check, with REX.W prefix
#define IMAGE_REL_AMD64_CFG_CALL        0x0016  // Indirect call to a CFG check
#define IMAGE_REL_AMD64_INDIR_BR        0x0017  // Indirect branch to a target in RAX (no CFG)
#define IMAGE_REL_AMD64_INDIR_BR_REX    0x0018  // Indirect branch to a target in RAX, with REX.W prefix (no CFG)
#define IMAGE_REL_AMD64_INDIR_CALL      0x0019  // Indirect call to a target in RAX (no CFG)
#define IMAGE_REL_AMD64_INDIR_BR_SWITCHTABLE_FIRST  0x0020 // Indirect branch for a switch table using Reg 0 (RAX)
#define IMAGE_REL_AMD64_INDIR_BR_SWITCHTABLE_LAST   0x002F // Indirect branch for a switch table using Reg 15 (R15)

//
// Based relocation format.
//

//@[comment("MVI_tracked")]
typedef struct _IMAGE_BASE_RELOCATION {
    DWORD   VirtualAddress;
    DWORD   SizeOfBlock;
    //  WORD    TypeOffset[1];
} IMAGE_BASE_RELOCATION;
typedef IMAGE_BASE_RELOCATION UNALIGNED* PIMAGE_BASE_RELOCATION;

//
// Based relocation types.
//

#define IMAGE_REL_BASED_ABSOLUTE              0
#define IMAGE_REL_BASED_HIGH                  1
#define IMAGE_REL_BASED_LOW                   2
#define IMAGE_REL_BASED_HIGHLOW               3
#define IMAGE_REL_BASED_HIGHADJ               4
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_5    5
#define IMAGE_REL_BASED_RESERVED              6
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_7    7
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_8    8
#define IMAGE_REL_BASED_MACHINE_SPECIFIC_9    9
#define IMAGE_REL_BASED_DIR64                 10

//
// Platform-specific based relocation types.
//

#define IMAGE_REL_BASED_IA64_IMM64            9

#define IMAGE_REL_BASED_MIPS_JMPADDR          5
#define IMAGE_REL_BASED_MIPS_JMPADDR16        9

#define IMAGE_REL_BASED_ARM_MOV32             5
#define IMAGE_REL_BASED_THUMB_MOV32           7

//
// Section header format.
//

#define IMAGE_SIZEOF_SHORT_NAME              8

typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
        DWORD   PhysicalAddress;
        DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, * PIMAGE_SECTION_HEADER;

//
// DLL support.
//

//
// Export Format
//

//@[comment("MVI_tracked")]
typedef struct _IMAGE_EXPORT_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   Name;
    DWORD   Base;
    DWORD   NumberOfFunctions;
    DWORD   NumberOfNames;
    DWORD   AddressOfFunctions;     // RVA from base of image
    DWORD   AddressOfNames;         // RVA from base of image
    DWORD   AddressOfNameOrdinals;  // RVA from base of image
} IMAGE_EXPORT_DIRECTORY, * PIMAGE_EXPORT_DIRECTORY;

//@[comment("MVI_tracked")]
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;            // 0 for terminating null import descriptor
        DWORD   OriginalFirstThunk;         // RVA to original unbound IAT (PIMAGE_THUNK_DATA)
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;                  // 0 if not bound,
                                            // -1 if bound, and real date\time stamp
                                            //     in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
                                            // O.W. date/time stamp of DLL bound to (Old BIND)

    DWORD   ForwarderChain;                 // -1 if no forwarders
    DWORD   Name;
    DWORD   FirstThunk;                     // RVA to IAT (if bound this IAT has actual addresses)
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED* PIMAGE_IMPORT_DESCRIPTOR;

//
// New format import descriptors pointed to by DataDirectory[ IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT ]
//

typedef struct _IMAGE_BOUND_IMPORT_DESCRIPTOR {
    DWORD   TimeDateStamp;
    WORD    OffsetModuleName;
    WORD    NumberOfModuleForwarderRefs;
    // Array of zero or more IMAGE_BOUND_FORWARDER_REF follows
} IMAGE_BOUND_IMPORT_DESCRIPTOR, * PIMAGE_BOUND_IMPORT_DESCRIPTOR;

typedef struct _IMAGE_BOUND_FORWARDER_REF {
    DWORD   TimeDateStamp;
    WORD    OffsetModuleName;
    WORD    Reserved;
} IMAGE_BOUND_FORWARDER_REF, * PIMAGE_BOUND_FORWARDER_REF;

typedef struct _IMAGE_DELAYLOAD_DESCRIPTOR {
    union {
        DWORD AllAttributes;
        struct {
            DWORD RvaBased : 1;             // Delay load version 2
            DWORD ReservedAttributes : 31;
        } DUMMYSTRUCTNAME;
    } Attributes;

    DWORD DllNameRVA;                       // RVA to the name of the target library (NULL-terminate ASCII string)
    DWORD ModuleHandleRVA;                  // RVA to the HMODULE caching location (PHMODULE)
    DWORD ImportAddressTableRVA;            // RVA to the start of the IAT (PIMAGE_THUNK_DATA)
    DWORD ImportNameTableRVA;               // RVA to the start of the name table (PIMAGE_THUNK_DATA::AddressOfData)
    DWORD BoundImportAddressTableRVA;       // RVA to an optional bound IAT
    DWORD UnloadInformationTableRVA;        // RVA to an optional unload info table
    DWORD TimeDateStamp;                    // 0 if not bound,
                                            // Otherwise, date/time of the target DLL

} IMAGE_DELAYLOAD_DESCRIPTOR, * PIMAGE_DELAYLOAD_DESCRIPTOR;

typedef const IMAGE_DELAYLOAD_DESCRIPTOR* PCIMAGE_DELAYLOAD_DESCRIPTOR;

//
// Resource Format.
//

//
// Resource directory consists of two counts, following by a variable length
// array of directory entries.  The first count is the number of entries at
// beginning of the array that have actual names associated with each entry.
// The entries are in ascending order, case insensitive strings.  The second
// count is the number of entries that immediately follow the named entries.
// This second count identifies the number of entries that have 16-bit integer
// Ids as their name.  These entries are also sorted in ascending order.
//
// This structure allows fast lookup by either name or number, but for any
// given resource entry only one form of lookup is supported, not both.
// This is consistant with the syntax of the .RC file and the .RES file.
//

typedef struct _IMAGE_RESOURCE_DIRECTORY {
    DWORD   Characteristics;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    WORD    NumberOfNamedEntries;
    WORD    NumberOfIdEntries;
    //  IMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntries[];
} IMAGE_RESOURCE_DIRECTORY, * PIMAGE_RESOURCE_DIRECTORY;

#define IMAGE_RESOURCE_NAME_IS_STRING        0x80000000
#define IMAGE_RESOURCE_DATA_IS_DIRECTORY     0x80000000
//
// Each directory contains the 32-bit Name of the entry and an offset,
// relative to the beginning of the resource directory of the data associated
// with this directory entry.  If the name of the entry is an actual text
// string instead of an integer Id, then the high order bit of the name field
// is set to one and the low order 31-bits are an offset, relative to the
// beginning of the resource directory of the string, which is of type
// IMAGE_RESOURCE_DIRECTORY_STRING.  Otherwise the high bit is clear and the
// low-order 16-bits are the integer Id that identify this resource directory
// entry. If the directory entry is yet another resource directory (i.e. a
// subdirectory), then the high order bit of the offset field will be
// set to indicate this.  Otherwise the high bit is clear and the offset
// field points to a resource data entry.
//

//@[comment("MVI_tracked")]
typedef struct _IMAGE_RESOURCE_DIRECTORY_ENTRY {
    union {
        struct {
            DWORD NameOffset : 31;
            DWORD NameIsString : 1;
        } DUMMYSTRUCTNAME;
        DWORD   Name;
        WORD    Id;
    } DUMMYUNIONNAME;
    union {
        DWORD   OffsetToData;
        struct {
            DWORD   OffsetToDirectory : 31;
            DWORD   DataIsDirectory : 1;
        } DUMMYSTRUCTNAME2;
    } DUMMYUNIONNAME2;
} IMAGE_RESOURCE_DIRECTORY_ENTRY, * PIMAGE_RESOURCE_DIRECTORY_ENTRY;

//
// For resource directory entries that have actual string names, the Name
// field of the directory entry points to an object of the following type.
// All of these string objects are stored together after the last resource
// directory entry and before the first resource data object.  This minimizes
// the impact of these variable length objects on the alignment of the fixed
// size directory entry objects.
//

typedef struct _IMAGE_RESOURCE_DIRECTORY_STRING {
    WORD    Length;
    CHAR    NameString[1];
} IMAGE_RESOURCE_DIRECTORY_STRING, * PIMAGE_RESOURCE_DIRECTORY_STRING;


typedef struct _IMAGE_RESOURCE_DIR_STRING_U {
    WORD    Length;
    WCHAR   NameString[1];
} IMAGE_RESOURCE_DIR_STRING_U, * PIMAGE_RESOURCE_DIR_STRING_U;


//
// Each resource data entry describes a leaf node in the resource directory
// tree.  It contains an offset, relative to the beginning of the resource
// directory of the data for the resource, a size field that gives the number
// of bytes of data at that offset, a CodePage that should be used when
// decoding code point values within the resource data.  Typically for new
// applications the code page would be the unicode code page.
//

//@[comment("MVI_tracked")]
typedef struct _IMAGE_RESOURCE_DATA_ENTRY {
    DWORD   OffsetToData;
    DWORD   Size;
    DWORD   CodePage;
    DWORD   Reserved;
} IMAGE_RESOURCE_DATA_ENTRY, * PIMAGE_RESOURCE_DATA_ENTRY;

// begin_ntoshvp

//
// Code Integrity in loadconfig (CI)
//

typedef struct _IMAGE_LOAD_CONFIG_CODE_INTEGRITY {
    WORD    Flags;          // Flags to indicate if CI information is available, etc.
    WORD    Catalog;        // 0xFFFF means not available
    DWORD   CatalogOffset;
    DWORD   Reserved;       // Additional bitmask to be defined later
} IMAGE_LOAD_CONFIG_CODE_INTEGRITY, * PIMAGE_LOAD_CONFIG_CODE_INTEGRITY;

//
// Dynamic value relocation table in loadconfig
//

typedef struct _IMAGE_DYNAMIC_RELOCATION_TABLE {
    DWORD Version;
    DWORD Size;
    //  IMAGE_DYNAMIC_RELOCATION DynamicRelocations[0];
} IMAGE_DYNAMIC_RELOCATION_TABLE, * PIMAGE_DYNAMIC_RELOCATION_TABLE;

//
// Dynamic value relocation entries following IMAGE_DYNAMIC_RELOCATION_TABLE
//

#pragma pack(push, 1)

typedef struct _IMAGE_DYNAMIC_RELOCATION32 {
    DWORD      Symbol;
    DWORD      BaseRelocSize;
    //  IMAGE_BASE_RELOCATION BaseRelocations[0];
} IMAGE_DYNAMIC_RELOCATION32, * PIMAGE_DYNAMIC_RELOCATION32;

typedef struct _IMAGE_DYNAMIC_RELOCATION64 {
    ULONGLONG  Symbol;
    DWORD      BaseRelocSize;
    //  IMAGE_BASE_RELOCATION BaseRelocations[0];
} IMAGE_DYNAMIC_RELOCATION64, * PIMAGE_DYNAMIC_RELOCATION64;

typedef struct _IMAGE_DYNAMIC_RELOCATION32_V2 {
    DWORD      HeaderSize;
    DWORD      FixupInfoSize;
    DWORD      Symbol;
    DWORD      SymbolGroup;
    DWORD      Flags;
    // ...     variable length header fields
    // BYTE    FixupInfo[FixupInfoSize]
} IMAGE_DYNAMIC_RELOCATION32_V2, * PIMAGE_DYNAMIC_RELOCATION32_V2;

typedef struct _IMAGE_DYNAMIC_RELOCATION64_V2 {
    DWORD      HeaderSize;
    DWORD      FixupInfoSize;
    ULONGLONG  Symbol;
    DWORD      SymbolGroup;
    DWORD      Flags;
    // ...     variable length header fields
    // BYTE    FixupInfo[FixupInfoSize]
} IMAGE_DYNAMIC_RELOCATION64_V2, * PIMAGE_DYNAMIC_RELOCATION64_V2;

#pragma pack(pop)

#ifdef _WIN64
typedef IMAGE_DYNAMIC_RELOCATION64          IMAGE_DYNAMIC_RELOCATION;
typedef PIMAGE_DYNAMIC_RELOCATION64         PIMAGE_DYNAMIC_RELOCATION;
typedef IMAGE_DYNAMIC_RELOCATION64_V2       IMAGE_DYNAMIC_RELOCATION_V2;
typedef PIMAGE_DYNAMIC_RELOCATION64_V2      PIMAGE_DYNAMIC_RELOCATION_V2;
#else
typedef IMAGE_DYNAMIC_RELOCATION32          IMAGE_DYNAMIC_RELOCATION;
typedef PIMAGE_DYNAMIC_RELOCATION32         PIMAGE_DYNAMIC_RELOCATION;
typedef IMAGE_DYNAMIC_RELOCATION32_V2       IMAGE_DYNAMIC_RELOCATION_V2;
typedef PIMAGE_DYNAMIC_RELOCATION32_V2      PIMAGE_DYNAMIC_RELOCATION_V2;
#endif

//
// Defined symbolic dynamic relocation entries.
//

#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_PROLOGUE   0x00000001
#define IMAGE_DYNAMIC_RELOCATION_GUARD_RF_EPILOGUE   0x00000002
#define IMAGE_DYNAMIC_RELOCATION_GUARD_IMPORT_CONTROL_TRANSFER  0x00000003
#define IMAGE_DYNAMIC_RELOCATION_GUARD_INDIR_CONTROL_TRANSFER   0x00000004
#define IMAGE_DYNAMIC_RELOCATION_GUARD_SWITCHTABLE_BRANCH       0x00000005

#pragma pack(push, 1)

typedef struct _IMAGE_PROLOGUE_DYNAMIC_RELOCATION_HEADER {
    BYTE       PrologueByteCount;
    // BYTE    PrologueBytes[PrologueByteCount];
} IMAGE_PROLOGUE_DYNAMIC_RELOCATION_HEADER;
typedef IMAGE_PROLOGUE_DYNAMIC_RELOCATION_HEADER UNALIGNED* PIMAGE_PROLOGUE_DYNAMIC_RELOCATION_HEADER;

typedef struct _IMAGE_EPILOGUE_DYNAMIC_RELOCATION_HEADER {
    DWORD      EpilogueCount;
    BYTE       EpilogueByteCount;
    BYTE       BranchDescriptorElementSize;
    WORD       BranchDescriptorCount;
    // BYTE    BranchDescriptors[...];
    // BYTE    BranchDescriptorBitMap[...];
} IMAGE_EPILOGUE_DYNAMIC_RELOCATION_HEADER;
typedef IMAGE_EPILOGUE_DYNAMIC_RELOCATION_HEADER UNALIGNED* PIMAGE_EPILOGUE_DYNAMIC_RELOCATION_HEADER;

typedef struct _IMAGE_IMPORT_CONTROL_TRANSFER_DYNAMIC_RELOCATION {
    DWORD       PageRelativeOffset : 12;
    DWORD       IndirectCall : 1;
    DWORD       IATIndex : 19;
} IMAGE_IMPORT_CONTROL_TRANSFER_DYNAMIC_RELOCATION;
typedef IMAGE_IMPORT_CONTROL_TRANSFER_DYNAMIC_RELOCATION UNALIGNED* PIMAGE_IMPORT_CONTROL_TRANSFER_DYNAMIC_RELOCATION;

typedef struct _IMAGE_INDIR_CONTROL_TRANSFER_DYNAMIC_RELOCATION {
    WORD        PageRelativeOffset : 12;
    WORD        IndirectCall : 1;
    WORD        RexWPrefix : 1;
    WORD        CfgCheck : 1;
    WORD        Reserved : 1;
} IMAGE_INDIR_CONTROL_TRANSFER_DYNAMIC_RELOCATION;
typedef IMAGE_INDIR_CONTROL_TRANSFER_DYNAMIC_RELOCATION UNALIGNED* PIMAGE_INDIR_CONTROL_TRANSFER_DYNAMIC_RELOCATION;

typedef struct _IMAGE_SWITCHTABLE_BRANCH_DYNAMIC_RELOCATION {
    WORD        PageRelativeOffset : 12;
    WORD        RegisterNumber : 4;
} IMAGE_SWITCHTABLE_BRANCH_DYNAMIC_RELOCATION;
typedef IMAGE_SWITCHTABLE_BRANCH_DYNAMIC_RELOCATION UNALIGNED* PIMAGE_SWITCHTABLE_BRANCH_DYNAMIC_RELOCATION;

#pragma pack(pop)                   // Back to 4 byte packing

//
// Load Configuration Directory Entry
//

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY32 {
    DWORD   Size;
    DWORD   TimeDateStamp;
    WORD    MajorVersion;
    WORD    MinorVersion;
    DWORD   GlobalFlagsClear;
    DWORD   GlobalFlagsSet;
    DWORD   CriticalSectionDefaultTimeout;
    DWORD   DeCommitFreeBlockThreshold;
    DWORD   DeCommitTotalFreeThreshold;
    DWORD   LockPrefixTable;                // VA
    DWORD   MaximumAllocationSize;
    DWORD   VirtualMemoryThreshold;
    DWORD   ProcessHeapFlags;
    DWORD   ProcessAffinityMask;
    WORD    CSDVersion;
    WORD    DependentLoadFlags;
    DWORD   EditList;                       // VA
    DWORD   SecurityCookie;                 // VA
    DWORD   SEHandlerTable;                 // VA
    DWORD   SEHandlerCount;
    DWORD   GuardCFCheckFunctionPointer;    // VA
    DWORD   GuardCFDispatchFunctionPointer; // VA
    DWORD   GuardCFFunctionTable;           // VA
    DWORD   GuardCFFunctionCount;
    DWORD   GuardFlags;
    IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    DWORD   GuardAddressTakenIatEntryTable; // VA
    DWORD   GuardAddressTakenIatEntryCount;
    DWORD   GuardLongJumpTargetTable;       // VA
    DWORD   GuardLongJumpTargetCount;
    DWORD   DynamicValueRelocTable;         // VA
    DWORD   CHPEMetadataPointer;
    DWORD   GuardRFFailureRoutine;          // VA
    DWORD   GuardRFFailureRoutineFunctionPointer; // VA
    DWORD   DynamicValueRelocTableOffset;
    WORD    DynamicValueRelocTableSection;
    WORD    Reserved2;
    DWORD   GuardRFVerifyStackPointerFunctionPointer; // VA
    DWORD   HotPatchTableOffset;
    DWORD   Reserved3;
    DWORD   EnclaveConfigurationPointer;    // VA
    DWORD   VolatileMetadataPointer;        // VA
    DWORD   GuardEHContinuationTable;       // VA
    DWORD   GuardEHContinuationCount;
    DWORD   GuardXFGCheckFunctionPointer;    // VA
    DWORD   GuardXFGDispatchFunctionPointer; // VA
    DWORD   GuardXFGTableDispatchFunctionPointer; // VA
    DWORD   CastGuardOsDeterminedFailureMode; // VA
} IMAGE_LOAD_CONFIG_DIRECTORY32, * PIMAGE_LOAD_CONFIG_DIRECTORY32;

typedef struct _IMAGE_LOAD_CONFIG_DIRECTORY64 {
    DWORD      Size;
    DWORD      TimeDateStamp;
    WORD       MajorVersion;
    WORD       MinorVersion;
    DWORD      GlobalFlagsClear;
    DWORD      GlobalFlagsSet;
    DWORD      CriticalSectionDefaultTimeout;
    ULONGLONG  DeCommitFreeBlockThreshold;
    ULONGLONG  DeCommitTotalFreeThreshold;
    ULONGLONG  LockPrefixTable;                // VA
    ULONGLONG  MaximumAllocationSize;
    ULONGLONG  VirtualMemoryThreshold;
    ULONGLONG  ProcessAffinityMask;
    DWORD      ProcessHeapFlags;
    WORD       CSDVersion;
    WORD       DependentLoadFlags;
    ULONGLONG  EditList;                       // VA
    ULONGLONG  SecurityCookie;                 // VA
    ULONGLONG  SEHandlerTable;                 // VA
    ULONGLONG  SEHandlerCount;
    ULONGLONG  GuardCFCheckFunctionPointer;    // VA
    ULONGLONG  GuardCFDispatchFunctionPointer; // VA
    ULONGLONG  GuardCFFunctionTable;           // VA
    ULONGLONG  GuardCFFunctionCount;
    DWORD      GuardFlags;
    IMAGE_LOAD_CONFIG_CODE_INTEGRITY CodeIntegrity;
    ULONGLONG  GuardAddressTakenIatEntryTable; // VA
    ULONGLONG  GuardAddressTakenIatEntryCount;
    ULONGLONG  GuardLongJumpTargetTable;       // VA
    ULONGLONG  GuardLongJumpTargetCount;
    ULONGLONG  DynamicValueRelocTable;         // VA
    ULONGLONG  CHPEMetadataPointer;            // VA
    ULONGLONG  GuardRFFailureRoutine;          // VA
    ULONGLONG  GuardRFFailureRoutineFunctionPointer; // VA
    DWORD      DynamicValueRelocTableOffset;
    WORD       DynamicValueRelocTableSection;
    WORD       Reserved2;
    ULONGLONG  GuardRFVerifyStackPointerFunctionPointer; // VA
    DWORD      HotPatchTableOffset;
    DWORD      Reserved3;
    ULONGLONG  EnclaveConfigurationPointer;     // VA
    ULONGLONG  VolatileMetadataPointer;         // VA
    ULONGLONG  GuardEHContinuationTable;        // VA
    ULONGLONG  GuardEHContinuationCount;
    ULONGLONG  GuardXFGCheckFunctionPointer;    // VA
    ULONGLONG  GuardXFGDispatchFunctionPointer; // VA
    ULONGLONG  GuardXFGTableDispatchFunctionPointer; // VA
    ULONGLONG  CastGuardOsDeterminedFailureMode; // VA
} IMAGE_LOAD_CONFIG_DIRECTORY64, * PIMAGE_LOAD_CONFIG_DIRECTORY64;

// end_ntoshvp
// begin_ntoshvp

#ifdef _WIN64
typedef IMAGE_LOAD_CONFIG_DIRECTORY64     IMAGE_LOAD_CONFIG_DIRECTORY;
typedef PIMAGE_LOAD_CONFIG_DIRECTORY64    PIMAGE_LOAD_CONFIG_DIRECTORY;
#else
typedef IMAGE_LOAD_CONFIG_DIRECTORY32     IMAGE_LOAD_CONFIG_DIRECTORY;
typedef PIMAGE_LOAD_CONFIG_DIRECTORY32    PIMAGE_LOAD_CONFIG_DIRECTORY;
#endif

typedef enum _HEAP_INFORMATION_CLASS {
    HeapCompatibilityInformation = 0,
    HeapEnableTerminationOnCorruption = 1,
    HeapOptimizeResources = 3
} HEAP_INFORMATION_CLASS;

typedef struct _MESSAGE_RESOURCE_ENTRY {
    WORD   Length;
    WORD   Flags;
    BYTE  Text[1];
} MESSAGE_RESOURCE_ENTRY, * PMESSAGE_RESOURCE_ENTRY;

#define MESSAGE_RESOURCE_UNICODE 0x0001

typedef char* va_list; //-V677

#define _ADDRESSOF(v) (&const_cast<char&>(reinterpret_cast<const volatile char&>(v)))

typedef struct _SID_AND_ATTRIBUTES {
    PSID Sid;
    DWORD Attributes;
} SID_AND_ATTRIBUTES, * PSID_AND_ATTRIBUTES;

#define SID_HASH_SIZE 32
typedef ULONG_PTR SID_HASH_ENTRY, * PSID_HASH_ENTRY;

typedef struct _SID_AND_ATTRIBUTES_HASH {
    DWORD SidCount;
    PSID_AND_ATTRIBUTES SidAttr;
    SID_HASH_ENTRY Hash[SID_HASH_SIZE];
} SID_AND_ATTRIBUTES_HASH, * PSID_AND_ATTRIBUTES_HASH;

#define SECURITY_DESCRIPTOR_MIN_LENGTH   (sizeof(SECURITY_DESCRIPTOR))

typedef WORD   SECURITY_DESCRIPTOR_CONTROL, * PSECURITY_DESCRIPTOR_CONTROL;

#define SE_OWNER_DEFAULTED               (0x0001)
#define SE_GROUP_DEFAULTED               (0x0002)
#define SE_DACL_PRESENT                  (0x0004)
#define SE_DACL_DEFAULTED                (0x0008)
#define SE_SACL_PRESENT                  (0x0010)
#define SE_SACL_DEFAULTED                (0x0020)
#define SE_DACL_AUTO_INHERIT_REQ         (0x0100)
#define SE_SACL_AUTO_INHERIT_REQ         (0x0200)
#define SE_DACL_AUTO_INHERITED           (0x0400)
#define SE_SACL_AUTO_INHERITED           (0x0800)
#define SE_DACL_PROTECTED                (0x1000)
#define SE_SACL_PROTECTED                (0x2000)
#define SE_RM_CONTROL_VALID              (0x4000)
#define SE_SELF_RELATIVE                 (0x8000)

//
//  Where:
//
//      SE_OWNER_DEFAULTED - This boolean flag, when set, indicates that the
//          SID pointed to by the Owner field was provided by a
//          defaulting mechanism rather than explicitly provided by the
//          original provider of the security descriptor.  This may
//          affect the treatment of the SID with respect to inheritence
//          of an owner.
//
//      SE_GROUP_DEFAULTED - This boolean flag, when set, indicates that the
//          SID in the Group field was provided by a defaulting mechanism
//          rather than explicitly provided by the original provider of
//          the security descriptor.  This may affect the treatment of
//          the SID with respect to inheritence of a primary group.
//
//      SE_DACL_PRESENT - This boolean flag, when set, indicates that the
//          security descriptor contains a discretionary ACL.  If this
//          flag is set and the Dacl field of the SECURITY_DESCRIPTOR is
//          null, then a null ACL is explicitly being specified.
//
//      SE_DACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Dacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the DaclPresent flag is not set.
//
//      SE_SACL_PRESENT - This boolean flag, when set,  indicates that the
//          security descriptor contains a system ACL pointed to by the
//          Sacl field.  If this flag is set and the Sacl field of the
//          SECURITY_DESCRIPTOR is null, then an empty (but present)
//          ACL is being specified.
//
//      SE_SACL_DEFAULTED - This boolean flag, when set, indicates that the
//          ACL pointed to by the Sacl field was provided by a defaulting
//          mechanism rather than explicitly provided by the original
//          provider of the security descriptor.  This may affect the
//          treatment of the ACL with respect to inheritence of an ACL.
//          This flag is ignored if the SaclPresent flag is not set.
//
//      SE_SELF_RELATIVE - This boolean flag, when set, indicates that the
//          security descriptor is in self-relative form.  In this form,
//          all fields of the security descriptor are contiguous in memory
//          and all pointer fields are expressed as offsets from the
//          beginning of the security descriptor.  This form is useful
//          for treating security descriptors as opaque data structures
//          for transmission in communication protocol or for storage on
//          secondary media.
//
//
//
// Pictorially the structure of a security descriptor is as follows:
//
//       3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//       1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//      +---------------------------------------------------------------+
//      |            Control            |Reserved1 (SBZ)|   Revision    |
//      +---------------------------------------------------------------+
//      |                            Owner                              |
//      +---------------------------------------------------------------+
//      |                            Group                              |
//      +---------------------------------------------------------------+
//      |                            Sacl                               |
//      +---------------------------------------------------------------+
//      |                            Dacl                               |
//      +---------------------------------------------------------------+
//
// In general, this data structure should be treated opaquely to ensure future
// compatibility.
//
//

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR   szCSDVersion[128];     // Maintenance string for PSS usage
} OSVERSIONINFOA, * POSVERSIONINFOA, * LPOSVERSIONINFOA;

typedef struct _OSVERSIONINFOW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR  szCSDVersion[128];     // Maintenance string for PSS usage
} OSVERSIONINFOW, * POSVERSIONINFOW, * LPOSVERSIONINFOW, RTL_OSVERSIONINFOW, * PRTL_OSVERSIONINFOW;

typedef struct _OSVERSIONINFOEXW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR  szCSDVersion[128];     // Maintenance string for PSS usage
    WORD   wServicePackMajor;
    WORD   wServicePackMinor;
    WORD   wSuiteMask;
    BYTE  wProductType;
    BYTE  wReserved;
} OSVERSIONINFOEXW, * POSVERSIONINFOEXW, * LPOSVERSIONINFOEXW, RTL_OSVERSIONINFOEXW, * PRTL_OSVERSIONINFOEXW;

typedef VOID(NTAPI* WAITORTIMERCALLBACKFUNC) (PVOID, BOOLEAN);
typedef VOID(NTAPI* WORKERCALLBACKFUNC) (PVOID);
typedef VOID(NTAPI* APC_CALLBACK_FUNCTION) (DWORD, PVOID, PVOID);

#define PERFORMANCE_DATA_VERSION 1

typedef struct _HARDWARE_COUNTER_DATA {
    HARDWARE_COUNTER_TYPE Type;
    DWORD Reserved;
    DWORD64 Value;
} HARDWARE_COUNTER_DATA, * PHARDWARE_COUNTER_DATA;

typedef struct _PERFORMANCE_DATA {
    WORD   Size;
    BYTE  Version;
    BYTE  HwCountersCount;
    DWORD ContextSwitchCount;
    DWORD64 WaitReasonBitMap;
    DWORD64 CycleTime;
    DWORD RetryCount;
    DWORD Reserved;
    HARDWARE_COUNTER_DATA HwCounters[MAX_HW_COUNTERS];
} PERFORMANCE_DATA, * PPERFORMANCE_DATA;

typedef unsigned int ULONG32, * PULONG32; //-V677
typedef unsigned int DWORD32, * PDWORD32; //-V677
typedef __int64 LONG64, * PLONG64; //-V677

//
// Token Types
//

typedef enum _TOKEN_TYPE {
    TokenPrimary = 1,
    TokenImpersonation
} TOKEN_TYPE;
typedef TOKEN_TYPE* PTOKEN_TYPE;

//
// Token elevation values describe the relative strength of a given token.
// A full token is a token with all groups and privileges to which the principal
// is authorized.  A limited token is one with some groups or privileges removed.
//

typedef enum _TOKEN_ELEVATION_TYPE {
    TokenElevationTypeDefault = 1,
    TokenElevationTypeFull,
    TokenElevationTypeLimited,
} TOKEN_ELEVATION_TYPE, * PTOKEN_ELEVATION_TYPE;

typedef struct _TOKEN_ELEVATION {
    DWORD TokenIsElevated;
} TOKEN_ELEVATION, * PTOKEN_ELEVATION;

typedef enum _TOKEN_INFORMATION_CLASS {
    TokenUser = 1,
    TokenGroups,
    TokenPrivileges,
    TokenOwner,
    TokenPrimaryGroup,
    TokenDefaultDacl,
    TokenSource,
    TokenType,
    TokenImpersonationLevel,
    TokenStatistics,
    TokenRestrictedSids,
    TokenSessionId,
    TokenGroupsAndPrivileges,
    TokenSessionReference,
    TokenSandBoxInert,
    TokenAuditPolicy,
    TokenOrigin,
    TokenElevationType,
    TokenLinkedToken,
    TokenElevation,
    TokenHasRestrictions,
    TokenAccessInformation,
    TokenVirtualizationAllowed,
    TokenVirtualizationEnabled,
    TokenIntegrityLevel,
    TokenUIAccess,
    TokenMandatoryPolicy,
    TokenLogonSid,
    TokenIsAppContainer,
    TokenCapabilities,
    TokenAppContainerSid,
    TokenAppContainerNumber,
    TokenUserClaimAttributes,
    TokenDeviceClaimAttributes,
    TokenRestrictedUserClaimAttributes,
    TokenRestrictedDeviceClaimAttributes,
    TokenDeviceGroups,
    TokenRestrictedDeviceGroups,
    TokenSecurityAttributes,
    TokenIsRestricted,
    TokenProcessTrustLevel,
    TokenPrivateNameSpace,
    TokenSingletonAttributes,
    TokenBnoIsolation,
    TokenChildProcessFlags,
    TokenIsLessPrivilegedAppContainer,
    MaxTokenInfoClass  // MaxTokenInfoClass should always be the last enum
} TOKEN_INFORMATION_CLASS, * PTOKEN_INFORMATION_CLASS;

////////////////////////////////////////////////////////////////////////
//                                                                    //
//                          User and Group related SID attributes     //
//                                                                    //
////////////////////////////////////////////////////////////////////////

//
// Group attributes
//

#define SE_GROUP_MANDATORY                 (0x00000001L)
#define SE_GROUP_ENABLED_BY_DEFAULT        (0x00000002L)
#define SE_GROUP_ENABLED                   (0x00000004L)
#define SE_GROUP_OWNER                     (0x00000008L)
#define SE_GROUP_USE_FOR_DENY_ONLY         (0x00000010L)
#define SE_GROUP_INTEGRITY                 (0x00000020L)
#define SE_GROUP_INTEGRITY_ENABLED         (0x00000040L)
#define SE_GROUP_LOGON_ID                  (0xC0000000L)
#define SE_GROUP_RESOURCE                  (0x20000000L)

#define SE_GROUP_VALID_ATTRIBUTES          (SE_GROUP_MANDATORY          | \
                                            SE_GROUP_ENABLED_BY_DEFAULT | \
                                            SE_GROUP_ENABLED            | \
                                            SE_GROUP_OWNER              | \
                                            SE_GROUP_USE_FOR_DENY_ONLY  | \
                                            SE_GROUP_LOGON_ID           | \
                                            SE_GROUP_RESOURCE           | \
                                            SE_GROUP_INTEGRITY          | \
                                            SE_GROUP_INTEGRITY_ENABLED)

#pragma pack(push,4)
typedef struct _LUID_AND_ATTRIBUTES {
    LUID Luid;
    DWORD Attributes;
} LUID_AND_ATTRIBUTES, * PLUID_AND_ATTRIBUTES;
typedef LUID_AND_ATTRIBUTES LUID_AND_ATTRIBUTES_ARRAY[ANYSIZE_ARRAY];
typedef LUID_AND_ATTRIBUTES_ARRAY* PLUID_AND_ATTRIBUTES_ARRAY;
#pragma pack(pop)

typedef struct _TOKEN_PRIVILEGES {
    DWORD PrivilegeCount;
    LUID_AND_ATTRIBUTES Privileges[ANYSIZE_ARRAY];
} TOKEN_PRIVILEGES, * PTOKEN_PRIVILEGES;

typedef struct _TOKEN_GROUPS {
    DWORD GroupCount;
    SID_AND_ATTRIBUTES Groups[ANYSIZE_ARRAY];
} TOKEN_GROUPS, * PTOKEN_GROUPS;

typedef struct _PRIVILEGE_SET {
    DWORD PrivilegeCount;
    DWORD Control;
    LUID_AND_ATTRIBUTES Privilege[ANYSIZE_ARRAY];
} PRIVILEGE_SET, * PPRIVILEGE_SET;

typedef struct _OBJECT_TYPE_LIST {
    WORD   Level;
    WORD   Sbz;
    GUID* ObjectType;
} OBJECT_TYPE_LIST, * POBJECT_TYPE_LIST;

typedef BYTE  SE_SIGNING_LEVEL, * PSE_SIGNING_LEVEL;

#define SE_SIGNING_LEVEL_UNCHECKED         0x00000000
#define SE_SIGNING_LEVEL_UNSIGNED          0x00000001
#define SE_SIGNING_LEVEL_ENTERPRISE        0x00000002
#define SE_SIGNING_LEVEL_CUSTOM_1          0x00000003
#define SE_SIGNING_LEVEL_DEVELOPER         SE_SIGNING_LEVEL_CUSTOM_1
#define SE_SIGNING_LEVEL_AUTHENTICODE      0x00000004
#define SE_SIGNING_LEVEL_CUSTOM_2          0x00000005
#define SE_SIGNING_LEVEL_STORE             0x00000006
#define SE_SIGNING_LEVEL_CUSTOM_3          0x00000007
#define SE_SIGNING_LEVEL_ANTIMALWARE       SE_SIGNING_LEVEL_CUSTOM_3
#define SE_SIGNING_LEVEL_MICROSOFT         0x00000008
#define SE_SIGNING_LEVEL_CUSTOM_4          0x00000009
#define SE_SIGNING_LEVEL_CUSTOM_5          0x0000000A
#define SE_SIGNING_LEVEL_DYNAMIC_CODEGEN   0x0000000B
#define SE_SIGNING_LEVEL_WINDOWS           0x0000000C
#define SE_SIGNING_LEVEL_CUSTOM_7          0x0000000D
#define SE_SIGNING_LEVEL_WINDOWS_TCB       0x0000000E
#define SE_SIGNING_LEVEL_CUSTOM_6          0x0000000F

typedef enum _AUDIT_EVENT_TYPE {
    AuditEventObjectAccess,
    AuditEventDirectoryServiceAccess
} AUDIT_EVENT_TYPE, * PAUDIT_EVENT_TYPE;

#define AUDIT_ALLOW_NO_PRIVILEGE 0x1

#if _MSC_VER >= 1200
#pragma warning(push)
#pragma warning(disable:4820) // padding added after data member
#endif

typedef GUID UOW, * PUOW;
typedef GUID CRM_PROTOCOL_ID, * PCRM_PROTOCOL_ID;

//
// Define the TransactionManager option values
//

#define TRANSACTION_MANAGER_VOLATILE              0x00000001
#define TRANSACTION_MANAGER_COMMIT_DEFAULT        0x00000000
#define TRANSACTION_MANAGER_COMMIT_SYSTEM_VOLUME  0x00000002
#define TRANSACTION_MANAGER_COMMIT_SYSTEM_HIVES   0x00000004
#define TRANSACTION_MANAGER_COMMIT_LOWEST         0x00000008
#define TRANSACTION_MANAGER_CORRUPT_FOR_RECOVERY  0x00000010
#define TRANSACTION_MANAGER_CORRUPT_FOR_PROGRESS  0x00000020
#define TRANSACTION_MANAGER_MAXIMUM_OPTION        0x0000003F

//
// Define the Transaction option values
//

#define TRANSACTION_DO_NOT_PROMOTE        0x00000001
#define TRANSACTION_MAXIMUM_OPTION        0x00000001

//
// Define the ResourceManager option values
//

#define RESOURCE_MANAGER_VOLATILE            0x00000001
#define RESOURCE_MANAGER_COMMUNICATION       0x00000002
#define RESOURCE_MANAGER_MAXIMUM_OPTION      0x00000003

//
// Define the RegisterProtocol option values
//

#define CRM_PROTOCOL_EXPLICIT_MARSHAL_ONLY      0x00000001
#define CRM_PROTOCOL_DYNAMIC_MARSHAL_INFO       0x00000002
#define CRM_PROTOCOL_MAXIMUM_OPTION             0x00000003

//
// Define the Enlistment option values
//

#define ENLISTMENT_SUPERIOR                  0x00000001
#define ENLISTMENT_MAXIMUM_OPTION            0x00000001

typedef ULONG NOTIFICATION_MASK;
#define TRANSACTION_NOTIFY_MASK                 0x3FFFFFFF
#define TRANSACTION_NOTIFY_PREPREPARE           0x00000001
#define TRANSACTION_NOTIFY_PREPARE              0x00000002
#define TRANSACTION_NOTIFY_COMMIT               0x00000004
#define TRANSACTION_NOTIFY_ROLLBACK             0x00000008
#define TRANSACTION_NOTIFY_PREPREPARE_COMPLETE  0x00000010
#define TRANSACTION_NOTIFY_PREPARE_COMPLETE     0x00000020
#define TRANSACTION_NOTIFY_COMMIT_COMPLETE      0x00000040
#define TRANSACTION_NOTIFY_ROLLBACK_COMPLETE    0x00000080
#define TRANSACTION_NOTIFY_RECOVER              0x00000100
#define TRANSACTION_NOTIFY_SINGLE_PHASE_COMMIT  0x00000200
#define TRANSACTION_NOTIFY_DELEGATE_COMMIT      0x00000400
#define TRANSACTION_NOTIFY_RECOVER_QUERY        0x00000800
#define TRANSACTION_NOTIFY_ENLIST_PREPREPARE    0x00001000
#define TRANSACTION_NOTIFY_LAST_RECOVER         0x00002000
#define TRANSACTION_NOTIFY_INDOUBT              0x00004000
#define TRANSACTION_NOTIFY_PROPAGATE_PULL       0x00008000
#define TRANSACTION_NOTIFY_PROPAGATE_PUSH       0x00010000
#define TRANSACTION_NOTIFY_MARSHAL              0x00020000
#define TRANSACTION_NOTIFY_ENLIST_MASK          0x00040000
#define TRANSACTION_NOTIFY_RM_DISCONNECTED      0x01000000
#define TRANSACTION_NOTIFY_TM_ONLINE            0x02000000
#define TRANSACTION_NOTIFY_COMMIT_REQUEST       0x04000000
#define TRANSACTION_NOTIFY_PROMOTE              0x08000000
#define TRANSACTION_NOTIFY_PROMOTE_NEW          0x10000000
#define TRANSACTION_NOTIFY_REQUEST_OUTCOME      0x20000000
//
//  Note that this flag is not included in the TRANSACTION_NOTIFY_MASK.
//  The reason being that KTM does not understand this flag yet. This
//  flag is strictly for the use of filter manager. In fact we mask it
//  out before enlisting in any transaction.
//
#define TRANSACTION_NOTIFY_COMMIT_FINALIZE      0x40000000

//
// Path to the transaction manager objects in the NT
// object namespace.
//
#define TRANSACTIONMANAGER_OBJECT_PATH  L"\\TransactionManager\\"
#define TRANSACTION_OBJECT_PATH         L"\\Transaction\\"
#define ENLISTMENT_OBJECT_PATH          L"\\Enlistment\\"
#define RESOURCE_MANAGER_OBJECT_PATH    L"\\ResourceManager\\"

//
// The following three defines are here to ease the allocation
// of string buffers which are to contain a fully qualified
// transaction manager object name, e.g., \Transaction\{GUID}
//
#define TRANSACTIONMANAGER_OBJECT_NAME_LENGTH_IN_BYTES      (sizeof(TRANSACTIONMANAGER_OBJECT_PATH)+(38*sizeof(WCHAR)))
#define TRANSACTION_OBJECT_NAME_LENGTH_IN_BYTES      (sizeof(TRANSACTION_OBJECT_PATH)+(38*sizeof(WCHAR)))
#define ENLISTMENT_OBJECT_NAME_LENGTH_IN_BYTES       (sizeof(ENLISTMENT_OBJECT_PATH)+(38*sizeof(WCHAR)))
#define RESOURCE_MANAGER_OBJECT_NAME_LENGTH_IN_BYTES (sizeof(RESOURCE_MANAGER_OBJECT_PATH)+(38*sizeof(WCHAR)))

//
// KTM Tm object rights
//
#define TRANSACTIONMANAGER_QUERY_INFORMATION     ( 0x0001 )
#define TRANSACTIONMANAGER_SET_INFORMATION       ( 0x0002 )
#define TRANSACTIONMANAGER_RECOVER               ( 0x0004 )
#define TRANSACTIONMANAGER_RENAME                ( 0x0008 )
#define TRANSACTIONMANAGER_CREATE_RM             ( 0x0010 )

// The following right is intended for DTC's use only; it will be
// deprecated, and no one else should take a dependency on it.
#define TRANSACTIONMANAGER_BIND_TRANSACTION      ( 0x0020 )

//
// Generic mappings for transaction manager rights.
//

#define TRANSACTIONMANAGER_GENERIC_READ            (STANDARD_RIGHTS_READ            |\
                                                    TRANSACTIONMANAGER_QUERY_INFORMATION)

#define TRANSACTIONMANAGER_GENERIC_WRITE           (STANDARD_RIGHTS_WRITE           |\
                                                    TRANSACTIONMANAGER_SET_INFORMATION     |\
                                                    TRANSACTIONMANAGER_RECOVER             |\
                                                    TRANSACTIONMANAGER_RENAME              |\
                                                    TRANSACTIONMANAGER_CREATE_RM)

#define TRANSACTIONMANAGER_GENERIC_EXECUTE         (STANDARD_RIGHTS_EXECUTE)

#define TRANSACTIONMANAGER_ALL_ACCESS              (STANDARD_RIGHTS_REQUIRED        |\
                                                    TRANSACTIONMANAGER_GENERIC_READ        |\
                                                    TRANSACTIONMANAGER_GENERIC_WRITE       |\
                                                    TRANSACTIONMANAGER_GENERIC_EXECUTE     |\
                                                    TRANSACTIONMANAGER_BIND_TRANSACTION)

//
// KTM transaction object rights.
//
#define TRANSACTION_QUERY_INFORMATION     ( 0x0001 )
#define TRANSACTION_SET_INFORMATION       ( 0x0002 )
#define TRANSACTION_ENLIST                ( 0x0004 )
#define TRANSACTION_COMMIT                ( 0x0008 )
#define TRANSACTION_ROLLBACK              ( 0x0010 )
#define TRANSACTION_PROPAGATE             ( 0x0020 )
#define TRANSACTION_RIGHT_RESERVED1       ( 0x0040 )

//
// Generic mappings for transaction rights.
// Resource managers, when enlisting, should generally use the macro
// TRANSACTION_RESOURCE_MANAGER_RIGHTS when opening a transaction.
// It's the same as generic read and write except that it does not allow
// a commit decision to be made.
//

#define TRANSACTION_GENERIC_READ            (STANDARD_RIGHTS_READ            |\
                                             TRANSACTION_QUERY_INFORMATION   |\
                                             SYNCHRONIZE)

#define TRANSACTION_GENERIC_WRITE           (STANDARD_RIGHTS_WRITE           |\
                                             TRANSACTION_SET_INFORMATION     |\
                                             TRANSACTION_COMMIT              |\
                                             TRANSACTION_ENLIST              |\
                                             TRANSACTION_ROLLBACK            |\
                                             TRANSACTION_PROPAGATE           |\
                                             SYNCHRONIZE)

#define TRANSACTION_GENERIC_EXECUTE         (STANDARD_RIGHTS_EXECUTE         |\
                                             TRANSACTION_COMMIT              |\
                                             TRANSACTION_ROLLBACK            |\
                                             SYNCHRONIZE)

#define TRANSACTION_ALL_ACCESS              (STANDARD_RIGHTS_REQUIRED        |\
                                             TRANSACTION_GENERIC_READ        |\
                                             TRANSACTION_GENERIC_WRITE       |\
                                             TRANSACTION_GENERIC_EXECUTE)

#define TRANSACTION_RESOURCE_MANAGER_RIGHTS (TRANSACTION_GENERIC_READ        |\
                                             STANDARD_RIGHTS_WRITE           |\
                                             TRANSACTION_SET_INFORMATION     |\
                                             TRANSACTION_ENLIST              |\
                                             TRANSACTION_ROLLBACK            |\
                                             TRANSACTION_PROPAGATE           |\
                                             SYNCHRONIZE)

//
// KTM resource manager object rights.
//
#define RESOURCEMANAGER_QUERY_INFORMATION     ( 0x0001 )
#define RESOURCEMANAGER_SET_INFORMATION       ( 0x0002 )
#define RESOURCEMANAGER_RECOVER               ( 0x0004 )
#define RESOURCEMANAGER_ENLIST                ( 0x0008 )
#define RESOURCEMANAGER_GET_NOTIFICATION      ( 0x0010 )
#define RESOURCEMANAGER_REGISTER_PROTOCOL     ( 0x0020 )
#define RESOURCEMANAGER_COMPLETE_PROPAGATION  ( 0x0040 )

//
// Generic mappings for resource manager rights.
//
#define RESOURCEMANAGER_GENERIC_READ        (STANDARD_RIGHTS_READ                 |\
                                             RESOURCEMANAGER_QUERY_INFORMATION    |\
                                             SYNCHRONIZE)

#define RESOURCEMANAGER_GENERIC_WRITE       (STANDARD_RIGHTS_WRITE                |\
                                             RESOURCEMANAGER_SET_INFORMATION      |\
                                             RESOURCEMANAGER_RECOVER              |\
                                             RESOURCEMANAGER_ENLIST               |\
                                             RESOURCEMANAGER_GET_NOTIFICATION     |\
                                             RESOURCEMANAGER_REGISTER_PROTOCOL    |\
                                             RESOURCEMANAGER_COMPLETE_PROPAGATION |\
                                             SYNCHRONIZE)

#define RESOURCEMANAGER_GENERIC_EXECUTE     (STANDARD_RIGHTS_EXECUTE              |\
                                             RESOURCEMANAGER_RECOVER              |\
                                             RESOURCEMANAGER_ENLIST               |\
                                             RESOURCEMANAGER_GET_NOTIFICATION     |\
                                             RESOURCEMANAGER_COMPLETE_PROPAGATION |\
                                             SYNCHRONIZE)

#define RESOURCEMANAGER_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED             |\
                                             RESOURCEMANAGER_GENERIC_READ         |\
                                             RESOURCEMANAGER_GENERIC_WRITE        |\
                                             RESOURCEMANAGER_GENERIC_EXECUTE)

//
// KTM enlistment object rights.
//
#define ENLISTMENT_QUERY_INFORMATION     ( 0x0001 )
#define ENLISTMENT_SET_INFORMATION       ( 0x0002 )
#define ENLISTMENT_RECOVER               ( 0x0004 )
#define ENLISTMENT_SUBORDINATE_RIGHTS    ( 0x0008 )
#define ENLISTMENT_SUPERIOR_RIGHTS       ( 0x0010 )

//
// Generic mappings for enlistment rights.
//
#define ENLISTMENT_GENERIC_READ        (STANDARD_RIGHTS_READ           |\
                                        ENLISTMENT_QUERY_INFORMATION)

#define ENLISTMENT_GENERIC_WRITE       (STANDARD_RIGHTS_WRITE          |\
                                        ENLISTMENT_SET_INFORMATION     |\
                                        ENLISTMENT_RECOVER             |\
                                        ENLISTMENT_SUBORDINATE_RIGHTS  |\
                                        ENLISTMENT_SUPERIOR_RIGHTS)

#define ENLISTMENT_GENERIC_EXECUTE     (STANDARD_RIGHTS_EXECUTE        |\
                                        ENLISTMENT_RECOVER             |\
                                        ENLISTMENT_SUBORDINATE_RIGHTS  |\
                                        ENLISTMENT_SUPERIOR_RIGHTS)

#define ENLISTMENT_ALL_ACCESS          (STANDARD_RIGHTS_REQUIRED       |\
                                        ENLISTMENT_GENERIC_READ        |\
                                        ENLISTMENT_GENERIC_WRITE       |\
                                        ENLISTMENT_GENERIC_EXECUTE)

// end_access

//
// Transaction outcomes.
//
// TODO: warning, must match values in KTRANSACTION_OUTCOME duplicated def
// in tm.h.
//

typedef enum _TRANSACTION_OUTCOME {
    TransactionOutcomeUndetermined = 1,
    TransactionOutcomeCommitted,
    TransactionOutcomeAborted,
} TRANSACTION_OUTCOME;

typedef enum _TRANSACTION_STATE {
    TransactionStateNormal = 1,
    TransactionStateIndoubt,
    TransactionStateCommittedNotify,
} TRANSACTION_STATE;

typedef struct _TRANSACTION_BASIC_INFORMATION {
    GUID    TransactionId;
    DWORD   State;
    DWORD   Outcome;
} TRANSACTION_BASIC_INFORMATION, * PTRANSACTION_BASIC_INFORMATION;

typedef struct _TRANSACTIONMANAGER_BASIC_INFORMATION {
    GUID    TmIdentity;
    LARGE_INTEGER VirtualClock;
} TRANSACTIONMANAGER_BASIC_INFORMATION, * PTRANSACTIONMANAGER_BASIC_INFORMATION;

typedef struct _TRANSACTIONMANAGER_LOG_INFORMATION {
    GUID  LogIdentity;
} TRANSACTIONMANAGER_LOG_INFORMATION, * PTRANSACTIONMANAGER_LOG_INFORMATION;

typedef struct _TRANSACTIONMANAGER_LOGPATH_INFORMATION {
    DWORD LogPathLength;
    _Field_size_(LogPathLength) WCHAR LogPath[1]; // Variable size
//  Data[1];                                        // Variable size data not declared
} TRANSACTIONMANAGER_LOGPATH_INFORMATION, * PTRANSACTIONMANAGER_LOGPATH_INFORMATION;

typedef struct _TRANSACTIONMANAGER_RECOVERY_INFORMATION {
    ULONGLONG  LastRecoveredLsn;
} TRANSACTIONMANAGER_RECOVERY_INFORMATION, * PTRANSACTIONMANAGER_RECOVERY_INFORMATION;

// end_wdm
typedef struct _TRANSACTIONMANAGER_OLDEST_INFORMATION {
    GUID OldestTransactionGuid;
} TRANSACTIONMANAGER_OLDEST_INFORMATION, * PTRANSACTIONMANAGER_OLDEST_INFORMATION;
// begin_wdm

typedef struct _TRANSACTION_PROPERTIES_INFORMATION {
    DWORD              IsolationLevel;
    DWORD              IsolationFlags;
    LARGE_INTEGER      Timeout;
    DWORD              Outcome;
    DWORD              DescriptionLength;
    WCHAR              Description[1];            // Variable size
//          Data[1];            // Variable size data not declared
} TRANSACTION_PROPERTIES_INFORMATION, * PTRANSACTION_PROPERTIES_INFORMATION;

// The following info-class is intended for DTC's use only; it will be
// deprecated, and no one else should take a dependency on it.
typedef struct _TRANSACTION_BIND_INFORMATION {
    HANDLE TmHandle;
} TRANSACTION_BIND_INFORMATION, * PTRANSACTION_BIND_INFORMATION;

typedef struct _TRANSACTION_ENLISTMENT_PAIR {
    GUID   EnlistmentId;
    GUID   ResourceManagerId;
} TRANSACTION_ENLISTMENT_PAIR, * PTRANSACTION_ENLISTMENT_PAIR;

typedef struct _TRANSACTION_ENLISTMENTS_INFORMATION {
    DWORD                       NumberOfEnlistments;
    TRANSACTION_ENLISTMENT_PAIR EnlistmentPair[1]; // Variable size
} TRANSACTION_ENLISTMENTS_INFORMATION, * PTRANSACTION_ENLISTMENTS_INFORMATION;

typedef struct _TRANSACTION_SUPERIOR_ENLISTMENT_INFORMATION {
    TRANSACTION_ENLISTMENT_PAIR SuperiorEnlistmentPair;
} TRANSACTION_SUPERIOR_ENLISTMENT_INFORMATION, * PTRANSACTION_SUPERIOR_ENLISTMENT_INFORMATION;

typedef struct _RESOURCEMANAGER_BASIC_INFORMATION {
    GUID    ResourceManagerId;
    DWORD   DescriptionLength;
    WCHAR   Description[1];            // Variable size
} RESOURCEMANAGER_BASIC_INFORMATION, * PRESOURCEMANAGER_BASIC_INFORMATION;

typedef struct _RESOURCEMANAGER_COMPLETION_INFORMATION {
    HANDLE    IoCompletionPortHandle;
    ULONG_PTR CompletionKey;
} RESOURCEMANAGER_COMPLETION_INFORMATION, * PRESOURCEMANAGER_COMPLETION_INFORMATION;

// end_wdm

// begin_wdm
typedef enum _TRANSACTION_INFORMATION_CLASS {
    TransactionBasicInformation,
    TransactionPropertiesInformation,
    TransactionEnlistmentInformation,
    TransactionSuperiorEnlistmentInformation
    // end_wdm
    ,
    // The following info-classes are intended for DTC's use only; it will be
    // deprecated, and no one else should take a dependency on it.
    TransactionBindInformation, // private and deprecated
    TransactionDTCPrivateInformation // private and deprecated
    ,
    // begin_wdm
} TRANSACTION_INFORMATION_CLASS;

// begin_wdm
typedef enum _TRANSACTIONMANAGER_INFORMATION_CLASS {
    TransactionManagerBasicInformation,
    TransactionManagerLogInformation,
    TransactionManagerLogPathInformation,
    TransactionManagerRecoveryInformation = 4
    // end_wdm
    ,
    // The following info-classes are intended for internal use only; they
    // are considered deprecated, and no one else should take a dependency
    // on them.
    TransactionManagerOnlineProbeInformation = 3,
    TransactionManagerOldestTransactionInformation = 5
    // end_wdm

    // begin_wdm
} TRANSACTIONMANAGER_INFORMATION_CLASS;

// begin_wdm
typedef enum _RESOURCEMANAGER_INFORMATION_CLASS {
    ResourceManagerBasicInformation,
    ResourceManagerCompletionInformation,
} RESOURCEMANAGER_INFORMATION_CLASS;

typedef struct _ENLISTMENT_BASIC_INFORMATION {
    GUID    EnlistmentId;
    GUID    TransactionId;
    GUID    ResourceManagerId;
} ENLISTMENT_BASIC_INFORMATION, * PENLISTMENT_BASIC_INFORMATION;

typedef struct _ENLISTMENT_CRM_INFORMATION {
    GUID   CrmTransactionManagerId;
    GUID   CrmResourceManagerId;
    GUID   CrmEnlistmentId;
} ENLISTMENT_CRM_INFORMATION, * PENLISTMENT_CRM_INFORMATION;

// begin_wdm
typedef enum _ENLISTMENT_INFORMATION_CLASS {
    EnlistmentBasicInformation,
    EnlistmentRecoveryInformation,
    EnlistmentCrmInformation
} ENLISTMENT_INFORMATION_CLASS;

typedef struct _TRANSACTION_LIST_ENTRY {
    UOW    UOW;
} TRANSACTION_LIST_ENTRY, * PTRANSACTION_LIST_ENTRY;

typedef struct _TRANSACTION_LIST_INFORMATION {
    DWORD   NumberOfTransactions;
    TRANSACTION_LIST_ENTRY TransactionInformation[1]; // Var size
} TRANSACTION_LIST_INFORMATION, * PTRANSACTION_LIST_INFORMATION;

//
// Types of objects known to the kernel transaction manager.
//

typedef enum _KTMOBJECT_TYPE {
    KTMOBJECT_TRANSACTION,
    KTMOBJECT_TRANSACTION_MANAGER,
    KTMOBJECT_RESOURCE_MANAGER,
    KTMOBJECT_ENLISTMENT,
    KTMOBJECT_INVALID
} KTMOBJECT_TYPE, * PKTMOBJECT_TYPE;

typedef DWORD TP_VERSION, * PTP_VERSION;

typedef struct _TP_CALLBACK_INSTANCE TP_CALLBACK_INSTANCE, * PTP_CALLBACK_INSTANCE;

typedef VOID(NTAPI* PTP_SIMPLE_CALLBACK)(
    _Inout_     PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID                 Context
    );

typedef struct _TP_POOL TP_POOL, * PTP_POOL;

typedef enum _TP_CALLBACK_PRIORITY {
    TP_CALLBACK_PRIORITY_HIGH,
    TP_CALLBACK_PRIORITY_NORMAL,
    TP_CALLBACK_PRIORITY_LOW,
    TP_CALLBACK_PRIORITY_INVALID,
    TP_CALLBACK_PRIORITY_COUNT = TP_CALLBACK_PRIORITY_INVALID
} TP_CALLBACK_PRIORITY;

typedef struct _TP_POOL_STACK_INFORMATION {
    SIZE_T StackReserve;
    SIZE_T StackCommit;
}TP_POOL_STACK_INFORMATION, * PTP_POOL_STACK_INFORMATION;

typedef struct _TP_CLEANUP_GROUP TP_CLEANUP_GROUP, * PTP_CLEANUP_GROUP;

typedef VOID(NTAPI* PTP_CLEANUP_GROUP_CANCEL_CALLBACK)(
    _Inout_opt_ PVOID ObjectContext,
    _Inout_opt_ PVOID CleanupContext
    );

typedef struct _TOKEN_USER {
    SID_AND_ATTRIBUTES User;
} TOKEN_USER, * PTOKEN_USER;

//
// KTMOBJECT_CURSOR
//
// Used by NtEnumerateTransactionObject to enumerate a transaction
// object namespace (e.g. enlistments in a resource manager).
//

typedef struct _KTMOBJECT_CURSOR {
    //
    // The last GUID enumerated; zero if beginning enumeration.
    //

    GUID    LastQuery;

    //
    // A count of GUIDs filled in by this last enumeration.
    //

    DWORD   ObjectIdCount;

    //
    // ObjectIdCount GUIDs from the namespace specified.
    //

    GUID    ObjectIds[1];
} KTMOBJECT_CURSOR, * PKTMOBJECT_CURSOR;

// TODO: warning, duplicated def in tm.h.
typedef struct _TRANSACTION_NOTIFICATION {
    PVOID         TransactionKey;
    ULONG         TransactionNotification;
    LARGE_INTEGER TmVirtualClock;
    ULONG         ArgumentLength;
} TRANSACTION_NOTIFICATION, * PTRANSACTION_NOTIFICATION;

typedef struct _TP_CALLBACK_ENVIRON_V3 {
    TP_VERSION                         Version;
    PTP_POOL                           Pool;
    PTP_CLEANUP_GROUP                  CleanupGroup;
    PTP_CLEANUP_GROUP_CANCEL_CALLBACK  CleanupGroupCancelCallback;
    PVOID                              RaceDll;
    struct _ACTIVATION_CONTEXT* ActivationContext;
    PTP_SIMPLE_CALLBACK                FinalizationCallback;
    union {
        DWORD                          Flags;
        struct {
            DWORD                      LongFunction : 1;
            DWORD                      Persistent : 1;
            DWORD                      Private : 30;
        } s;
    } u;
    TP_CALLBACK_PRIORITY               CallbackPriority;
    DWORD                              Size;
} TP_CALLBACK_ENVIRON_V3;

typedef TP_CALLBACK_ENVIRON_V3 TP_CALLBACK_ENVIRON, * PTP_CALLBACK_ENVIRON;

typedef struct _TP_WORK TP_WORK, * PTP_WORK;

typedef VOID(NTAPI* PTP_WORK_CALLBACK)(
    _Inout_     PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID                 Context,
    _Inout_     PTP_WORK              Work
    );

typedef struct _TP_TIMER TP_TIMER, * PTP_TIMER;

typedef VOID(NTAPI* PTP_TIMER_CALLBACK)(
    _Inout_     PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID                 Context,
    _Inout_     PTP_TIMER             Timer
    );

typedef DWORD    TP_WAIT_RESULT;

typedef struct _TP_WAIT TP_WAIT, * PTP_WAIT;

typedef VOID(NTAPI* PTP_WAIT_CALLBACK)(
    _Inout_     PTP_CALLBACK_INSTANCE Instance,
    _Inout_opt_ PVOID                 Context,
    _Inout_     PTP_WAIT              Wait,
    _In_        TP_WAIT_RESULT        WaitResult
    );

typedef struct _TP_IO TP_IO, * PTP_IO;

typedef struct LIST_ENTRY32 {
    DWORD Flink;
    DWORD Blink;
} LIST_ENTRY32;
typedef LIST_ENTRY32* PLIST_ENTRY32;

typedef struct LIST_ENTRY64 {
    ULONGLONG Flink;
    ULONGLONG Blink;
} LIST_ENTRY64;
typedef LIST_ENTRY64* PLIST_ENTRY64;

//
// 32 and 64 bit specific version for wow64 and the debugger
//
typedef struct _NT_TIB32 {
    DWORD ExceptionList;
    DWORD StackBase;
    DWORD StackLimit;
    DWORD SubSystemTib;

#if defined(_MSC_EXTENSIONS)
    union {
        DWORD FiberData;
        DWORD Version;
    };
#else
    DWORD FiberData;
#endif

    DWORD ArbitraryUserPointer;
    DWORD Self;
} NT_TIB32, * PNT_TIB32;

#define POINTER_32 __ptr32

typedef enum _SID_NAME_USE {
    SidTypeUser = 1,
    SidTypeGroup,
    SidTypeDomain,
    SidTypeAlias,
    SidTypeWellKnownGroup,
    SidTypeDeletedAccount,
    SidTypeInvalid,
    SidTypeUnknown,
    SidTypeComputer,
    SidTypeLabel,
    SidTypeLogonSession
} SID_NAME_USE, * PSID_NAME_USE;

__inline
unsigned long
HandleToULong(
    const void* h
)
{
    return((unsigned long)(ULONG_PTR)h);
}

__inline
long
HandleToLong(
    const void* h
)
{
    return((long)(LONG_PTR)h);
}

__inline
void*
ULongToHandle(
    const unsigned long h
)
{
    return((void*)(UINT_PTR)h);
}

__inline
void*
LongToHandle(
    const long h
)
{
    return((void*)(INT_PTR)h);
}

__inline
unsigned long
PtrToUlong(
    const void* p
)
{
    return((unsigned long)(ULONG_PTR)p);
}

__inline
unsigned int
PtrToUint(
    const void* p
)
{
    return((unsigned int)(UINT_PTR)p);
}

__inline
unsigned short
PtrToUshort(
    const void* p
)
{
    return((unsigned short)(unsigned long)(ULONG_PTR)p);
}

__inline
long
PtrToLong(
    const void* p
)
{
    return((long)(LONG_PTR)p);
}

__inline
int
PtrToInt(
    const void* p
)
{
    return((int)(INT_PTR)p);
}

__inline
short
PtrToShort(
    const void* p
)
{
    return((short)(long)(LONG_PTR)p);
}

__inline
void*
IntToPtr(
    const int i
)
// Caution: IntToPtr() sign-extends the int value.
{
    return((void*)(INT_PTR)i);
}

__inline
void*
UIntToPtr(
    const unsigned int ui
)
// Caution: UIntToPtr() zero-extends the unsigned int value.
{
    return((void*)(UINT_PTR)ui);
}

__inline
void*
LongToPtr(
    const long l
)
// Caution: LongToPtr() sign-extends the long value.
{
    return((void*)(LONG_PTR)l);
}

__inline
void*
ULongToPtr(
    const unsigned long ul
)
// Caution: ULongToPtr() zero-extends the unsigned long value.
{
    return((void*)(ULONG_PTR)ul);
}

#define PtrToPtr64( p )         ((void * POINTER_64) p)
#define Ptr64ToPtr( p )         ((void *) p)
#define HandleToHandle64( h )   (PtrToPtr64( h ))
#define Handle64ToHandle( h )   (Ptr64ToPtr( h ))

__inline
void*
Ptr32ToPtr(
    const void* POINTER_32 p
)
{
    return((void*)(ULONG_PTR)(unsigned long)p);
}

__inline
void*
Handle32ToHandle(
    const void* POINTER_32 h
)
{
    return((void*)(LONG_PTR)(long)h);
}

__inline
void* POINTER_32
PtrToPtr32(
    const void* p
)
{
    return((void* POINTER_32) (unsigned long) (ULONG_PTR) p);
}

#define HandleToHandle32( h )       (PtrToPtr32( h ))

#define HandleToUlong(h)  HandleToULong(h)
#define UlongToHandle(ul) ULongToHandle(ul)
#define UlongToPtr(ul) ULongToPtr(ul)
#define UintToPtr(ui)  UIntToPtr(ui)

#define MAXUINT_PTR  (~((UINT_PTR)0))
#define MAXINT_PTR   ((INT_PTR)(MAXUINT_PTR >> 1))
#define MININT_PTR   (~MAXINT_PTR)

#define MAXULONG_PTR (~((ULONG_PTR)0))
#define MAXLONG_PTR  ((LONG_PTR)(MAXULONG_PTR >> 1))
#define MINLONG_PTR  (~MAXLONG_PTR)

#define MAXUHALF_PTR ((UHALF_PTR)~0)
#define MAXHALF_PTR  ((HALF_PTR)(MAXUHALF_PTR >> 1))
#define MINHALF_PTR  (~MAXHALF_PTR)

typedef struct _TOKEN_OWNER {
    PSID Owner;
} TOKEN_OWNER, * PTOKEN_OWNER;

typedef struct _TOKEN_MANDATORY_POLICY {
    DWORD Policy;
} TOKEN_MANDATORY_POLICY, * PTOKEN_MANDATORY_POLICY;

#define TOKEN_SOURCE_LENGTH 8

typedef struct _TOKEN_SOURCE {
    CHAR SourceName[TOKEN_SOURCE_LENGTH];
    LUID SourceIdentifier;
} TOKEN_SOURCE, * PTOKEN_SOURCE;

typedef struct _QUOTA_LIMITS {
    SIZE_T PagedPoolLimit;
    SIZE_T NonPagedPoolLimit;
    SIZE_T MinimumWorkingSetSize;
    SIZE_T MaximumWorkingSetSize;
    SIZE_T PagefileLimit;
    LARGE_INTEGER TimeLimit;
} QUOTA_LIMITS, * PQUOTA_LIMITS;

typedef struct _TOKEN_PRIMARY_GROUP {
    PSID PrimaryGroup;
} TOKEN_PRIMARY_GROUP, * PTOKEN_PRIMARY_GROUP;

typedef struct _TOKEN_DEFAULT_DACL {
    PACL DefaultDacl;
} TOKEN_DEFAULT_DACL, * PTOKEN_DEFAULT_DACL;

typedef struct _TOKEN_USER_CLAIMS {
    PCLAIMS_BLOB UserClaims;
} TOKEN_USER_CLAIMS, * PTOKEN_USER_CLAIMS;

typedef struct _TOKEN_DEVICE_CLAIMS {
    PCLAIMS_BLOB DeviceClaims;
} TOKEN_DEVICE_CLAIMS, * PTOKEN_DEVICE_CLAIMS;

typedef SHORT* PSHORT; //-V677
typedef LONG* PLONG; //-V677

typedef LONGLONG* PLONGLONG; //-V677
typedef ULONGLONG* PULONGLONG; //-V677

typedef struct _TOKEN_STATISTICS {
    LUID                         TokenId;
    LUID                         AuthenticationId;
    LARGE_INTEGER                ExpirationTime;
    TOKEN_TYPE                   TokenType;
    SECURITY_IMPERSONATION_LEVEL ImpersonationLevel;
    ULONG                        DynamicCharged;
    ULONG                        DynamicAvailable;
    ULONG                        GroupCount;
    ULONG                        PrivilegeCount;
    LUID                         ModifiedId;
} TOKEN_STATISTICS, * PTOKEN_STATISTICS;

//
// Define unwind history table structure.
//

#define UNWIND_HISTORY_TABLE_SIZE 12

typedef struct _UNWIND_HISTORY_TABLE_ENTRY {
    DWORD64 ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
} UNWIND_HISTORY_TABLE_ENTRY, * PUNWIND_HISTORY_TABLE_ENTRY;

typedef struct _UNWIND_HISTORY_TABLE {
    DWORD Count;
    BYTE  LocalHint;
    BYTE  GlobalHint;
    BYTE  Search;
    BYTE  Once;
    DWORD64 LowAddress;
    DWORD64 HighAddress;
    UNWIND_HISTORY_TABLE_ENTRY Entry[UNWIND_HISTORY_TABLE_SIZE];
} UNWIND_HISTORY_TABLE, * PUNWIND_HISTORY_TABLE;

//
// Define exception dispatch context structure.
//

typedef struct _DISPATCHER_CONTEXT {
    DWORD64 ControlPc;
    DWORD64 ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    DWORD64 EstablisherFrame;
    DWORD64 TargetIp;
    PCONTEXT ContextRecord;
    PEXCEPTION_ROUTINE LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    DWORD ScopeIndex;
    DWORD Fill0;
} DISPATCHER_CONTEXT, * PDISPATCHER_CONTEXT;

typedef unsigned __int64  uintptr_t; //-V677

#define WINAPI      __stdcall
typedef INT_PTR(FAR WINAPI* FARPROC)();
typedef INT_PTR(NEAR WINAPI* NEARPROC)();
typedef INT_PTR(WINAPI* PROC)();

extern "C" {
    NTSYSAPI int NTAPI strcmp(const char* str1, const char* str2);
    NTSYSAPI void* NTAPI memmove(void* destination, const void* source, size_t num);
    NTSYSAPI void* NTAPI memcpy(void* destination, const void* source, size_t num);
    NTSYSAPI void* NTAPI memset(void* ptr, int value, size_t num);
    NTSYSAPI int NTAPI memcmp ( const void * ptr1, const void * ptr2, size_t num );
    extern __int64 NTAPI _NLG_Notify(__int64 a1, __int64 a2, __int64 a3);
    extern __int64 NTAPI __NLG_Dispatch2(__int64 a, __int64 b, __int64 c, __int64 d);
    extern __int64 NTAPI __NLG_Return2();

    NTSYSAPI double __cdecl pow(double X, double Y);
    NTSYSAPI double __cdecl sin(double X);
    NTSYSAPI double __cdecl cos(double X);
    NTSYSAPI double __cdecl tan(double X);
    //NTSYSAPI double __cdecl asin(double X);
    NTSYSAPI double __cdecl atan(double X);
    NTSYSAPI double __cdecl sqrt(double x);
    NTSYSAPI double __cdecl ceil(double val);
    NTSYSAPI double __cdecl floor(double x);
    NTSYSAPI double __cdecl log(double x);
}

#define RtlEqualMemory(Destination,Source,Length) (!memcmp((Destination),(Source),(Length)))
#define RtlMoveMemory(Destination,Source,Length) memmove((Destination),(Source),(Length))
#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
#define RtlFillMemory(Destination,Length,Fill) memset((Destination),(Fill),(Length))
#define RtlZeroMemory(Destination,Length) memset((Destination),0,(Length))

#define EXCEPTION_NONCONTINUABLE 0x1    // Noncontinuable exception
#define EXCEPTION_UNWINDING 0x2         // Unwind is in progress
#define EXCEPTION_EXIT_UNWIND 0x4       // Exit unwind is in progress
#define EXCEPTION_STACK_INVALID 0x8     // Stack out of limits or unaligned
#define EXCEPTION_NESTED_CALL 0x10      // Nested exception handler call
#define EXCEPTION_TARGET_UNWIND 0x20    // Target unwind in progress
#define EXCEPTION_COLLIDED_UNWIND 0x40  // Collided exception handler call

#define EXCEPTION_UNWIND (EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND | \
                          EXCEPTION_TARGET_UNWIND | EXCEPTION_COLLIDED_UNWIND)

#define IS_UNWINDING(Flag) ((Flag & EXCEPTION_UNWIND) != 0) //-V1003
#define IS_DISPATCHING(Flag) ((Flag & EXCEPTION_UNWIND) == 0) //-V1003
#define IS_TARGET_UNWIND(Flag) (Flag & EXCEPTION_TARGET_UNWIND) //-V1003

#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters

//
// Nonvolatile context pointer record.
//

typedef struct _KNONVOLATILE_CONTEXT_POINTERS {
    union {
        PM128A FloatingContext[16];
        struct {
            PM128A Xmm0;
            PM128A Xmm1;
            PM128A Xmm2;
            PM128A Xmm3;
            PM128A Xmm4;
            PM128A Xmm5;
            PM128A Xmm6;
            PM128A Xmm7;
            PM128A Xmm8;
            PM128A Xmm9;
            PM128A Xmm10;
            PM128A Xmm11;
            PM128A Xmm12;
            PM128A Xmm13;
            PM128A Xmm14;
            PM128A Xmm15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;

    union {
        PDWORD64 IntegerContext[16];
        struct {
            PDWORD64 Rax;
            PDWORD64 Rcx;
            PDWORD64 Rdx;
            PDWORD64 Rbx;
            PDWORD64 Rsp;
            PDWORD64 Rbp;
            PDWORD64 Rsi;
            PDWORD64 Rdi;
            PDWORD64 R8;
            PDWORD64 R9;
            PDWORD64 R10;
            PDWORD64 R11;
            PDWORD64 R12;
            PDWORD64 R13;
            PDWORD64 R14;
            PDWORD64 R15;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME2;
} KNONVOLATILE_CONTEXT_POINTERS, * PKNONVOLATILE_CONTEXT_POINTERS;

FORCEINLINE
PVOID
RtlSecureZeroMemory(
    _Out_writes_bytes_all_(cnt) PVOID ptr,
    _In_ SIZE_T cnt
)
{
    volatile char* vptr = (volatile char*)ptr;

#if defined(_M_AMD64)

    __stosb((PBYTE)((DWORD64)vptr), 0, cnt);

#else

    while (cnt) {
#if !defined(_M_CEE) && (defined(_M_ARM) || defined(_M_ARM64))

        __iso_volatile_store8(vptr, 0);

#else

        * vptr = 0;

#endif

        vptr++;
        cnt--;
    }

#endif // _M_AMD64

    return ptr;
}

typedef struct _xDISPATCHER_CONTEXT_AMD64 {
    ULONG ControlPc;
    ULONG ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG EstablisherFrame;
    ULONG TargetPc;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PVOID HistoryTable;
    ULONG ScopeIndex;
    BOOLEAN ControlPcIsUnwound;
    PUCHAR NonVolatileRegisters;
} DispatcherContextAMD64;

#define DELETE                           (0x00010000L)
#define READ_CONTROL                     (0x00020000L)
#define WRITE_DAC                        (0x00040000L)
#define WRITE_OWNER                      (0x00080000L)
#define SYNCHRONIZE                      (0x00100000L)


#define STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define STANDARD_RIGHTS_READ             (READ_CONTROL)
#define STANDARD_RIGHTS_WRITE            (READ_CONTROL)
#define STANDARD_RIGHTS_EXECUTE          (READ_CONTROL)

#define STANDARD_RIGHTS_ALL              (0x001F0000L)

#define SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

//
// AccessSystemAcl access type
//

#define ACCESS_SYSTEM_SECURITY           (0x01000000L)

//
// MaximumAllowed access type
//

#define MAXIMUM_ALLOWED                  (0x02000000L)

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define UNICODE_STRING_MAX_CHARS (32767)


#define PROCESS_TERMINATE                  (0x0001)  
#define PROCESS_CREATE_THREAD              (0x0002)  
#define PROCESS_SET_SESSIONID              (0x0004)  
#define PROCESS_VM_OPERATION               (0x0008)  
#define PROCESS_VM_READ                    (0x0010)  
#define PROCESS_VM_WRITE                   (0x0020)  
#define PROCESS_DUP_HANDLE                 (0x0040)  
#define PROCESS_CREATE_PROCESS             (0x0080)  
#define PROCESS_SET_QUOTA                  (0x0100)  
#define PROCESS_SET_INFORMATION            (0x0200)  
#define PROCESS_QUERY_INFORMATION          (0x0400)  
#define PROCESS_SUSPEND_RESUME             (0x0800)  
#define PROCESS_QUERY_LIMITED_INFORMATION  (0x1000)  
#define PROCESS_SET_LIMITED_INFORMATION    (0x2000)  
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFFF)
#else
#define PROCESS_ALL_ACCESS        (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFF)
#endif

#define THREAD_TERMINATE                 (0x0001)  
#define THREAD_SUSPEND_RESUME            (0x0002)  
#define THREAD_GET_CONTEXT               (0x0008)  
#define THREAD_SET_CONTEXT               (0x0010)  
#define THREAD_QUERY_INFORMATION         (0x0040)  
#define THREAD_SET_INFORMATION           (0x0020)  
#define THREAD_SET_THREAD_TOKEN          (0x0080)
#define THREAD_IMPERSONATE               (0x0100)
#define THREAD_DIRECT_IMPERSONATION      (0x0200)
// begin_wdm
#define THREAD_SET_LIMITED_INFORMATION   (0x0400)  // winnt
#define THREAD_QUERY_LIMITED_INFORMATION (0x0800)  // winnt
#define THREAD_RESUME                    (0x1000)  // winnt
#if (NTDDI_VERSION >= NTDDI_VISTA)
#define THREAD_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0xFFFF)
#else
#define THREAD_ALL_ACCESS         (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                   0x3FF)
#endif
#define JOB_OBJECT_ASSIGN_PROCESS           (0x0001)
#define JOB_OBJECT_SET_ATTRIBUTES           (0x0002)
#define JOB_OBJECT_QUERY                    (0x0004)
#define JOB_OBJECT_TERMINATE                (0x0008)
#define JOB_OBJECT_SET_SECURITY_ATTRIBUTES  (0x0010)
#define JOB_OBJECT_IMPERSONATE              (0x0020)
#define JOB_OBJECT_ALL_ACCESS       (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | \
                                        0x3F )

//
// Token Specific Access Rights.
//

#define TOKEN_ASSIGN_PRIMARY    (0x0001)
#define TOKEN_DUPLICATE         (0x0002)
#define TOKEN_IMPERSONATE       (0x0004)
#define TOKEN_QUERY             (0x0008)
#define TOKEN_QUERY_SOURCE      (0x0010)
#define TOKEN_ADJUST_PRIVILEGES (0x0020)
#define TOKEN_ADJUST_GROUPS     (0x0040)
#define TOKEN_ADJUST_DEFAULT    (0x0080)
#define TOKEN_ADJUST_SESSIONID  (0x0100)

#define TOKEN_ALL_ACCESS_P (STANDARD_RIGHTS_REQUIRED  |\
                          TOKEN_ASSIGN_PRIMARY      |\
                          TOKEN_DUPLICATE           |\
                          TOKEN_IMPERSONATE         |\
                          TOKEN_QUERY               |\
                          TOKEN_QUERY_SOURCE        |\
                          TOKEN_ADJUST_PRIVILEGES   |\
                          TOKEN_ADJUST_GROUPS       |\
                          TOKEN_ADJUST_DEFAULT )

#if ((defined(_WIN32_WINNT) && (_WIN32_WINNT > 0x0400)) || (!defined(_WIN32_WINNT)))
#define TOKEN_ALL_ACCESS  (TOKEN_ALL_ACCESS_P |\
                          TOKEN_ADJUST_SESSIONID )
#else
#define TOKEN_ALL_ACCESS  (TOKEN_ALL_ACCESS_P)
#endif

#define TOKEN_READ       (STANDARD_RIGHTS_READ      |\
                          TOKEN_QUERY)


#define TOKEN_WRITE      (STANDARD_RIGHTS_WRITE     |\
                          TOKEN_ADJUST_PRIVILEGES   |\
                          TOKEN_ADJUST_GROUPS       |\
                          TOKEN_ADJUST_DEFAULT)

#define TOKEN_EXECUTE    (STANDARD_RIGHTS_EXECUTE)

#define TOKEN_TRUST_CONSTRAINT_MASK    (STANDARD_RIGHTS_READ  | \
                                       TOKEN_QUERY  |\
                                       TOKEN_QUERY_SOURCE )

//
// Allocate the System Luid.  The first 1000 LUIDs are reserved.
// Use #999 here (0x3e7 = 999)
//

#define SYSTEM_LUID                     { 0x3e7, 0x0 }
#define ANONYMOUS_LOGON_LUID            { 0x3e6, 0x0 }
#define LOCALSERVICE_LUID               { 0x3e5, 0x0 }
#define NETWORKSERVICE_LUID             { 0x3e4, 0x0 }
#define IUSER_LUID                      { 0x3e3, 0x0 }
#define PROTECTED_TO_SYSTEM_LUID        { 0x3e2, 0x0 }


typedef struct _IMAGE_DOS_HEADER {      // DOS .EXE header
    WORD   e_magic;                     // Magic number
    WORD   e_cblp;                      // Bytes on last page of file
    WORD   e_cp;                        // Pages in file
    WORD   e_crlc;                      // Relocations
    WORD   e_cparhdr;                   // Size of header in paragraphs
    WORD   e_minalloc;                  // Minimum extra paragraphs needed
    WORD   e_maxalloc;                  // Maximum extra paragraphs needed
    WORD   e_ss;                        // Initial (relative) SS value
    WORD   e_sp;                        // Initial SP value
    WORD   e_csum;                      // Checksum
    WORD   e_ip;                        // Initial IP value
    WORD   e_cs;                        // Initial (relative) CS value
    WORD   e_lfarlc;                    // File address of relocation table
    WORD   e_ovno;                      // Overlay number
    WORD   e_res[4];                    // Reserved words
    WORD   e_oemid;                     // OEM identifier (for e_oeminfo)
    WORD   e_oeminfo;                   // OEM information; e_oemid specific
    WORD   e_res2[10];                  // Reserved words
    LONG   e_lfanew;                    // File address of new exe header
} IMAGE_DOS_HEADER, * PIMAGE_DOS_HEADER;

#define CALLBACK    __stdcall

//
// Import Format
//

//@[comment("MVI_tracked")]
typedef struct _IMAGE_IMPORT_BY_NAME {
    WORD    Hint;
    CHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, * PIMAGE_IMPORT_BY_NAME;

#pragma warning(push)
#pragma warning(disable:4103)
#pragma pack(push,8)       // Use align 8 for the 64-bit IAT.

typedef struct _IMAGE_THUNK_DATA64 {
    union {
        ULONGLONG ForwarderString;  // PBYTE
        ULONGLONG Function;         // PDWORD
        ULONGLONG Ordinal;
        ULONGLONG AddressOfData;    // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA64;
typedef IMAGE_THUNK_DATA64* PIMAGE_THUNK_DATA64;

#pragma pack(pop)                       // Back to 4 byte packing
#pragma warning(pop)

typedef struct _IMAGE_THUNK_DATA32 {
    union {
        DWORD ForwarderString;      // PBYTE
        DWORD Function;             // PDWORD
        DWORD Ordinal;
        DWORD AddressOfData;        // PIMAGE_IMPORT_BY_NAME
    } u1;
} IMAGE_THUNK_DATA32;
typedef IMAGE_THUNK_DATA32* PIMAGE_THUNK_DATA32;

#define IMAGE_ORDINAL_FLAG64 0x8000000000000000
#define IMAGE_ORDINAL_FLAG32 0x80000000
#define IMAGE_ORDINAL64(Ordinal) (Ordinal & 0xffff) //-V1003
#define IMAGE_ORDINAL32(Ordinal) (Ordinal & 0xffff) //-V1003
#define IMAGE_SNAP_BY_ORDINAL64(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG64) != 0) //-V1003
#define IMAGE_SNAP_BY_ORDINAL32(Ordinal) ((Ordinal & IMAGE_ORDINAL_FLAG32) != 0) //-V1003

//
// Thread Local Storage
//

typedef VOID
(NTAPI* PIMAGE_TLS_CALLBACK) (
    PVOID DllHandle,
    DWORD Reason,
    PVOID Reserved
    );

typedef struct _IMAGE_TLS_DIRECTORY64 {
    ULONGLONG StartAddressOfRawData;
    ULONGLONG EndAddressOfRawData;
    ULONGLONG AddressOfIndex;         // PDWORD
    ULONGLONG AddressOfCallBacks;     // PIMAGE_TLS_CALLBACK *;
    DWORD SizeOfZeroFill;
    union {
        DWORD Characteristics;
        struct {
            DWORD Reserved0 : 20;
            DWORD Alignment : 4;
            DWORD Reserved1 : 8;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} IMAGE_TLS_DIRECTORY64;

typedef IMAGE_TLS_DIRECTORY64* PIMAGE_TLS_DIRECTORY64;

typedef struct _IMAGE_TLS_DIRECTORY32 {
    DWORD   StartAddressOfRawData;
    DWORD   EndAddressOfRawData;
    DWORD   AddressOfIndex;             // PDWORD
    DWORD   AddressOfCallBacks;         // PIMAGE_TLS_CALLBACK *
    DWORD   SizeOfZeroFill;
    union {
        DWORD Characteristics;
        struct {
            DWORD Reserved0 : 20;
            DWORD Alignment : 4;
            DWORD Reserved1 : 8;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} IMAGE_TLS_DIRECTORY32;
typedef IMAGE_TLS_DIRECTORY32* PIMAGE_TLS_DIRECTORY32;

#ifdef _WIN64
#define IMAGE_ORDINAL_FLAG              IMAGE_ORDINAL_FLAG64
#define IMAGE_ORDINAL(Ordinal)          IMAGE_ORDINAL64(Ordinal)
typedef IMAGE_THUNK_DATA64              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA64             PIMAGE_THUNK_DATA;
#define IMAGE_SNAP_BY_ORDINAL(Ordinal)  IMAGE_SNAP_BY_ORDINAL64(Ordinal)
typedef IMAGE_TLS_DIRECTORY64           IMAGE_TLS_DIRECTORY;
typedef PIMAGE_TLS_DIRECTORY64          PIMAGE_TLS_DIRECTORY;
#else
#define IMAGE_ORDINAL_FLAG              IMAGE_ORDINAL_FLAG32
#define IMAGE_ORDINAL(Ordinal)          IMAGE_ORDINAL32(Ordinal)
typedef IMAGE_THUNK_DATA32              IMAGE_THUNK_DATA;
typedef PIMAGE_THUNK_DATA32             PIMAGE_THUNK_DATA;
#define IMAGE_SNAP_BY_ORDINAL(Ordinal)  IMAGE_SNAP_BY_ORDINAL32(Ordinal)
typedef IMAGE_TLS_DIRECTORY32           IMAGE_TLS_DIRECTORY;
typedef PIMAGE_TLS_DIRECTORY32          PIMAGE_TLS_DIRECTORY;
#endif

#define DLL_PROCESS_ATTACH   1
#define DLL_THREAD_ATTACH    2
#define DLL_THREAD_DETACH    3
#define DLL_PROCESS_DETACH   0

#define CONTEXT_AMD64   0x00100000L

// end_wx86

#define CONTEXT_CONTROL         (CONTEXT_AMD64 | 0x00000001L)
#define CONTEXT_INTEGER         (CONTEXT_AMD64 | 0x00000002L)
#define CONTEXT_SEGMENTS        (CONTEXT_AMD64 | 0x00000004L)
#define CONTEXT_FLOATING_POINT  (CONTEXT_AMD64 | 0x00000008L)
#define CONTEXT_DEBUG_REGISTERS (CONTEXT_AMD64 | 0x00000010L)

#define CONTEXT_FULL            (CONTEXT_CONTROL | CONTEXT_INTEGER | \
                                 CONTEXT_FLOATING_POINT)

#define CONTEXT_ALL             (CONTEXT_CONTROL | CONTEXT_INTEGER | \
                                 CONTEXT_SEGMENTS | CONTEXT_FLOATING_POINT | \
                                 CONTEXT_DEBUG_REGISTERS)

#define CONTEXT_XSTATE          (CONTEXT_AMD64 | 0x00000040L)

#pragma warning(pop)

#ifdef _MAC
#define CALLBACK    PASCAL
#define WINAPI      CDECL
#define WINAPIV     CDECL
#define APIENTRY    WINAPI
#define APIPRIVATE  CDECL
#ifdef _68K_
#define PASCAL      __pascal
#else
#define PASCAL
#endif
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#else
#ifndef CALLBACK
#define CALLBACK
#endif
#ifndef CALLBACK
#define WINAPI
#endif
#define WINAPIV
#define APIENTRY    WINAPI
#define APIPRIVATE
#define PASCAL      pascal
#endif

// Update Sequence Number

typedef LONGLONG USN; //-V677

#define SECTION_QUERY                0x0001
#define SECTION_MAP_WRITE            0x0002
#define SECTION_MAP_READ             0x0004
#define SECTION_MAP_EXECUTE          0x0008
#define SECTION_EXTEND_SIZE          0x0010
#define SECTION_MAP_EXECUTE_EXPLICIT 0x0020 // not included in SECTION_ALL_ACCESS

#define SECTION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SECTION_QUERY|\
                            SECTION_MAP_WRITE |      \
                            SECTION_MAP_READ |       \
                            SECTION_MAP_EXECUTE |    \
                            SECTION_EXTEND_SIZE)

//
// Session Specific Access Rights.
//

#define SESSION_QUERY_ACCESS  0x0001
#define SESSION_MODIFY_ACCESS 0x0002

#define SESSION_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED |  \
                            SESSION_QUERY_ACCESS |             \
                            SESSION_MODIFY_ACCESS)

// end_access
#define PAGE_NOACCESS           0x01    
#define PAGE_READONLY           0x02    
#define PAGE_READWRITE          0x04    
#define PAGE_WRITECOPY          0x08    
#define PAGE_EXECUTE            0x10    
#define PAGE_EXECUTE_READ       0x20    
#define PAGE_EXECUTE_READWRITE  0x40    
#define PAGE_EXECUTE_WRITECOPY  0x80    
#define PAGE_GUARD             0x100    
#define PAGE_NOCACHE           0x200    
#define PAGE_WRITECOMBINE      0x400    
#define PAGE_ENCLAVE_THREAD_CONTROL 0x80000000  
#define PAGE_REVERT_TO_FILE_MAP     0x80000000  
#define PAGE_TARGETS_NO_UPDATE      0x40000000  
#define PAGE_TARGETS_INVALID        0x40000000  
#define MEM_COMMIT                      0x00001000  
#define MEM_RESERVE                     0x00002000  
#define MEM_REPLACE_PLACEHOLDER         0x00004000  
#define MEM_RESERVE_PLACEHOLDER         0x00040000  
#define MEM_RESET                       0x00080000  
#define MEM_TOP_DOWN                    0x00100000  
#define MEM_WRITE_WATCH                 0x00200000  
#define MEM_PHYSICAL                    0x00400000  
#define MEM_ROTATE                      0x00800000  
#define MEM_DIFFERENT_IMAGE_BASE_OK     0x00800000  
#define MEM_RESET_UNDO                  0x01000000  
#define MEM_LARGE_PAGES                 0x20000000  
#define MEM_4MB_PAGES                   0x80000000  
#define MEM_64K_PAGES                   (MEM_LARGE_PAGES | MEM_PHYSICAL)  
#define MEM_UNMAP_WITH_TRANSIENT_BOOST  0x00000001  
#define MEM_COALESCE_PLACEHOLDERS       0x00000001  
#define MEM_PRESERVE_PLACEHOLDER        0x00000002  
#define MEM_DECOMMIT                    0x00004000  
#define MEM_RELEASE                     0x00008000  
#define MEM_FREE                        0x00010000 

#define SEC_PARTITION_OWNER_HANDLE  0x00040000  
#define SEC_64K_PAGES               0x00080000  
#define SEC_FILE                    0x00800000  
#define SEC_IMAGE                   0x01000000  
#define SEC_PROTECTED_IMAGE         0x02000000  
#define SEC_RESERVE                 0x04000000  
#define SEC_COMMIT                  0x08000000  
#define SEC_NOCACHE                 0x10000000  
#define SEC_WRITECOMBINE            0x40000000  
#define SEC_LARGE_PAGES             0x80000000  
#define SEC_IMAGE_NO_EXECUTE (SEC_IMAGE | SEC_NOCACHE) 

//
// Define the NamedPipe definitions
//


//
// Define the dwOpenMode values for CreateNamedPipe
//

#define PIPE_ACCESS_INBOUND         0x00000001
#define PIPE_ACCESS_OUTBOUND        0x00000002
#define PIPE_ACCESS_DUPLEX          0x00000003

//
// Define the Named Pipe End flags for GetNamedPipeInfo
//

#define PIPE_CLIENT_END             0x00000000
#define PIPE_SERVER_END             0x00000001

//
// Define the dwPipeMode values for CreateNamedPipe
//

#define PIPE_WAIT                   0x00000000
#define PIPE_NOWAIT                 0x00000001
#define PIPE_READMODE_BYTE          0x00000000
#define PIPE_READMODE_MESSAGE       0x00000002
#define PIPE_TYPE_BYTE              0x00000000
#define PIPE_TYPE_MESSAGE           0x00000004
#define PIPE_ACCEPT_REMOTE_CLIENTS  0x00000000
#define PIPE_REJECT_REMOTE_CLIENTS  0x00000008

//
// Define the well known values for CreateNamedPipe nMaxInstances
//

#define PIPE_UNLIMITED_INSTANCES    255

//
// Define the Security Quality of Service bits to be passed
// into CreateFile
//

typedef /* [wire_marshal] */ void* HDC; //-V677

//
// Debug APIs
//
#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef DWORD(WINAPI* PTHREAD_START_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef LPVOID(WINAPI* PENCLAVE_ROUTINE)(
    LPVOID lpThreadParameter
    );
typedef PENCLAVE_ROUTINE LPENCLAVE_ROUTINE;

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO, * LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, * LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO, * LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO, * LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, * LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO, * LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, * LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD fUnicode;
    WORD nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, * LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO, * LPRIP_INFO;


typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, * LPDEBUG_EVENT;

//
// Defines for power request APIs
//

#define POWER_REQUEST_CONTEXT_VERSION               DIAGNOSTIC_REASON_VERSION

#define POWER_REQUEST_CONTEXT_SIMPLE_STRING         DIAGNOSTIC_REASON_SIMPLE_STRING
#define POWER_REQUEST_CONTEXT_DETAILED_STRING       DIAGNOSTIC_REASON_DETAILED_STRING

typedef enum _POWER_REQUEST_TYPE {
    PowerRequestDisplayRequired,
    PowerRequestSystemRequired,
    PowerRequestAwayModeRequired,
    PowerRequestExecutionRequired
} POWER_REQUEST_TYPE, * PPOWER_REQUEST_TYPE;

#define ANSI_NULL ((CHAR)0)     
#define UNICODE_NULL ((WCHAR)0) 
#define UNICODE_STRING_MAX_BYTES ((WORD  ) 65534) 
#define UNICODE_STRING_MAX_CHARS (32767) 

typedef VOID
(NTAPI* PFLS_CALLBACK_FUNCTION) (
    PVOID lpFlsData
    );


typedef enum MEM_EXTENDED_PARAMETER_TYPE {
    MemExtendedParameterInvalidType = 0,
    MemExtendedParameterAddressRequirements,
    MemExtendedParameterNumaNode,
    MemExtendedParameterPartitionHandle,
    MemExtendedParameterUserPhysicalHandle,
    MemExtendedParameterAttributeFlags,
    MemExtendedParameterMax
} MEM_EXTENDED_PARAMETER_TYPE, * PMEM_EXTENDED_PARAMETER_TYPE;

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, * PFILETIME, * LPFILETIME;

typedef /* [wire_marshal] */ void* HWND; //-V677

#define MEM_EXTENDED_PARAMETER_TYPE_BITS    8

typedef struct DECLSPEC_ALIGN(8) MEM_EXTENDED_PARAMETER {

    struct {
        DWORD64 Type : MEM_EXTENDED_PARAMETER_TYPE_BITS;
        DWORD64 Reserved : 64 - MEM_EXTENDED_PARAMETER_TYPE_BITS;
    } DUMMYSTRUCTNAME;

    union {
        DWORD64 ULong64;
        PVOID Pointer;
        SIZE_T Size;
        HANDLE Handle;
        DWORD ULong;
    } DUMMYUNIONNAME;

} MEM_EXTENDED_PARAMETER, * PMEM_EXTENDED_PARAMETER;

//
//  *** Claim Security attributes ***
//
//      These #defines and data structures (almost) exactly mirror
//      the Token_XXX definitions (except for PWSTR/PUNICODE changes)
//      in ntseapi.w as well as AUTHZ_XXX in authz.w. 
//      Keep them in sync. 
//
//
//  Security attribute data types ...
//

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_INVALID   0x00

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_INT64     0x01
#define CLAIM_SECURITY_ATTRIBUTE_TYPE_UINT64    0x02

//
//  Case insensitive attribute value string by default.
//  Unless the flag CLAIM_SECURITY_ATTRIBUTE_VALUE_CASE_SENSITIVE
//  is set indicating otherwise.
//

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_STRING    0x03

//
//  Fully-qualified binary name.
//

typedef struct _CLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE {
    DWORD64             Version;
    PWSTR               Name;
} CLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE, * PCLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE;

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_FQBN      0x04

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_SID       0x05

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_BOOLEAN   0x06


typedef struct _CLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE {
    PVOID   pValue;         //  Pointer is BYTE aligned.
    DWORD   ValueLength;    //  In bytes
} CLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE,
* PCLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE;

#define CLAIM_SECURITY_ATTRIBUTE_TYPE_OCTET_STRING  0x10

//
// Attribute Flags
//

//
//  Attribute must not be inherited across process spawns.
//

#define CLAIM_SECURITY_ATTRIBUTE_NON_INHERITABLE      0x0001


//
//  Attribute value is compared in a case sensitive way. It is valid with string value
//  or composite type containing string value. For other types of value, this flag
//  will be ignored. Currently, it is valid with the two types:
//  CLAIM_SECURITY_ATTRIBUTE_TYPE_STRING and CLAIM_SECURITY_ATTRIBUTE_TYPE_FQBN.
//
#define CLAIM_SECURITY_ATTRIBUTE_VALUE_CASE_SENSITIVE         0x0002

//
// Attribute is considered only for Deny Aces.
//

#define CLAIM_SECURITY_ATTRIBUTE_USE_FOR_DENY_ONLY 0x0004

//
// Attribute is disabled by default.
//

#define CLAIM_SECURITY_ATTRIBUTE_DISABLED_BY_DEFAULT 0x0008

// 
// Attribute is disabled.
//

#define CLAIM_SECURITY_ATTRIBUTE_DISABLED 0x0010

//
// Attribute is mandatory.
//

#define CLAIM_SECURITY_ATTRIBUTE_MANDATORY 0x0020


#define CLAIM_SECURITY_ATTRIBUTE_VALID_FLAGS   (    \
                        CLAIM_SECURITY_ATTRIBUTE_NON_INHERITABLE       |  \
                        CLAIM_SECURITY_ATTRIBUTE_VALUE_CASE_SENSITIVE  |  \
                        CLAIM_SECURITY_ATTRIBUTE_USE_FOR_DENY_ONLY     |  \
                        CLAIM_SECURITY_ATTRIBUTE_DISABLED_BY_DEFAULT   |  \
                        CLAIM_SECURITY_ATTRIBUTE_DISABLED              |  \
                        CLAIM_SECURITY_ATTRIBUTE_MANDATORY )


//
// Reserve upper 16 bits for custom flags. These should be preserved but not
// validated as they do not affect security in any way.
//
#define CLAIM_SECURITY_ATTRIBUTE_CUSTOM_FLAGS   0xFFFF0000

//
//  An individual security attribute.
//


typedef struct _CLAIM_SECURITY_ATTRIBUTE_V1 {

    //
    //  Name of the attribute.
    //  Case insensitive Unicode string.
    //

    PWSTR   Name;

    //
    //  Data type of attribute.
    //

    WORD    ValueType;

    //
    //  Pass 0 in a set operation and check for 0 in
    //  a get operation.
    //

    WORD    Reserved;

    //
    // Attribute Flags
    //

    DWORD   Flags;

    //
    //  Number of values.
    //

    DWORD   ValueCount;

    //
    //  The actual value itself.
    //

    union {
        PLONG64                                         pInt64;
        PDWORD64                                        pUint64;
        PWSTR* ppString;
        PCLAIM_SECURITY_ATTRIBUTE_FQBN_VALUE            pFqbn;
        PCLAIM_SECURITY_ATTRIBUTE_OCTET_STRING_VALUE    pOctetString;
    } Values;
} CLAIM_SECURITY_ATTRIBUTE_V1, * PCLAIM_SECURITY_ATTRIBUTE_V1;

//
//  Relative form of the security attribute.
//


typedef struct _CLAIM_SECURITY_ATTRIBUTE_RELATIVE_V1 {

    //
    //  Name of the attribute.
    //  Offset from beginning of structure.
    //

    DWORD   Name;

    //
    //  Data type of attribute.
    //

    WORD    ValueType;

    //
    //  Pass 0 in a set operation and check for 0 in
    //  a get operation.
    //

    WORD    Reserved;

    //
    // Attribute Flags
    //

    DWORD   Flags;

    //
    //  Number of values.
    //

    DWORD   ValueCount;

    //
    //  The actual value itself.
    //

    union {
        DWORD pInt64[ANYSIZE_ARRAY];
        DWORD pUint64[ANYSIZE_ARRAY];
        DWORD ppString[ANYSIZE_ARRAY];
        DWORD pFqbn[ANYSIZE_ARRAY];
        DWORD pOctetString[ANYSIZE_ARRAY];
    } Values;
} CLAIM_SECURITY_ATTRIBUTE_RELATIVE_V1, * PCLAIM_SECURITY_ATTRIBUTE_RELATIVE_V1;


//
//  Set of security attributes.
//

//
//  Versioning. The interpretation of the pointers in the
//  Attribute field below is dependent on the version field.
//
//  Get operations return the version while the set operation
//  MUST specify the version of the data structure passed in.
//

#define CLAIM_SECURITY_ATTRIBUTES_INFORMATION_VERSION_V1    1

#define CLAIM_SECURITY_ATTRIBUTES_INFORMATION_VERSION       \
    CLAIM_SECURITY_ATTRIBUTES_INFORMATION_VERSION_V1



typedef struct _CLAIM_SECURITY_ATTRIBUTES_INFORMATION {

    //
    //  MUST BE first.
    //

    WORD    Version;

    //
    //  Pass 0 in set operations and ignore on get operations.
    //

    WORD    Reserved;

    DWORD   AttributeCount;
    union {
        PCLAIM_SECURITY_ATTRIBUTE_V1    pAttributeV1;
    } Attribute;
} CLAIM_SECURITY_ATTRIBUTES_INFORMATION, * PCLAIM_SECURITY_ATTRIBUTES_INFORMATION;

typedef struct DECLSPEC_ALIGN(8) _XSAVE_AREA_HEADER {
    ULONG64 Mask;
    ULONG64 Reserved[7];
} XSAVE_AREA_HEADER, * PXSAVE_AREA_HEADER;

__forceinline
struct _TEB*
    NtCurrentTeb(
        VOID
    )

{
    return (struct _TEB*)__readgsqword(FIELD_OFFSET(NT_TIB, Self));
}

__forceinline
PVOID
GetCurrentFiber(
    VOID
)

{

    return (PVOID)__readgsqword(FIELD_OFFSET(NT_TIB, FiberData));
}

__forceinline
PVOID
GetFiberData(
    VOID
)

{

    return *(PVOID*)GetCurrentFiber();
}


//
// Registry Specific Access Rights.
//

#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)
#define KEY_WOW64_32KEY         (0x0200)
#define KEY_WOW64_64KEY         (0x0100)
#define KEY_WOW64_RES           (0x0300)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

// end_access

//
// Open/Create Options
//

#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved

#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved
                                                    // when system is rebooted

#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved
                                                    // when system is rebooted

#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a
                                                    // symbolic link

#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore
                                                    // special access rules
                                                    // privilege required

#define REG_OPTION_OPEN_LINK        (0x00000008L)   // Open symbolic link

#define REG_OPTION_DONT_VIRTUALIZE  (0x00000010L)   // Disable Open/Read/Write
                                                    // virtualization for this
                                                    // open and the resulting
                                                    // handle.

#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK           |\
                 REG_OPTION_DONT_VIRTUALIZE)

#define REG_OPEN_LEGAL_OPTION       \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK           |\
                 REG_OPTION_DONT_VIRTUALIZE)

//
// Key creation/open disposition
//

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

//
// hive format to be used by Reg(Nt)SaveKeyEx
//
#define REG_STANDARD_FORMAT     1
#define REG_LATEST_FORMAT       2
#define REG_NO_COMPRESSION      4

//
// Key restore & hive load flags
//

#define REG_WHOLE_HIVE_VOLATILE         (0x00000001L)   // Restore whole hive volatile
#define REG_REFRESH_HIVE                (0x00000002L)   // Unwind changes to last flush
#define REG_NO_LAZY_FLUSH               (0x00000004L)   // Never lazy flush this hive
#define REG_FORCE_RESTORE               (0x00000008L)   // Force the restore process even when we have open handles on subkeys
#define REG_APP_HIVE                    (0x00000010L)   // Loads the hive visible to the calling process
#define REG_PROCESS_PRIVATE             (0x00000020L)   // Hive cannot be mounted by any other process while in use
#define REG_START_JOURNAL               (0x00000040L)   // Starts Hive Journal
#define REG_HIVE_EXACT_FILE_GROWTH      (0x00000080L)   // Grow hive file in exact 4k increments
#define REG_HIVE_NO_RM                  (0x00000100L)   // No RM is started for this hive (no transactions)
#define REG_HIVE_SINGLE_LOG             (0x00000200L)   // Legacy single logging is used for this hive
#define REG_BOOT_HIVE                   (0x00000400L)   // This hive might be used by the OS loader
#define REG_LOAD_HIVE_OPEN_HANDLE       (0x00000800L)   // Load the hive and return a handle to its root kcb
#define REG_FLUSH_HIVE_FILE_GROWTH      (0x00001000L)   // Flush changes to primary hive file size as part of all flushes
#define REG_OPEN_READ_ONLY              (0x00002000L)   // Open a hive's files in read-only mode
#define REG_IMMUTABLE                   (0x00004000L)   // Load the hive, but don't allow any modification of it
#define REG_APP_HIVE_OPEN_READ_ONLY     (REG_OPEN_READ_ONLY)   // Open an app hive's files in read-only mode (if the hive was not previously loaded)

//
// Unload Flags
//
#define REG_FORCE_UNLOAD            1
#define REG_UNLOAD_LEGAL_FLAGS      (REG_FORCE_UNLOAD)

//
// Notify filter values
//

#define REG_NOTIFY_CHANGE_NAME          (0x00000001L) // Create or delete (child)
#define REG_NOTIFY_CHANGE_ATTRIBUTES    (0x00000002L)
#define REG_NOTIFY_CHANGE_LAST_SET      (0x00000004L) // time stamp
#define REG_NOTIFY_CHANGE_SECURITY      (0x00000008L)
#define REG_NOTIFY_THREAD_AGNOSTIC      (0x10000000L) // Not associated with a calling thread, can only be used
                                                      // for async user event based notification

#define REG_LEGAL_CHANGE_FILTER                 \
                (REG_NOTIFY_CHANGE_NAME          |\
                 REG_NOTIFY_CHANGE_ATTRIBUTES    |\
                 REG_NOTIFY_CHANGE_LAST_SET      |\
                 REG_NOTIFY_CHANGE_SECURITY      |\
                 REG_NOTIFY_THREAD_AGNOSTIC)

// end_wdm

//
//
// Predefined Value Types.
//

#define REG_NONE                    ( 0ul ) // No value type
#define REG_SZ                      ( 1ul ) // Unicode nul terminated string
#define REG_EXPAND_SZ               ( 2ul ) // Unicode nul terminated string
                                            // (with environment variable references)
#define REG_BINARY                  ( 3ul ) // Free form binary
#define REG_DWORD                   ( 4ul ) // 32-bit number
#define REG_DWORD_LITTLE_ENDIAN     ( 4ul ) // 32-bit number (same as REG_DWORD)
#define REG_DWORD_BIG_ENDIAN        ( 5ul ) // 32-bit number
#define REG_LINK                    ( 6ul ) // Symbolic Link (unicode)
#define REG_MULTI_SZ                ( 7ul ) // Multiple Unicode strings
#define REG_RESOURCE_LIST           ( 8ul ) // Resource list in the resource map
#define REG_FULL_RESOURCE_DESCRIPTOR ( 9ul ) // Resource list in the hardware description
#define REG_RESOURCE_REQUIREMENTS_LIST ( 10ul )
#define REG_QWORD                   ( 11ul ) // 64-bit number
#define REG_QWORD_LITTLE_ENDIAN     ( 11ul ) // 64-bit number (same as REG_QWORD)

// end_wdm

#define HEAP_NO_SERIALIZE               0x00000001      
#define HEAP_GROWABLE                   0x00000002      
#define HEAP_GENERATE_EXCEPTIONS        0x00000004      
#define HEAP_ZERO_MEMORY                0x00000008      
#define HEAP_REALLOC_IN_PLACE_ONLY      0x00000010      
#define HEAP_TAIL_CHECKING_ENABLED      0x00000020      
#define HEAP_FREE_CHECKING_ENABLED      0x00000040      
#define HEAP_DISABLE_COALESCE_ON_FREE   0x00000080      
#define HEAP_CREATE_ALIGN_16            0x00010000      
#define HEAP_CREATE_ENABLE_TRACING      0x00020000      
#define HEAP_CREATE_ENABLE_EXECUTE      0x00040000      
#define HEAP_MAXIMUM_TAG                0x0FFF              
#define HEAP_PSEUDO_TAG_FLAG            0x8000              
#define HEAP_TAG_SHIFT                  18                  
#define HEAP_CREATE_SEGMENT_HEAP        0x00000100      
#define HEAP_CREATE_HARDENED            0x00000200  

//
//  Crc32 and Crc64 routines that use standardized algorithms
//
extern "C" {
    NTSYSAPI
        DWORD
        NTAPI
        RtlCrc32(
            _In_reads_bytes_(Size) const void* Buffer,
            _In_ size_t Size,
            _In_ DWORD InitialCrc
        );

    NTSYSAPI
        ULONGLONG
        NTAPI
        RtlCrc64(
            _In_reads_bytes_(Size) const void* Buffer,
            _In_ size_t Size,
            _In_ ULONGLONG InitialCrc
        );

    DECLSPEC_NORETURN
    NTSYSAPI
        VOID
        NTAPI
        RtlUnwindEx(
            _In_opt_ PVOID TargetFrame,
            _In_opt_ PVOID TargetIp,
            _In_opt_ PEXCEPTION_RECORD ExceptionRecord,
            _In_ PVOID ReturnValue,
            _In_ PCONTEXT ContextRecord,
            _In_opt_ PUNWIND_HISTORY_TABLE HistoryTable
        );


    NTSYSAPI
        PEXCEPTION_ROUTINE
        NTAPI
        RtlVirtualUnwind(
            _In_ DWORD HandlerType,
            _In_ DWORD64 ImageBase,
            _In_ DWORD64 ControlPc,
            _In_ PRUNTIME_FUNCTION FunctionEntry,
            _Inout_ PCONTEXT ContextRecord,
            _Out_ PVOID* HandlerData,
            _Out_ PDWORD64 EstablisherFrame,
            _Inout_opt_ PKNONVOLATILE_CONTEXT_POINTERS ContextPointers
        );
}

typedef RTL_SRWLOCK SRWLOCK, * PSRWLOCK;


typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;
    PVOID AllocationBase;
    DWORD AllocationProtect;
#if defined (_WIN64)
    WORD   PartitionId;
#endif
    SIZE_T RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION, * PMEMORY_BASIC_INFORMATION;

// end_ntifs

typedef struct _MEMORY_BASIC_INFORMATION32 {
    DWORD BaseAddress;
    DWORD AllocationBase;
    DWORD AllocationProtect;
    DWORD RegionSize;
    DWORD State;
    DWORD Protect;
    DWORD Type;
} MEMORY_BASIC_INFORMATION32, * PMEMORY_BASIC_INFORMATION32;

typedef struct DECLSPEC_ALIGN(16) _MEMORY_BASIC_INFORMATION64 {
    ULONGLONG BaseAddress;
    ULONGLONG AllocationBase;
    DWORD     AllocationProtect;
    DWORD     __alignment1;
    ULONGLONG RegionSize;
    DWORD     State;
    DWORD     Protect;
    DWORD     Type;
    DWORD     __alignment2;
} MEMORY_BASIC_INFORMATION64, * PMEMORY_BASIC_INFORMATION64;

#define EXTERN_C    extern "C"
#define DECLSPEC_SELECTANY  __declspec(selectany)

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID DECLSPEC_SELECTANY name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

//
// Enum which defines the effective altitude of a power setting.
//

typedef enum POWER_SETTING_ALTITUDE {
    ALTITUDE_GROUP_POLICY,
    ALTITUDE_USER,
    ALTITUDE_RUNTIME_OVERRIDE,
    ALTITUDE_PROVISIONING,
    ALTITUDE_OEM_CUSTOMIZATION,
    ALTITUDE_INTERNAL_OVERRIDE,
    ALTITUDE_OS_DEFAULT,
} POWER_SETTING_ALTITUDE, * PPOWER_SETTING_ALTITUDE;

//
// System power manager capabilities
//

#if (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_)
typedef struct {
    DWORD       Granularity;
    DWORD       Capacity;
} BATTERY_REPORTING_SCALE, * PBATTERY_REPORTING_SCALE;
#endif // (NTDDI_VERSION >= NTDDI_WINXP) || !defined(_BATCLASS_)

//

typedef struct {
    DWORD   Frequency;
    DWORD   Flags;
    DWORD   PercentFrequency;
} PPM_WMI_LEGACY_PERFSTATE, * PPPM_WMI_LEGACY_PERFSTATE;

typedef struct {
    DWORD Latency;
    DWORD Power;
    DWORD TimeCheck;
    BYTE  PromotePercent;
    BYTE  DemotePercent;
    BYTE  StateType;
    BYTE  Reserved;
    DWORD StateFlags;
    DWORD Context;
    DWORD IdleHandler;
    DWORD Reserved1;            // reserved for future use
} PPM_WMI_IDLE_STATE, * PPPM_WMI_IDLE_STATE;

typedef struct {
    DWORD Type;
    DWORD Count;
    DWORD TargetState;          // current idle state
    DWORD OldState;             // previous idle state
    DWORD64 TargetProcessors;
    PPM_WMI_IDLE_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_IDLE_STATES, * PPPM_WMI_IDLE_STATES;

typedef struct {
    DWORD Type;
    DWORD Count;
    DWORD TargetState;          // current idle state
    DWORD OldState;             // previous idle state
    PVOID TargetProcessors;
    PPM_WMI_IDLE_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_IDLE_STATES_EX, * PPPM_WMI_IDLE_STATES_EX;

typedef struct {
    DWORD Frequency;            // in Mhz
    DWORD Power;                // in milliwatts
    BYTE  PercentFrequency;
    BYTE  IncreaseLevel;        // goto higher state
    BYTE  DecreaseLevel;        // goto lower state
    BYTE  Type;                 // performance or throttle
    DWORD IncreaseTime;         // in tick counts
    DWORD DecreaseTime;         // in tick counts
    DWORD64 Control;            // control value
    DWORD64 Status;             // control value
    DWORD HitCount;
    DWORD Reserved1;            // reserved for future use
    DWORD64 Reserved2;
    DWORD64 Reserved3;
} PPM_WMI_PERF_STATE, * PPPM_WMI_PERF_STATE;

typedef struct {
    DWORD Count;
    DWORD MaxFrequency;
    DWORD CurrentState;         // current state
    DWORD MaxPerfState;         // fastest state considering policy restrictions
    DWORD MinPerfState;         // slowest state considering policy restrictions
    DWORD LowestPerfState;      // slowest perf state, fixed, aka the "knee"
    DWORD ThermalConstraint;
    BYTE  BusyAdjThreshold;
    BYTE  PolicyType;           // domain coordination
    BYTE  Type;
    BYTE  Reserved;
    DWORD TimerInterval;
    DWORD64 TargetProcessors;   // domain affinity
    DWORD PStateHandler;
    DWORD PStateContext;
    DWORD TStateHandler;
    DWORD TStateContext;
    DWORD FeedbackHandler;
    DWORD Reserved1;
    DWORD64 Reserved2;
    PPM_WMI_PERF_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_PERF_STATES, * PPPM_WMI_PERF_STATES;

typedef struct {
    DWORD Count;
    DWORD MaxFrequency;
    DWORD CurrentState;         // current state
    DWORD MaxPerfState;         // fastest state considering policy restrictions
    DWORD MinPerfState;         // slowest state considering policy restrictions
    DWORD LowestPerfState;      // slowest perf state, fixed, aka the "knee"
    DWORD ThermalConstraint;
    BYTE  BusyAdjThreshold;
    BYTE  PolicyType;           // domain coordination
    BYTE  Type;
    BYTE  Reserved;
    DWORD TimerInterval;
    PVOID TargetProcessors;     // domain affinity
    DWORD PStateHandler;
    DWORD PStateContext;
    DWORD TStateHandler;
    DWORD TStateContext;
    DWORD FeedbackHandler;
    DWORD Reserved1;
    DWORD64 Reserved2;
    PPM_WMI_PERF_STATE State[ANYSIZE_ARRAY];
} PPM_WMI_PERF_STATES_EX, * PPPM_WMI_PERF_STATES_EX;

//
// Legacy processor idle accounting.
//

#define PROC_IDLE_BUCKET_COUNT 6

typedef struct {
    DWORD IdleTransitions;
    DWORD FailedTransitions;
    DWORD InvalidBucketIndex;
    DWORD64 TotalTime;
    DWORD IdleTimeBuckets[PROC_IDLE_BUCKET_COUNT];
} PPM_IDLE_STATE_ACCOUNTING, * PPPM_IDLE_STATE_ACCOUNTING;

typedef struct {
    DWORD StateCount;
    DWORD TotalTransitions;
    DWORD ResetCount;
    DWORD64 StartTime;
    PPM_IDLE_STATE_ACCOUNTING State[ANYSIZE_ARRAY];
} PPM_IDLE_ACCOUNTING, * PPPM_IDLE_ACCOUNTING;

//
// Processor idle accounting.
//

#define PROC_IDLE_BUCKET_COUNT_EX 16

typedef struct {
    DWORD64 TotalTimeUs;
    DWORD MinTimeUs;
    DWORD MaxTimeUs;
    DWORD Count;
} PPM_IDLE_STATE_BUCKET_EX, * PPPM_IDLE_STATE_BUCKET_EX;

typedef struct {
    DWORD64 TotalTime;
    DWORD IdleTransitions;
    DWORD FailedTransitions;
    DWORD InvalidBucketIndex;
    DWORD MinTimeUs;
    DWORD MaxTimeUs;
    DWORD CancelledTransitions;
    PPM_IDLE_STATE_BUCKET_EX IdleTimeBuckets[PROC_IDLE_BUCKET_COUNT_EX];
} PPM_IDLE_STATE_ACCOUNTING_EX, * PPPM_IDLE_STATE_ACCOUNTING_EX;

typedef struct {
    DWORD StateCount;
    DWORD TotalTransitions;
    DWORD ResetCount;
    DWORD AbortCount;
    DWORD64 StartTime;
    _Field_size_(StateCount) PPM_IDLE_STATE_ACCOUNTING_EX State[ANYSIZE_ARRAY];
} PPM_IDLE_ACCOUNTING_EX, * PPPM_IDLE_ACCOUNTING_EX;

//
// Definitions of coordination types for _PSD, _TSD, and _CSD BIOS objects from
// the Acpi 3.0 specification
//

#define ACPI_PPM_SOFTWARE_ALL     0xFC
#define ACPI_PPM_SOFTWARE_ANY     0xFD
#define ACPI_PPM_HARDWARE_ALL     0xFE

//
// Definition of Microsoft PPM coordination types.
//

#define MS_PPM_SOFTWARE_ALL       0x1

//
// Processor firmware rundown feature bit definitions.
//

#define PPM_FIRMWARE_ACPI1C2      0x00000001
#define PPM_FIRMWARE_ACPI1C3      0x00000002
#define PPM_FIRMWARE_ACPI1TSTATES 0x00000004
#define PPM_FIRMWARE_CST          0x00000008
#define PPM_FIRMWARE_CSD          0x00000010
#define PPM_FIRMWARE_PCT          0x00000020
#define PPM_FIRMWARE_PSS          0x00000040
#define PPM_FIRMWARE_XPSS         0x00000080
#define PPM_FIRMWARE_PPC          0x00000100
#define PPM_FIRMWARE_PSD          0x00000200
#define PPM_FIRMWARE_PTC          0x00000400
#define PPM_FIRMWARE_TSS          0x00000800
#define PPM_FIRMWARE_TPC          0x00001000
#define PPM_FIRMWARE_TSD          0x00002000
#define PPM_FIRMWARE_PCCH         0x00004000
#define PPM_FIRMWARE_PCCP         0x00008000
#define PPM_FIRMWARE_OSC          0x00010000
#define PPM_FIRMWARE_PDC          0x00020000
#define PPM_FIRMWARE_CPC          0x00040000
#define PPM_FIRMWARE_LPI          0x00080000

//
// Processor performance and idle controls implementations.
//

#define PPM_PERFORMANCE_IMPLEMENTATION_NONE      0x00000000
#define PPM_PERFORMANCE_IMPLEMENTATION_PSTATES   0x00000001
#define PPM_PERFORMANCE_IMPLEMENTATION_PCCV1     0x00000002
#define PPM_PERFORMANCE_IMPLEMENTATION_CPPC      0x00000003
#define PPM_PERFORMANCE_IMPLEMENTATION_PEP       0x00000004

#define PPM_IDLE_IMPLEMENTATION_NONE             0x00000000
#define PPM_IDLE_IMPLEMENTATION_CSTATES          0x00000001
#define PPM_IDLE_IMPLEMENTATION_PEP              0x00000002
#define PPM_IDLE_IMPLEMENTATION_MICROPEP         0x00000003
#define PPM_IDLE_IMPLEMENTATION_LPISTATES        0x00000004

//
// Processor Power Management WMI interface.
//

// {A5B32DDD-7F39-4abc-B892-900E43B59EBB}
DEFINE_GUID(PPM_PERFSTATE_CHANGE_GUID,
    0xa5b32ddd, 0x7f39, 0x4abc, 0xb8, 0x92, 0x90, 0xe, 0x43, 0xb5, 0x9e, 0xbb);

// {995e6b7f-d653-497a-b978-36a30c29bf01}
DEFINE_GUID(PPM_PERFSTATE_DOMAIN_CHANGE_GUID,
    0x995e6b7f, 0xd653, 0x497a, 0xb9, 0x78, 0x36, 0xa3, 0xc, 0x29, 0xbf, 0x1);

// {4838fe4f-f71c-4e51-9ecc-8430a7ac4c6c}
DEFINE_GUID(PPM_IDLESTATE_CHANGE_GUID,
    0x4838fe4f, 0xf71c, 0x4e51, 0x9e, 0xcc, 0x84, 0x30, 0xa7, 0xac, 0x4c, 0x6c);

// {5708cc20-7d40-4bf4-b4aa-2b01338d0126}
DEFINE_GUID(PPM_PERFSTATES_DATA_GUID,
    0x5708cc20, 0x7d40, 0x4bf4, 0xb4, 0xaa, 0x2b, 0x01, 0x33, 0x8d, 0x01, 0x26);

// {ba138e10-e250-4ad7-8616-cf1a7ad410e7}
DEFINE_GUID(PPM_IDLESTATES_DATA_GUID,
    0xba138e10, 0xe250, 0x4ad7, 0x86, 0x16, 0xcf, 0x1a, 0x7a, 0xd4, 0x10, 0xe7);

// {e2a26f78-ae07-4ee0-a30f-ce354f5a94cd}
DEFINE_GUID(PPM_IDLE_ACCOUNTING_GUID,
    0xe2a26f78, 0xae07, 0x4ee0, 0xa3, 0x0f, 0xce, 0x54, 0xf5, 0x5a, 0x94, 0xcd);

// {d67abd39-81f8-4a5e-8152-72e31ec912ee}
DEFINE_GUID(PPM_IDLE_ACCOUNTING_EX_GUID,
    0xd67abd39, 0x81f8, 0x4a5e, 0x81, 0x52, 0x72, 0xe3, 0x1e, 0xc9, 0x12, 0xee);

// {a852c2c8-1a4c-423b-8c2c-f30d82931a88}
DEFINE_GUID(PPM_THERMALCONSTRAINT_GUID,
    0xa852c2c8, 0x1a4c, 0x423b, 0x8c, 0x2c, 0xf3, 0x0d, 0x82, 0x93, 0x1a, 0x88);

// {7fd18652-0cfe-40d2-b0a1-0b066a87759e}
DEFINE_GUID(PPM_PERFMON_PERFSTATE_GUID,
    0x7fd18652, 0xcfe, 0x40d2, 0xb0, 0xa1, 0xb, 0x6, 0x6a, 0x87, 0x75, 0x9e);

// {48f377b8-6880-4c7b-8bdc-380176c6654d}
DEFINE_GUID(PPM_THERMAL_POLICY_CHANGE_GUID,
    0x48f377b8, 0x6880, 0x4c7b, 0x8b, 0xdc, 0x38, 0x1, 0x76, 0xc6, 0x65, 0x4d);


typedef struct {
    DWORD State;
    DWORD Status;
    DWORD Latency;
    DWORD Speed;
    DWORD Processor;
} PPM_PERFSTATE_EVENT, * PPPM_PERFSTATE_EVENT;

typedef struct {
    DWORD State;
    DWORD Latency;
    DWORD Speed;
    DWORD64 Processors;
} PPM_PERFSTATE_DOMAIN_EVENT, * PPPM_PERFSTATE_DOMAIN_EVENT;

typedef struct {
    DWORD NewState;
    DWORD OldState;
    DWORD64 Processors;
} PPM_IDLESTATE_EVENT, * PPPM_IDLESTATE_EVENT;

typedef struct {
    DWORD ThermalConstraint;
    DWORD64 Processors;
} PPM_THERMALCHANGE_EVENT, * PPPM_THERMALCHANGE_EVENT;

#pragma warning(push)
#pragma warning(disable:4121)

typedef struct {
    BYTE  Mode;
    DWORD64 Processors;
} PPM_THERMAL_POLICY_EVENT, * PPPM_THERMAL_POLICY_EVENT;

#pragma warning(pop)

// Power Policy Management interfaces
//

typedef struct {
    POWER_ACTION    Action;
    DWORD           Flags;
    DWORD           EventCode;
} POWER_ACTION_POLICY, * PPOWER_ACTION_POLICY;

// POWER_ACTION_POLICY->Flags:
#define POWER_ACTION_QUERY_ALLOWED      0x00000001
#define POWER_ACTION_UI_ALLOWED         0x00000002
#define POWER_ACTION_OVERRIDE_APPS      0x00000004
#define POWER_ACTION_HIBERBOOT          0x00000008
#define POWER_ACTION_USER_NOTIFY        0x00000010  // Indicate User-mode of an impending action.
#define POWER_ACTION_DOZE_TO_HIBERNATE  0x00000020
#define POWER_ACTION_ACPI_CRITICAL      0x01000000
#define POWER_ACTION_ACPI_USER_NOTIFY   0x02000000
#define POWER_ACTION_DIRECTED_DRIPS     0x04000000
#define POWER_ACTION_PSEUDO_TRANSITION  0x08000000
#define POWER_ACTION_LIGHTEST_FIRST     0x10000000
#define POWER_ACTION_LOCK_CONSOLE       0x20000000
#define POWER_ACTION_DISABLE_WAKES      0x40000000
#define POWER_ACTION_CRITICAL           0x80000000

// POWER_ACTION_POLICY->EventCode flags
#define POWER_LEVEL_USER_NOTIFY_TEXT      0x00000001
#define POWER_LEVEL_USER_NOTIFY_SOUND     0x00000002
#define POWER_LEVEL_USER_NOTIFY_EXEC      0x00000004
#define POWER_USER_NOTIFY_BUTTON          0x00000008
#define POWER_USER_NOTIFY_SHUTDOWN        0x00000010 // Application and Services are intimated of shutdown.
#define POWER_USER_NOTIFY_FORCED_SHUTDOWN 0x00000020 // Immediate shutdown - Application and Services are not intimated.
#define POWER_FORCE_TRIGGER_RESET         0x80000000

// Note: for battery alarm EventCodes, the ID of the battery alarm << 16 is ORed
// into the flags.  For example: DISCHARGE_POLICY_LOW << 16

//
// The GUID_BATTERY_DISCHARGE_FLAGS_x power settings use a subset of EventCode
// flags.  The POWER_FORCE_TRIGGER_RESET flag doesn't make sense for a battery
// alarm so it is overloaded for other purposes (gerneral enable/disable).
#define BATTERY_DISCHARGE_FLAGS_EVENTCODE_MASK  0x00000007
#define BATTERY_DISCHARGE_FLAGS_ENABLE  0x80000000

// system battery drain policies
typedef struct {
    BOOLEAN                 Enable;
    BYTE                    Spare[3];
    DWORD                   BatteryLevel;
    POWER_ACTION_POLICY     PowerPolicy;
    SYSTEM_POWER_STATE      MinSystemState;
} SYSTEM_POWER_LEVEL, * PSYSTEM_POWER_LEVEL;

// Discharge policy constants
#define NUM_DISCHARGE_POLICIES      4
#define DISCHARGE_POLICY_CRITICAL   0
#define DISCHARGE_POLICY_LOW        1


// system power policies
typedef struct _SYSTEM_POWER_POLICY {
    DWORD                   Revision;       // 1

    // events
    POWER_ACTION_POLICY     PowerButton;
    POWER_ACTION_POLICY     SleepButton;
    POWER_ACTION_POLICY     LidClose;
    SYSTEM_POWER_STATE      LidOpenWake;
    DWORD                   Reserved;

    // "system idle" detection
    POWER_ACTION_POLICY     Idle;
    DWORD                   IdleTimeout;
    BYTE                    IdleSensitivity;

    BYTE                    DynamicThrottle;
    BYTE                    Spare2[2];

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MinSleep;
    SYSTEM_POWER_STATE      MaxSleep;
    SYSTEM_POWER_STATE      ReducedLatencySleep;
    DWORD                   WinLogonFlags;

    DWORD                   Spare3;

    // parameters for dozing
    //
    DWORD                   DozeS4Timeout;

    // battery policies
    DWORD                   BroadcastCapacityResolution;
    SYSTEM_POWER_LEVEL      DischargePolicy[NUM_DISCHARGE_POLICIES];

    // video policies
    DWORD                   VideoTimeout;
    BOOLEAN                 VideoDimDisplay;
    DWORD                   VideoReserved[3];

    // hard disk policies
    DWORD                   SpindownTimeout;

    // processor policies
    BOOLEAN                 OptimizeForPower;
    BYTE                    FanThrottleTolerance;
    BYTE                    ForcedThrottle;
    BYTE                    MinThrottle;
    POWER_ACTION_POLICY     OverThrottled;

} SYSTEM_POWER_POLICY, * PSYSTEM_POWER_POLICY;


// processor power policy state

//
// Processor Idle State Policy.
//

#define PROCESSOR_IDLESTATE_POLICY_COUNT 0x3

typedef struct {
    DWORD TimeCheck;
    BYTE  DemotePercent;
    BYTE  PromotePercent;
    BYTE  Spare[2];
} PROCESSOR_IDLESTATE_INFO, * PPROCESSOR_IDLESTATE_INFO;

typedef struct {
    WORD   Revision;
    union {
        WORD   AsWORD;
        struct {
            WORD   AllowScaling : 1;
            WORD   Disabled : 1;
            WORD   Reserved : 14;
        } DUMMYSTRUCTNAME;
    } Flags;

    DWORD PolicyCount;
    PROCESSOR_IDLESTATE_INFO Policy[PROCESSOR_IDLESTATE_POLICY_COUNT];
} PROCESSOR_IDLESTATE_POLICY, * PPROCESSOR_IDLESTATE_POLICY;

//
// Legacy Processor Policy.  This is only provided to allow legacy
// applications to compile.  New applications must use
// PROCESSOR_IDLESTATE_POLICY.
//

#define PO_THROTTLE_NONE            0
#define PO_THROTTLE_CONSTANT        1
#define PO_THROTTLE_DEGRADE         2
#define PO_THROTTLE_ADAPTIVE        3
#define PO_THROTTLE_MAXIMUM         4   // not a policy, just a limit


typedef struct _PROCESSOR_POWER_POLICY_INFO {

    // Time based information (will be converted to kernel units)
    DWORD                   TimeCheck;                      // in US
    DWORD                   DemoteLimit;                    // in US
    DWORD                   PromoteLimit;                   // in US

    // Percentage based information
    BYTE                    DemotePercent;
    BYTE                    PromotePercent;
    BYTE                    Spare[2];

    // Flags
    DWORD                   AllowDemotion : 1;
    DWORD                   AllowPromotion : 1;
    DWORD                   Reserved : 30;

} PROCESSOR_POWER_POLICY_INFO, * PPROCESSOR_POWER_POLICY_INFO;

// processor power policy
typedef struct _PROCESSOR_POWER_POLICY {
    DWORD                       Revision;       // 1

    // Dynamic Throttling Policy
    BYTE                        DynamicThrottle;
    BYTE                        Spare[3];

    // Flags
    DWORD                       DisableCStates : 1;
    DWORD                       Reserved : 31;

    // System policy information
    // The Array is last, in case it needs to be grown and the structure
    // revision incremented.
    DWORD                       PolicyCount;
    PROCESSOR_POWER_POLICY_INFO Policy[3];

} PROCESSOR_POWER_POLICY, * PPROCESSOR_POWER_POLICY;

//
// Processor Perf State Policy.
//

typedef struct {
    DWORD Revision;
    BYTE  MaxThrottle;
    BYTE  MinThrottle;
    BYTE  BusyAdjThreshold;
    union {
        BYTE  Spare;
        union {
            BYTE  AsBYTE;
            struct {
                BYTE  NoDomainAccounting : 1;
                BYTE  IncreasePolicy : 2;
                BYTE  DecreasePolicy : 2;
                BYTE  Reserved : 3;
            } DUMMYSTRUCTNAME;
        } Flags;
    } DUMMYUNIONNAME;

    DWORD TimeCheck;
    DWORD IncreaseTime;
    DWORD DecreaseTime;
    DWORD IncreasePercent;
    DWORD DecreasePercent;
} PROCESSOR_PERFSTATE_POLICY, * PPROCESSOR_PERFSTATE_POLICY;

// administrator power policy overrides
typedef struct _ADMINISTRATOR_POWER_POLICY {

    // meaning of power action "sleep"
    SYSTEM_POWER_STATE      MinSleep;
    SYSTEM_POWER_STATE      MaxSleep;

    // video policies
    DWORD                   MinVideoTimeout;
    DWORD                   MaxVideoTimeout;

    // disk policies
    DWORD                   MinSpindownTimeout;
    DWORD                   MaxSpindownTimeout;
} ADMINISTRATOR_POWER_POLICY, * PADMINISTRATOR_POWER_POLICY;


typedef enum _HIBERFILE_BUCKET_SIZE {
    HiberFileBucket1GB = 0,
    HiberFileBucket2GB,
    HiberFileBucket4GB,
    HiberFileBucket8GB,
    HiberFileBucket16GB,
    HiberFileBucket32GB,
    HiberFileBucketUnlimited,
    HiberFileBucketMax
} HIBERFILE_BUCKET_SIZE, * PHIBERFILE_BUCKET_SIZE;

#define HIBERFILE_TYPE_NONE       0x00
#define HIBERFILE_TYPE_REDUCED    0x01
#define HIBERFILE_TYPE_FULL       0x02
#define HIBERFILE_TYPE_MAX        0x03

typedef struct _HIBERFILE_BUCKET {
    DWORD64 MaxPhysicalMemory;
    DWORD PhysicalMemoryPercent[HIBERFILE_TYPE_MAX];
} HIBERFILE_BUCKET, * PHIBERFILE_BUCKET;

typedef struct {
    // Misc supported system features
    BOOLEAN             PowerButtonPresent;
    BOOLEAN             SleepButtonPresent;
    BOOLEAN             LidPresent;
    BOOLEAN             SystemS1;
    BOOLEAN             SystemS2;
    BOOLEAN             SystemS3;
    BOOLEAN             SystemS4;           // hibernate
    BOOLEAN             SystemS5;           // off
    BOOLEAN             HiberFilePresent;
    BOOLEAN             FullWake;
    BOOLEAN             VideoDimPresent;
    BOOLEAN             ApmPresent;
    BOOLEAN             UpsPresent;

    // Processors
    BOOLEAN             ThermalControl;
    BOOLEAN             ProcessorThrottle;
    BYTE                ProcessorMinThrottle;

#if (NTDDI_VERSION < NTDDI_WINXP)
    BYTE                ProcessorThrottleScale;
    BYTE                spare2[4];
#else
    BYTE                ProcessorMaxThrottle;
    BOOLEAN             FastSystemS4;
    BOOLEAN             Hiberboot;
    BOOLEAN             WakeAlarmPresent;
    BOOLEAN             AoAc;
#endif // (NTDDI_VERSION < NTDDI_WINXP)

    // Disk
    BOOLEAN             DiskSpinDown;

#if (NTDDI_VERSION < NTDDI_WINTHRESHOLD)
    BYTE                spare3[8];
# else
    // HiberFile
    BYTE                HiberFileType;
    BOOLEAN             AoAcConnectivitySupported;
    BYTE                spare3[6];
#endif // (NTDDI_VERSION < NTDDI_WINTHRESHOLD)

    // System Battery
    BOOLEAN             SystemBatteriesPresent;
    BOOLEAN             BatteriesAreShortTerm;
    BATTERY_REPORTING_SCALE BatteryScale[3];

    // Wake
    SYSTEM_POWER_STATE  AcOnLineWake;
    SYSTEM_POWER_STATE  SoftLidWake;
    SYSTEM_POWER_STATE  RtcWake;
    SYSTEM_POWER_STATE  MinDeviceWakeState; // note this may change on driver load
    SYSTEM_POWER_STATE  DefaultLowLatencyWake;
} SYSTEM_POWER_CAPABILITIES, * PSYSTEM_POWER_CAPABILITIES;

typedef struct {
    BOOLEAN             AcOnLine;
    BOOLEAN             BatteryPresent;
    BOOLEAN             Charging;
    BOOLEAN             Discharging;
    BOOLEAN             Spare1[3];

    BYTE                Tag;

    DWORD               MaxCapacity;
    DWORD               RemainingCapacity;
    DWORD               Rate;
    DWORD               EstimatedTime;

    DWORD               DefaultAlert1;
    DWORD               DefaultAlert2;
} SYSTEM_BATTERY_STATE, * PSYSTEM_BATTERY_STATE;

#define CONTAINING_RECORD(address, type, field) \
    ((type *)((PCHAR)(address) - (ULONG_PTR)(&((type *)0)->field)))

#define IMAGE_DOS_SIGNATURE                 0x5A4D      // MZ
#define IMAGE_OS2_SIGNATURE                 0x454E      // NE
#define IMAGE_OS2_SIGNATURE_LE              0x454C      // LE
#define IMAGE_VXD_SIGNATURE                 0x454C      // LE
#define IMAGE_NT_SIGNATURE                  0x00004550  // PE00

#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#ifdef UNICODE
#define MAKEINTRESOURCE  MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE


#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_FONTDIR          MAKEINTRESOURCE(7)
#define RT_FONT             MAKEINTRESOURCE(8)
#define RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define RT_RCDATA           MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)

#define DIFFERENCE     11
#define RT_GROUP_CURSOR MAKEINTRESOURCE((ULONG_PTR)(RT_CURSOR) + DIFFERENCE)
#define RT_GROUP_ICON   MAKEINTRESOURCE((ULONG_PTR)(RT_ICON) + DIFFERENCE)
#define RT_VERSION      MAKEINTRESOURCE(16)
#define RT_DLGINCLUDE   MAKEINTRESOURCE(17)

typedef struct tagVS_FIXEDFILEINFO {
    DWORD dwSignature;
    DWORD dwStrucVersion;
    DWORD dwFileVersionMS;
    DWORD dwFileVersionLS;
    DWORD dwProductVersionMS;
    DWORD dwProductVersionLS;
    DWORD dwFileFlagsMask;
    DWORD dwFileFlags;
    DWORD dwFileOS;
    DWORD dwFileType;
    DWORD dwFileSubtype;
    DWORD dwFileDateMS;
    DWORD dwFileDateLS;
} VS_FIXEDFILEINFO;

typedef struct {
    WORD             wLength;
    WORD             wValueLength;
    WORD             wType;
    WCHAR            szKey;
    WORD             Padding1;
    VS_FIXEDFILEINFO Value;
    WORD             Padding2;
    WORD             Children;
} VS_VERSIONINFO;

// Privilege attributes
//

#define SE_PRIVILEGE_ENABLED_BY_DEFAULT (0x00000001L)
#define SE_PRIVILEGE_ENABLED            (0x00000002L)
#define SE_PRIVILEGE_REMOVED            (0X00000004L)
#define SE_PRIVILEGE_USED_FOR_ACCESS    (0x80000000L)

#define SE_PRIVILEGE_VALID_ATTRIBUTES   (SE_PRIVILEGE_ENABLED_BY_DEFAULT | \
                                         SE_PRIVILEGE_ENABLED            | \
                                         SE_PRIVILEGE_REMOVED            | \
                                         SE_PRIVILEGE_USED_FOR_ACCESS)


//
// Privilege Set Control flags
//

#define PRIVILEGE_SET_ALL_NECESSARY    (1)

EXTERN_C
NTSYSAPI PRUNTIME_FUNCTION NTAPI RtlLookupFunctionEntry(
    DWORD64               ControlPc,
    PDWORD64              ImageBase,
    PUNWIND_HISTORY_TABLE HistoryTable
);

typedef enum _QUEUE_USER_APC_FLAGS {
    QUEUE_USER_APC_FLAGS_NONE = 0x00000000,
    QUEUE_USER_APC_FLAGS_SPECIAL_USER_APC = 0x00000001,

    //
    // Used for requesting additional callback data.
    //

    QUEUE_USER_APC_CALLBACK_DATA_CONTEXT = 0x00010000,
} QUEUE_USER_APC_FLAGS;

// IMAGE_FIRST_SECTION doesn't need 32/64 versions since the file header is the same either way.

#define IMAGE_FIRST_SECTION( ntheader ) ((PIMAGE_SECTION_HEADER)        \
    ((ULONG_PTR)(ntheader) +                                            \
     FIELD_OFFSET( IMAGE_NT_HEADERS, OptionalHeader ) +                 \
     ((ntheader))->FileHeader.SizeOfOptionalHeader   \
    ))

//
// Mandatory Label Authority.
//

#define SECURITY_MANDATORY_LABEL_AUTHORITY          {0,0,0,0,0,16}
#define SECURITY_MANDATORY_UNTRUSTED_RID            (0x00000000L)
#define SECURITY_MANDATORY_LOW_RID                  (0x00001000L)
#define SECURITY_MANDATORY_MEDIUM_RID               (0x00002000L)
#define SECURITY_MANDATORY_MEDIUM_PLUS_RID          (SECURITY_MANDATORY_MEDIUM_RID + 0x100)
#define SECURITY_MANDATORY_HIGH_RID                 (0x00003000L)
#define SECURITY_MANDATORY_SYSTEM_RID               (0x00004000L)
#define SECURITY_MANDATORY_PROTECTED_PROCESS_RID    (0x00005000L)

//
// SECURITY_MANDATORY_MAXIMUM_USER_RID is the highest RID that
// can be set by a usermode caller.
//

#define SECURITY_MANDATORY_MAXIMUM_USER_RID   SECURITY_MANDATORY_SYSTEM_RID

#define MANDATORY_LEVEL_TO_MANDATORY_RID(IL) (IL * 0x1000)

#define SECURITY_SCOPED_POLICY_ID_AUTHORITY             {0,0,0,0,0,17}

//
// Authentication Authority
//

#define SECURITY_AUTHENTICATION_AUTHORITY                        {0,0,0,0,0,18}
#define SECURITY_AUTHENTICATION_AUTHORITY_RID_COUNT              (1L)
#define SECURITY_AUTHENTICATION_AUTHORITY_ASSERTED_RID           (0x00000001L)
#define SECURITY_AUTHENTICATION_SERVICE_ASSERTED_RID             (0x00000002L)
#define SECURITY_AUTHENTICATION_FRESH_KEY_AUTH_RID               (0x00000003L)
#define SECURITY_AUTHENTICATION_KEY_TRUST_RID                    (0x00000004L)
#define SECURITY_AUTHENTICATION_KEY_PROPERTY_MFA_RID             (0x00000005L)
#define SECURITY_AUTHENTICATION_KEY_PROPERTY_ATTESTATION_RID     (0x00000006L)

//
// Process Trust Authority
//

#define SECURITY_PROCESS_TRUST_AUTHORITY    {0,0,0,0,0,19}
#define SECURITY_PROCESS_TRUST_AUTHORITY_RID_COUNT (2L)

#define SECURITY_PROCESS_PROTECTION_TYPE_FULL_RID           (0x00000400L)
#define SECURITY_PROCESS_PROTECTION_TYPE_LITE_RID           (0x00000200L)
#define SECURITY_PROCESS_PROTECTION_TYPE_NONE_RID           (0x00000000L)

#define SECURITY_PROCESS_PROTECTION_LEVEL_WINTCB_RID        (0x00002000L)
#define SECURITY_PROCESS_PROTECTION_LEVEL_WINDOWS_RID       (0x00001000L)
#define SECURITY_PROCESS_PROTECTION_LEVEL_APP_RID           (0x00000800L)
#define SECURITY_PROCESS_PROTECTION_LEVEL_ANTIMALWARE_RID   (0x00000600L)
#define SECURITY_PROCESS_PROTECTION_LEVEL_AUTHENTICODE_RID  (0x00000400L)
#define SECURITY_PROCESS_PROTECTION_LEVEL_NONE_RID          (0x00000000L)

//
// Trusted Installer RIDs
//

#define SECURITY_TRUSTED_INSTALLER_RID1 956008885
#define SECURITY_TRUSTED_INSTALLER_RID2 3418522649
#define SECURITY_TRUSTED_INSTALLER_RID3 1831038044
#define SECURITY_TRUSTED_INSTALLER_RID4 1853292631
#define SECURITY_TRUSTED_INSTALLER_RID5 2271478464

typedef struct _TOKEN_MANDATORY_LABEL {
    SID_AND_ATTRIBUTES Label;
} TOKEN_MANDATORY_LABEL, * PTOKEN_MANDATORY_LABEL;

#ifndef __PCGUID_DEFINED__
#define __PCGUID_DEFINED__
typedef const GUID* PCGUID;
#endif

//
// The reparse GUID structure is used by all 3rd party layered drivers to
// store data in a reparse point. For non-Microsoft tags, The GUID field
// cannot be GUID_NULL.
// The constraints on reparse tags are defined below.
// Microsoft tags can also be used with this format of the reparse point buffer.
//

typedef struct _REPARSE_GUID_DATA_BUFFER {
    DWORD  ReparseTag;
    WORD   ReparseDataLength;
    WORD   Reserved;
    GUID   ReparseGuid;
    struct {
        BYTE   DataBuffer[1];
    } GenericReparseBuffer;
} REPARSE_GUID_DATA_BUFFER, * PREPARSE_GUID_DATA_BUFFER;

#define REPARSE_GUID_DATA_BUFFER_HEADER_SIZE   UFIELD_OFFSET(REPARSE_GUID_DATA_BUFFER, GenericReparseBuffer)

//
// Maximum allowed size of the reparse data.
//

#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE      ( 16 * 1024 )

//
// Predefined reparse tags.
// These tags need to avoid conflicting with IO_REMOUNT defined in ntos\inc\io.h
//

#define IO_REPARSE_TAG_RESERVED_ZERO             (0)
#define IO_REPARSE_TAG_RESERVED_ONE              (1)
#define IO_REPARSE_TAG_RESERVED_TWO              (2)

//
// The value of the following constant needs to satisfy the following conditions:
//  (1) Be at least as large as the largest of the reserved tags.
//  (2) Be strictly smaller than all the tags in use.
//

#define IO_REPARSE_TAG_RESERVED_RANGE            IO_REPARSE_TAG_RESERVED_TWO

//
// The reparse tags are a DWORD. The 32 bits are laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-----------------------+-------------------------------+
//  |M|R|N|D|     Reserved bits     |       Reparse Tag Value       |
//  +-+-+-+-+-----------------------+-------------------------------+
//
// M is the Microsoft bit. When set to 1, it denotes a tag owned by Microsoft.
//   All ISVs must use a tag with a 0 in this position.
//   Note: If a Microsoft tag is used by non-Microsoft software, the
//   behavior is not defined.
//
// R is reserved.  Must be zero for non-Microsoft tags.
//
// N is name surrogate. When set to 1, the file represents another named
//   entity in the system.
//
// D is the directory bit. When set to 1, indicates that any directory
//   with this reparse tag can have children. Has no special meaning when used
//   on a non-directory file. Not compatible with the name surrogate bit.
//
// The M and N bits are OR-able.
// The following macros check for the M and N bit values:
//

//
// Macro to determine whether a reparse point tag corresponds to a tag
// owned by Microsoft.
//

#define IsReparseTagMicrosoft(_tag) (              \
                           ((_tag) & 0x80000000)   \
                           )

//
// Macro to determine whether a reparse point tag corresponds to a reserved
// tag owned by Microsoft.
//

#define IsReparseTagReserved(_tag) (               \
                           ((_tag) & 0x40000000)   \
                           )

//
// Macro to determine whether a reparse point tag is a name surrogate
//

#define IsReparseTagNameSurrogate(_tag) (          \
                           ((_tag) & 0x20000000)   \
                           )

//
// Macro to determine whether a directory with this reparse point can have
// children.
//

#define IsReparseTagDirectory(_tag) (    \
                           ((_tag) & 0x10000000)   \
                           )

#define IO_REPARSE_TAG_RESERVED_INVALID         (0xC0008000L)       
#define IO_REPARSE_TAG_MOUNT_POINT              (0xA0000003L)       
#define IO_REPARSE_TAG_HSM                      (0xC0000004L)       
#define IO_REPARSE_TAG_HSM2                     (0x80000006L)       
#define IO_REPARSE_TAG_SIS                      (0x80000007L)       
#define IO_REPARSE_TAG_WIM                      (0x80000008L)       
#define IO_REPARSE_TAG_CSV                      (0x80000009L)       
#define IO_REPARSE_TAG_DFS                      (0x8000000AL)       
#define IO_REPARSE_TAG_SYMLINK                  (0xA000000CL)       
#define IO_REPARSE_TAG_DFSR                     (0x80000012L)       
#define IO_REPARSE_TAG_DEDUP                    (0x80000013L)       
#define IO_REPARSE_TAG_NFS                      (0x80000014L)       
#define IO_REPARSE_TAG_FILE_PLACEHOLDER         (0x80000015L)       
#define IO_REPARSE_TAG_WOF                      (0x80000017L)       
#define IO_REPARSE_TAG_WCI                      (0x80000018L)       
#define IO_REPARSE_TAG_WCI_1                    (0x90001018L)       
#define IO_REPARSE_TAG_GLOBAL_REPARSE           (0xA0000019L)       
#define IO_REPARSE_TAG_CLOUD                    (0x9000001AL)       
#define IO_REPARSE_TAG_CLOUD_1                  (0x9000101AL)       
#define IO_REPARSE_TAG_CLOUD_2                  (0x9000201AL)       
#define IO_REPARSE_TAG_CLOUD_3                  (0x9000301AL)       
#define IO_REPARSE_TAG_CLOUD_4                  (0x9000401AL)       
#define IO_REPARSE_TAG_CLOUD_5                  (0x9000501AL)       
#define IO_REPARSE_TAG_CLOUD_6                  (0x9000601AL)       
#define IO_REPARSE_TAG_CLOUD_7                  (0x9000701AL)       
#define IO_REPARSE_TAG_CLOUD_8                  (0x9000801AL)       
#define IO_REPARSE_TAG_CLOUD_9                  (0x9000901AL)       
#define IO_REPARSE_TAG_CLOUD_A                  (0x9000A01AL)       
#define IO_REPARSE_TAG_CLOUD_B                  (0x9000B01AL)       
#define IO_REPARSE_TAG_CLOUD_C                  (0x9000C01AL)       
#define IO_REPARSE_TAG_CLOUD_D                  (0x9000D01AL)       
#define IO_REPARSE_TAG_CLOUD_E                  (0x9000E01AL)       
#define IO_REPARSE_TAG_CLOUD_F                  (0x9000F01AL)       
#define IO_REPARSE_TAG_CLOUD_MASK               (0x0000F000L)       
#define IO_REPARSE_TAG_APPEXECLINK              (0x8000001BL)       
#define IO_REPARSE_TAG_PROJFS                   (0x9000001CL)       
#define IO_REPARSE_TAG_STORAGE_SYNC             (0x8000001EL)       
#define IO_REPARSE_TAG_WCI_TOMBSTONE            (0xA000001FL)       
#define IO_REPARSE_TAG_UNHANDLED                (0x80000020L)       
#define IO_REPARSE_TAG_ONEDRIVE                 (0x80000021L)       
#define IO_REPARSE_TAG_PROJFS_TOMBSTONE         (0xA0000022L)       
#define IO_REPARSE_TAG_AF_UNIX                  (0x80000023L)       
#define IO_REPARSE_TAG_WCI_LINK                 (0xA0000027L)       
#define IO_REPARSE_TAG_WCI_LINK_1               (0xA0001027L)       
#define IO_REPARSE_TAG_DATALESS_CIM             (0xA0000028L)       

typedef PVOID DLL_DIRECTORY_COOKIE, * PDLL_DIRECTORY_COOKIE;

//
// Idle resiliency
//
typedef struct _POWER_IDLE_RESILIENCY {
    DWORD CoalescingTimeout;
    DWORD IdleResiliencyPeriod;
} POWER_IDLE_RESILIENCY, * PPOWER_IDLE_RESILIENCY;

//
// Monitor on/off reasons
//
// N.B. Update power-event mapping when adding new events.
//
typedef enum {
    MonitorRequestReasonUnknown,
    MonitorRequestReasonPowerButton,
    MonitorRequestReasonRemoteConnection,
    MonitorRequestReasonScMonitorpower,
    MonitorRequestReasonUserInput,
    MonitorRequestReasonAcDcDisplayBurst,
    MonitorRequestReasonUserDisplayBurst,
    MonitorRequestReasonPoSetSystemState,
    MonitorRequestReasonSetThreadExecutionState,
    MonitorRequestReasonFullWake,
    MonitorRequestReasonSessionUnlock,
    MonitorRequestReasonScreenOffRequest,
    MonitorRequestReasonIdleTimeout,
    MonitorRequestReasonPolicyChange,
    MonitorRequestReasonSleepButton,
    MonitorRequestReasonLid,
    MonitorRequestReasonBatteryCountChange,
    MonitorRequestReasonGracePeriod,
    MonitorRequestReasonPnP,
    MonitorRequestReasonDP,
    MonitorRequestReasonSxTransition,
    MonitorRequestReasonSystemIdle,
    MonitorRequestReasonNearProximity,
    MonitorRequestReasonThermalStandby,
    MonitorRequestReasonResumePdc,
    MonitorRequestReasonResumeS4,
    MonitorRequestReasonTerminal,
    MonitorRequestReasonPdcSignal,
    MonitorRequestReasonAcDcDisplayBurstSuppressed,
    MonitorRequestReasonSystemStateEntered, // When CS exit happens because system
    // transition to S4/S5, please note this
    // reason is different than ReasonSxTransition.
    MonitorRequestReasonWinrt,
    MonitorRequestReasonUserInputKeyboard,
    MonitorRequestReasonUserInputMouse,
    MonitorRequestReasonUserInputTouchpad,
    MonitorRequestReasonUserInputPen,
    MonitorRequestReasonUserInputAccelerometer,
    MonitorRequestReasonUserInputHid,
    MonitorRequestReasonUserInputPoUserPresent,
    MonitorRequestReasonUserInputSessionSwitch,
    MonitorRequestReasonUserInputInitialization,
    MonitorRequestReasonPdcSignalWindowsMobilePwrNotif,         // PDC_SIGNAL_PROVIDER_PWRNOTIF_SVC
    MonitorRequestReasonPdcSignalWindowsMobileShell,            // PDC_SIGNAL_PROVIDER_UM_CS_CONTROL
    MonitorRequestReasonPdcSignalHeyCortana,                    // PDC_SIGNAL_PROVIDER_HEY_CORTANA
    MonitorRequestReasonPdcSignalHolographicShell,              // PDC_SIGNAL_PROVIDER_HOLOSI_CRITICAL_BATTERY_WAKE
    MonitorRequestReasonPdcSignalFingerprint,                   // PDC_SIGNAL_PROVIDER_WINBIO
    MonitorRequestReasonDirectedDrips,
    MonitorRequestReasonDim,
    MonitorRequestReasonBuiltinPanel,
    MonitorRequestReasonDisplayRequiredUnDim,
    MonitorRequestReasonBatteryCountChangeSuppressed,
    MonitorRequestReasonResumeModernStandby,
    MonitorRequestReasonTerminalInit,
    MonitorRequestReasonPdcSignalSensorsHumanPresence,          // PDC_SIGNAL_PROVIDER_SENSORS_HUMAN_PRESENCE_MONITOR
    MonitorRequestReasonBatteryPreCritical,
    MonitorRequestReasonUserInputTouch,
    MonitorRequestReasonMax
} POWER_MONITOR_REQUEST_REASON;

typedef enum _POWER_MONITOR_REQUEST_TYPE {
    MonitorRequestTypeOff,
    MonitorRequestTypeOnAndPresent,
    MonitorRequestTypeToggleOn
} POWER_MONITOR_REQUEST_TYPE;

//
// Monitor invocation
//
typedef struct _POWER_MONITOR_INVOCATION {
    BOOLEAN Console;
    POWER_MONITOR_REQUEST_REASON RequestReason;
} POWER_MONITOR_INVOCATION, * PPOWER_MONITOR_INVOCATION;


typedef
VOID
(NTAPI* PAPCFUNC)(
    _In_ ULONG_PTR Parameter
    );

//
//

typedef struct _JOBOBJECT_BASIC_ACCOUNTING_INFORMATION {
    LARGE_INTEGER TotalUserTime;
    LARGE_INTEGER TotalKernelTime;
    LARGE_INTEGER ThisPeriodTotalUserTime;
    LARGE_INTEGER ThisPeriodTotalKernelTime;
    DWORD TotalPageFaultCount;
    DWORD TotalProcesses;
    DWORD ActiveProcesses;
    DWORD TotalTerminatedProcesses;
} JOBOBJECT_BASIC_ACCOUNTING_INFORMATION, * PJOBOBJECT_BASIC_ACCOUNTING_INFORMATION;

//@[comment("MVI_tracked")]
typedef struct _JOBOBJECT_BASIC_LIMIT_INFORMATION {
    LARGE_INTEGER PerProcessUserTimeLimit;
    LARGE_INTEGER PerJobUserTimeLimit;
    DWORD LimitFlags;
    SIZE_T MinimumWorkingSetSize;
    SIZE_T MaximumWorkingSetSize;
    DWORD ActiveProcessLimit;
    ULONG_PTR Affinity;
    DWORD PriorityClass;
    DWORD SchedulingClass;
} JOBOBJECT_BASIC_LIMIT_INFORMATION, * PJOBOBJECT_BASIC_LIMIT_INFORMATION;

//@[comment("MVI_tracked")]
typedef struct _JOBOBJECT_EXTENDED_LIMIT_INFORMATION {
    JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
    IO_COUNTERS IoInfo;
    SIZE_T ProcessMemoryLimit;
    SIZE_T JobMemoryLimit;
    SIZE_T PeakProcessMemoryUsed;
    SIZE_T PeakJobMemoryUsed;
} JOBOBJECT_EXTENDED_LIMIT_INFORMATION, * PJOBOBJECT_EXTENDED_LIMIT_INFORMATION;

//
//

//@[comment("MVI_tracked")]
typedef struct _JOBOBJECT_BASIC_PROCESS_ID_LIST {
    DWORD NumberOfAssignedProcesses;
    DWORD NumberOfProcessIdsInList;
    ULONG_PTR ProcessIdList[1];
} JOBOBJECT_BASIC_PROCESS_ID_LIST, * PJOBOBJECT_BASIC_PROCESS_ID_LIST;

typedef struct _JOBOBJECT_BASIC_UI_RESTRICTIONS {
    DWORD UIRestrictionsClass;
} JOBOBJECT_BASIC_UI_RESTRICTIONS, * PJOBOBJECT_BASIC_UI_RESTRICTIONS;

//
// N.B. The JOBOBJECT_SECURITY_LIMIT_INFORMATION information class is no longer supported.
//

typedef struct _JOBOBJECT_SECURITY_LIMIT_INFORMATION {
    DWORD SecurityLimitFlags;
    HANDLE JobToken;
    PTOKEN_GROUPS SidsToDisable;
    PTOKEN_PRIVILEGES PrivilegesToDelete;
    PTOKEN_GROUPS RestrictedSids;
} JOBOBJECT_SECURITY_LIMIT_INFORMATION, * PJOBOBJECT_SECURITY_LIMIT_INFORMATION;

typedef struct _JOBOBJECT_END_OF_JOB_TIME_INFORMATION {
    DWORD EndOfJobTimeAction;
} JOBOBJECT_END_OF_JOB_TIME_INFORMATION, * PJOBOBJECT_END_OF_JOB_TIME_INFORMATION;

typedef struct _JOBOBJECT_ASSOCIATE_COMPLETION_PORT {
    PVOID CompletionKey;
    HANDLE CompletionPort;
} JOBOBJECT_ASSOCIATE_COMPLETION_PORT, * PJOBOBJECT_ASSOCIATE_COMPLETION_PORT;

typedef struct _JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION {
    JOBOBJECT_BASIC_ACCOUNTING_INFORMATION BasicInfo;
    IO_COUNTERS IoInfo;
} JOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION, * PJOBOBJECT_BASIC_AND_IO_ACCOUNTING_INFORMATION;

typedef struct _JOBOBJECT_JOBSET_INFORMATION {
    DWORD MemberLevel;
} JOBOBJECT_JOBSET_INFORMATION, * PJOBOBJECT_JOBSET_INFORMATION;

typedef enum _JOBOBJECT_RATE_CONTROL_TOLERANCE {
    ToleranceLow = 1,
    ToleranceMedium,
    ToleranceHigh
} JOBOBJECT_RATE_CONTROL_TOLERANCE, * PJOBOBJECT_RATE_CONTROL_TOLERANCE;

typedef enum _JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL {
    ToleranceIntervalShort = 1,
    ToleranceIntervalMedium,
    ToleranceIntervalLong
} JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL,
* PJOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL;

typedef struct _JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION {
    DWORD64 IoReadBytesLimit;
    DWORD64 IoWriteBytesLimit;
    LARGE_INTEGER PerJobUserTimeLimit;
    DWORD64 JobMemoryLimit;
    JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlTolerance;
    JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL RateControlToleranceInterval;
    DWORD LimitFlags;
} JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION, * PJOBOBJECT_NOTIFICATION_LIMIT_INFORMATION;

typedef struct JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION_2 {
    DWORD64 IoReadBytesLimit;
    DWORD64 IoWriteBytesLimit;
    LARGE_INTEGER PerJobUserTimeLimit;
    union {
        DWORD64 JobHighMemoryLimit;
        DWORD64 JobMemoryLimit;
    } DUMMYUNIONNAME;

    union {
        JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlTolerance;
        JOBOBJECT_RATE_CONTROL_TOLERANCE CpuRateControlTolerance;
    } DUMMYUNIONNAME2;

    union {
        JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL RateControlToleranceInterval;
        JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL
            CpuRateControlToleranceInterval;
    } DUMMYUNIONNAME3;

    DWORD LimitFlags;
    JOBOBJECT_RATE_CONTROL_TOLERANCE IoRateControlTolerance;
    DWORD64 JobLowMemoryLimit;
    JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL IoRateControlToleranceInterval;
    JOBOBJECT_RATE_CONTROL_TOLERANCE NetRateControlTolerance;
    JOBOBJECT_RATE_CONTROL_TOLERANCE_INTERVAL NetRateControlToleranceInterval;
} JOBOBJECT_NOTIFICATION_LIMIT_INFORMATION_2;

//
//

typedef struct _JOBOBJECT_LIMIT_VIOLATION_INFORMATION {
    DWORD LimitFlags;
    DWORD ViolationLimitFlags;
    DWORD64 IoReadBytes;
    DWORD64 IoReadBytesLimit;
    DWORD64 IoWriteBytes;
    DWORD64 IoWriteBytesLimit;
    LARGE_INTEGER PerJobUserTime;
    LARGE_INTEGER PerJobUserTimeLimit;
    DWORD64 JobMemory;
    DWORD64 JobMemoryLimit;
    JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlTolerance;
    JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlToleranceLimit;
} JOBOBJECT_LIMIT_VIOLATION_INFORMATION, * PJOBOBJECT_LIMIT_VIOLATION_INFORMATION;

typedef struct JOBOBJECT_LIMIT_VIOLATION_INFORMATION_2 {
    DWORD LimitFlags;
    DWORD ViolationLimitFlags;
    DWORD64 IoReadBytes;
    DWORD64 IoReadBytesLimit;
    DWORD64 IoWriteBytes;
    DWORD64 IoWriteBytesLimit;
    LARGE_INTEGER PerJobUserTime;
    LARGE_INTEGER PerJobUserTimeLimit;
    DWORD64 JobMemory;
    union {
        DWORD64 JobHighMemoryLimit;
        DWORD64 JobMemoryLimit;
    } DUMMYUNIONNAME;

    union {
        JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlTolerance;
        JOBOBJECT_RATE_CONTROL_TOLERANCE CpuRateControlTolerance;
    } DUMMYUNIONNAME2;

    union {
        JOBOBJECT_RATE_CONTROL_TOLERANCE RateControlToleranceLimit;
        JOBOBJECT_RATE_CONTROL_TOLERANCE CpuRateControlToleranceLimit;
    } DUMMYUNIONNAME3;

    DWORD64 JobLowMemoryLimit;
    JOBOBJECT_RATE_CONTROL_TOLERANCE IoRateControlTolerance;
    JOBOBJECT_RATE_CONTROL_TOLERANCE IoRateControlToleranceLimit;
    JOBOBJECT_RATE_CONTROL_TOLERANCE NetRateControlTolerance;
    JOBOBJECT_RATE_CONTROL_TOLERANCE NetRateControlToleranceLimit;
} JOBOBJECT_LIMIT_VIOLATION_INFORMATION_2;

//
//

typedef struct _JOBOBJECT_CPU_RATE_CONTROL_INFORMATION {
    DWORD ControlFlags;
    union {
        DWORD CpuRate;
        DWORD Weight;
        struct {
            WORD   MinRate;
            WORD   MaxRate;
        } DUMMYSTRUCTNAME;
    } DUMMYUNIONNAME;
} JOBOBJECT_CPU_RATE_CONTROL_INFORMATION, * PJOBOBJECT_CPU_RATE_CONTROL_INFORMATION;

//
// Control flags for network rate control.
//

typedef enum JOB_OBJECT_NET_RATE_CONTROL_FLAGS {
    JOB_OBJECT_NET_RATE_CONTROL_ENABLE = 0x1,
    JOB_OBJECT_NET_RATE_CONTROL_MAX_BANDWIDTH = 0x2,
    JOB_OBJECT_NET_RATE_CONTROL_DSCP_TAG = 0x4,
    JOB_OBJECT_NET_RATE_CONTROL_VALID_FLAGS = 0x7
} JOB_OBJECT_NET_RATE_CONTROL_FLAGS;

#if !defined(SORTPP_PASS) && !defined(MIDL_PASS) && !defined(RC_INVOKED)

DEFINE_ENUM_FLAG_OPERATORS(JOB_OBJECT_NET_RATE_CONTROL_FLAGS)
C_ASSERT(JOB_OBJECT_NET_RATE_CONTROL_VALID_FLAGS ==
         (JOB_OBJECT_NET_RATE_CONTROL_ENABLE +
          JOB_OBJECT_NET_RATE_CONTROL_MAX_BANDWIDTH +
          JOB_OBJECT_NET_RATE_CONTROL_DSCP_TAG));

#endif

#define JOB_OBJECT_NET_RATE_CONTROL_MAX_DSCP_TAG 64

typedef struct JOBOBJECT_NET_RATE_CONTROL_INFORMATION {
    DWORD64 MaxBandwidth;
    JOB_OBJECT_NET_RATE_CONTROL_FLAGS ControlFlags;
    BYTE  DscpTag;
} JOBOBJECT_NET_RATE_CONTROL_INFORMATION;

//
//

//
// Control flags for IO rate control.
//

typedef enum JOB_OBJECT_IO_RATE_CONTROL_FLAGS {
    JOB_OBJECT_IO_RATE_CONTROL_ENABLE = 0x1,
    JOB_OBJECT_IO_RATE_CONTROL_STANDALONE_VOLUME = 0x2,
    JOB_OBJECT_IO_RATE_CONTROL_FORCE_UNIT_ACCESS_ALL = 0x4,
    JOB_OBJECT_IO_RATE_CONTROL_FORCE_UNIT_ACCESS_ON_SOFT_CAP = 0x8,
    JOB_OBJECT_IO_RATE_CONTROL_VALID_FLAGS = (int) JOB_OBJECT_IO_RATE_CONTROL_ENABLE |
    (int) JOB_OBJECT_IO_RATE_CONTROL_STANDALONE_VOLUME |
    (int) JOB_OBJECT_IO_RATE_CONTROL_FORCE_UNIT_ACCESS_ALL |
    (int) JOB_OBJECT_IO_RATE_CONTROL_FORCE_UNIT_ACCESS_ON_SOFT_CAP
} JOB_OBJECT_IO_RATE_CONTROL_FLAGS;

typedef struct JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE {
    LONG64 MaxIops;
    LONG64 MaxBandwidth;
    LONG64 ReservationIops;
    PWSTR VolumeName;
    DWORD BaseIoSize;
    JOB_OBJECT_IO_RATE_CONTROL_FLAGS ControlFlags;
    WORD   VolumeNameLength;
} JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE;

typedef JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE
JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V1;

typedef struct JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V2 {
    LONG64 MaxIops;
    LONG64 MaxBandwidth;
    LONG64 ReservationIops;
    PWSTR VolumeName;
    DWORD BaseIoSize;
    JOB_OBJECT_IO_RATE_CONTROL_FLAGS ControlFlags;
    WORD   VolumeNameLength;
    LONG64 CriticalReservationIops;
    LONG64 ReservationBandwidth;
    LONG64 CriticalReservationBandwidth;
    LONG64 MaxTimePercent;
    LONG64 ReservationTimePercent;
    LONG64 CriticalReservationTimePercent;
} JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V2;

typedef struct JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V3 {
    LONG64 MaxIops;
    LONG64 MaxBandwidth;
    LONG64 ReservationIops;
    PWSTR VolumeName;
    DWORD BaseIoSize;
    JOB_OBJECT_IO_RATE_CONTROL_FLAGS ControlFlags;
    WORD   VolumeNameLength;
    LONG64 CriticalReservationIops;
    LONG64 ReservationBandwidth;
    LONG64 CriticalReservationBandwidth;
    LONG64 MaxTimePercent;
    LONG64 ReservationTimePercent;
    LONG64 CriticalReservationTimePercent;
    LONG64 SoftMaxIops;
    LONG64 SoftMaxBandwidth;
    LONG64 SoftMaxTimePercent;
    LONG64 LimitExcessNotifyIops;
    LONG64 LimitExcessNotifyBandwidth;
    LONG64 LimitExcessNotifyTimePercent;
} JOBOBJECT_IO_RATE_CONTROL_INFORMATION_NATIVE_V3;

//
//

typedef enum JOBOBJECT_IO_ATTRIBUTION_CONTROL_FLAGS {
    JOBOBJECT_IO_ATTRIBUTION_CONTROL_ENABLE = 0x1,
    JOBOBJECT_IO_ATTRIBUTION_CONTROL_DISABLE = 0x2,
    JOBOBJECT_IO_ATTRIBUTION_CONTROL_VALID_FLAGS = 0x3
} JOBOBJECT_IO_ATTRIBUTION_CONTROL_FLAGS;

typedef struct _JOBOBJECT_IO_ATTRIBUTION_STATS {

    ULONG_PTR IoCount;
    ULONGLONG TotalNonOverlappedQueueTime;
    ULONGLONG TotalNonOverlappedServiceTime;
    ULONGLONG TotalSize;

} JOBOBJECT_IO_ATTRIBUTION_STATS, * PJOBOBJECT_IO_ATTRIBUTION_STATS;

typedef struct _JOBOBJECT_IO_ATTRIBUTION_INFORMATION {
    DWORD ControlFlags;

    JOBOBJECT_IO_ATTRIBUTION_STATS ReadStats;
    JOBOBJECT_IO_ATTRIBUTION_STATS WriteStats;

} JOBOBJECT_IO_ATTRIBUTION_INFORMATION, * PJOBOBJECT_IO_ATTRIBUTION_INFORMATION;

//
//

#define JOB_OBJECT_TERMINATE_AT_END_OF_JOB  0
#define JOB_OBJECT_POST_AT_END_OF_JOB       1

//
// Completion Port Messages for job objects
//
// These values are returned via the lpNumberOfBytesTransferred parameter
//

#define JOB_OBJECT_MSG_END_OF_JOB_TIME          1
#define JOB_OBJECT_MSG_END_OF_PROCESS_TIME      2
#define JOB_OBJECT_MSG_ACTIVE_PROCESS_LIMIT     3
#define JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO      4
#define JOB_OBJECT_MSG_NEW_PROCESS              6
#define JOB_OBJECT_MSG_EXIT_PROCESS             7
#define JOB_OBJECT_MSG_ABNORMAL_EXIT_PROCESS    8
#define JOB_OBJECT_MSG_PROCESS_MEMORY_LIMIT     9
#define JOB_OBJECT_MSG_JOB_MEMORY_LIMIT         10
#define JOB_OBJECT_MSG_NOTIFICATION_LIMIT       11
#define JOB_OBJECT_MSG_JOB_CYCLE_TIME_LIMIT     12
#define JOB_OBJECT_MSG_SILO_TERMINATED          13

//
// Define the valid notification filter values.
//

#define JOB_OBJECT_MSG_MINIMUM 1
#define JOB_OBJECT_MSG_MAXIMUM 13

#define JOB_OBJECT_VALID_COMPLETION_FILTER \
    (((1UL << (JOB_OBJECT_MSG_MAXIMUM + 1)) - 1) - \
     ((1UL << JOB_OBJECT_MSG_MINIMUM) - 1))

//
// Basic Limits
//
#define JOB_OBJECT_LIMIT_WORKINGSET                 0x00000001
#define JOB_OBJECT_LIMIT_PROCESS_TIME               0x00000002
#define JOB_OBJECT_LIMIT_JOB_TIME                   0x00000004
#define JOB_OBJECT_LIMIT_ACTIVE_PROCESS             0x00000008
#define JOB_OBJECT_LIMIT_AFFINITY                   0x00000010
#define JOB_OBJECT_LIMIT_PRIORITY_CLASS             0x00000020
#define JOB_OBJECT_LIMIT_PRESERVE_JOB_TIME          0x00000040
#define JOB_OBJECT_LIMIT_SCHEDULING_CLASS           0x00000080

//
// Extended Limits
//
#define JOB_OBJECT_LIMIT_PROCESS_MEMORY             0x00000100
#define JOB_OBJECT_LIMIT_JOB_MEMORY                 0x00000200
#define JOB_OBJECT_LIMIT_JOB_MEMORY_HIGH            JOB_OBJECT_LIMIT_JOB_MEMORY
#define JOB_OBJECT_LIMIT_DIE_ON_UNHANDLED_EXCEPTION 0x00000400
#define JOB_OBJECT_LIMIT_BREAKAWAY_OK               0x00000800
#define JOB_OBJECT_LIMIT_SILENT_BREAKAWAY_OK        0x00001000
#define JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE          0x00002000
#define JOB_OBJECT_LIMIT_SUBSET_AFFINITY            0x00004000
#define JOB_OBJECT_LIMIT_JOB_MEMORY_LOW             0x00008000

//
// Notification Limits
//

#define JOB_OBJECT_LIMIT_JOB_READ_BYTES             0x00010000
#define JOB_OBJECT_LIMIT_JOB_WRITE_BYTES            0x00020000
#define JOB_OBJECT_LIMIT_RATE_CONTROL               0x00040000
#define JOB_OBJECT_LIMIT_CPU_RATE_CONTROL           JOB_OBJECT_LIMIT_RATE_CONTROL
#define JOB_OBJECT_LIMIT_IO_RATE_CONTROL            0x00080000
#define JOB_OBJECT_LIMIT_NET_RATE_CONTROL           0x00100000

//
//

//
// Valid Job Object Limits
//

#define JOB_OBJECT_LIMIT_VALID_FLAGS                 0x0007ffff
#define JOB_OBJECT_BASIC_LIMIT_VALID_FLAGS           0x000000ff
#define JOB_OBJECT_EXTENDED_LIMIT_VALID_FLAGS        0x00007fff
#define JOB_OBJECT_NOTIFICATION_LIMIT_VALID_FLAGS \
    (JOB_OBJECT_LIMIT_JOB_READ_BYTES | \
     JOB_OBJECT_LIMIT_JOB_WRITE_BYTES | \
     JOB_OBJECT_LIMIT_JOB_TIME | \
     JOB_OBJECT_LIMIT_JOB_MEMORY_LOW | \
     JOB_OBJECT_LIMIT_JOB_MEMORY_HIGH | \
     JOB_OBJECT_LIMIT_CPU_RATE_CONTROL | \
     JOB_OBJECT_LIMIT_IO_RATE_CONTROL | \
     JOB_OBJECT_LIMIT_NET_RATE_CONTROL)

//
//

//
// UI restrictions for jobs
//

#define JOB_OBJECT_UILIMIT_NONE             0x00000000

#define JOB_OBJECT_UILIMIT_HANDLES          0x00000001
#define JOB_OBJECT_UILIMIT_READCLIPBOARD    0x00000002
#define JOB_OBJECT_UILIMIT_WRITECLIPBOARD   0x00000004
#define JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS 0x00000008
#define JOB_OBJECT_UILIMIT_DISPLAYSETTINGS  0x00000010
#define JOB_OBJECT_UILIMIT_GLOBALATOMS      0x00000020
#define JOB_OBJECT_UILIMIT_DESKTOP          0x00000040
#define JOB_OBJECT_UILIMIT_EXITWINDOWS      0x00000080
#define JOB_OBJECT_UILIMIT_IME              0x00000100

#define JOB_OBJECT_UILIMIT_ALL              0x000001FF

#define JOB_OBJECT_UI_VALID_FLAGS           0x000001FF

#define JOB_OBJECT_SECURITY_NO_ADMIN            0x00000001
#define JOB_OBJECT_SECURITY_RESTRICTED_TOKEN    0x00000002
#define JOB_OBJECT_SECURITY_ONLY_TOKEN          0x00000004
#define JOB_OBJECT_SECURITY_FILTER_TOKENS       0x00000008

#define JOB_OBJECT_SECURITY_VALID_FLAGS         0x0000000f

//
// Control flags for CPU rate control.
//

#define JOB_OBJECT_CPU_RATE_CONTROL_ENABLE 0x1
#define JOB_OBJECT_CPU_RATE_CONTROL_WEIGHT_BASED 0x2
#define JOB_OBJECT_CPU_RATE_CONTROL_HARD_CAP 0x4
#define JOB_OBJECT_CPU_RATE_CONTROL_NOTIFY 0x8
#define JOB_OBJECT_CPU_RATE_CONTROL_MIN_MAX_RATE 0x10
#define JOB_OBJECT_CPU_RATE_CONTROL_VALID_FLAGS 0x1f

//
//

//@[comment("MVI_tracked")]
typedef enum _JOBOBJECTINFOCLASS {
    JobObjectBasicAccountingInformation = 1,
    JobObjectBasicLimitInformation,
    JobObjectBasicProcessIdList,
    JobObjectBasicUIRestrictions,
    JobObjectSecurityLimitInformation,  // deprecated
    JobObjectEndOfJobTimeInformation,
    JobObjectAssociateCompletionPortInformation,
    JobObjectBasicAndIoAccountingInformation,
    JobObjectExtendedLimitInformation,
    JobObjectJobSetInformation,
    JobObjectGroupInformation,
    JobObjectNotificationLimitInformation,
    JobObjectLimitViolationInformation,
    JobObjectGroupInformationEx,
    JobObjectCpuRateControlInformation,
    JobObjectCompletionFilter,
    JobObjectCompletionCounter,

    //
    //

    JobObjectReserved1Information = 18,
    JobObjectReserved2Information,
    JobObjectReserved3Information,
    JobObjectReserved4Information,
    JobObjectReserved5Information,
    JobObjectReserved6Information,
    JobObjectReserved7Information,
    JobObjectReserved8Information,
    JobObjectReserved9Information,
    JobObjectReserved10Information,
    JobObjectReserved11Information,
    JobObjectReserved12Information,
    JobObjectReserved13Information,
    JobObjectReserved14Information = 31,
    JobObjectNetRateControlInformation,
    JobObjectNotificationLimitInformation2,
    JobObjectLimitViolationInformation2,
    JobObjectCreateSilo,
    JobObjectSiloBasicInformation,
    JobObjectReserved15Information = 37,
    JobObjectReserved16Information = 38,
    JobObjectReserved17Information = 39,
    JobObjectReserved18Information = 40,
    JobObjectReserved19Information = 41,
    JobObjectReserved20Information = 42,
    JobObjectReserved21Information = 43,
    JobObjectReserved22Information = 44,
    JobObjectReserved23Information = 45,
    JobObjectReserved24Information = 46,
    JobObjectReserved25Information = 47,
    JobObjectReserved26Information = 48,
    JobObjectReserved27Information = 49,
    MaxJobObjectInfoClass
} JOBOBJECTINFOCLASS;

//
//

typedef struct _SILOOBJECT_BASIC_INFORMATION {
    DWORD SiloId;
    DWORD SiloParentId;
    DWORD NumberOfProcesses;
    BOOLEAN IsInServerSilo;
    BYTE  Reserved[3];
} SILOOBJECT_BASIC_INFORMATION, * PSILOOBJECT_BASIC_INFORMATION;

typedef enum _SERVERSILO_STATE {
    SERVERSILO_INITING = 0,
    SERVERSILO_STARTED,
    SERVERSILO_SHUTTING_DOWN,
    SERVERSILO_TERMINATING,
    SERVERSILO_TERMINATED,
} SERVERSILO_STATE, * PSERVERSILO_STATE;

typedef struct _SERVERSILO_BASIC_INFORMATION {
    DWORD ServiceSessionId;
    SERVERSILO_STATE State;
    DWORD    ExitStatus;
    BOOLEAN IsDownlevelContainer;
    PVOID ApiSetSchema;
    PVOID HostApiSetSchema;
} SERVERSILO_BASIC_INFORMATION, * PSERVERSILO_BASIC_INFORMATION;

#if _MSC_VER < 1900
#define DECLSPEC_ALLOCATOR
#else
#define DECLSPEC_ALLOCATOR __declspec(allocator)
#endif

#ifndef DECLSPEC_NORETURN
#if (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#define DECLSPEC_NORETURN   __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif

#ifndef DECLSPEC_NOTHROW
#if (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#define DECLSPEC_NOTHROW   __declspec(nothrow)
#else
#define DECLSPEC_NOTHROW
#endif
#endif

#ifndef DECLSPEC_RESTRICT
#if (_MSC_VER >= 1915) && !defined(MIDL_PASS)
#define DECLSPEC_RESTRICT   __declspec(restrict)
#else
#define DECLSPEC_RESTRICT
#endif
#endif

#ifndef DECLSPEC_ALIGN
#if (_MSC_VER >= 1300) && !defined(MIDL_PASS)
#define DECLSPEC_ALIGN(x)   __declspec(align(x))
#else
#define DECLSPEC_ALIGN(x)
#endif
#endif

typedef enum _ACTIVATION_CONTEXT_INFO_CLASS {
    ActivationContextBasicInformation = 1,
    ActivationContextDetailedInformation = 2,
    AssemblyDetailedInformationInActivationContext = 3,
    FileInformationInAssemblyOfAssemblyInActivationContext = 4,
    RunlevelInformationInActivationContext = 5,
    CompatibilityInformationInActivationContext = 6,
    ActivationContextManifestResourceName = 7,
    MaxActivationContextInfoClass,

    //
    // compatibility with old names
    //
    AssemblyDetailedInformationInActivationContxt = 3,
    FileInformationInAssemblyOfAssemblyInActivationContxt = 4
} ACTIVATION_CONTEXT_INFO_CLASS;

#define ACTIVATIONCONTEXTINFOCLASS ACTIVATION_CONTEXT_INFO_CLASS


typedef struct _ACTIVATION_CONTEXT_QUERY_INDEX {
    DWORD ulAssemblyIndex;
    DWORD ulFileIndexInAssembly;
} ACTIVATION_CONTEXT_QUERY_INDEX, * PACTIVATION_CONTEXT_QUERY_INDEX;

typedef const struct _ACTIVATION_CONTEXT_QUERY_INDEX* PCACTIVATION_CONTEXT_QUERY_INDEX;


#define ACTIVATION_CONTEXT_PATH_TYPE_NONE (1)
#define ACTIVATION_CONTEXT_PATH_TYPE_WIN32_FILE (2)
#define ACTIVATION_CONTEXT_PATH_TYPE_URL (3)
#define ACTIVATION_CONTEXT_PATH_TYPE_ASSEMBLYREF (4)

typedef struct _ASSEMBLY_FILE_DETAILED_INFORMATION {
    DWORD ulFlags;
    DWORD ulFilenameLength;
    DWORD ulPathLength;

    PCWSTR lpFileName;
    PCWSTR lpFilePath;
} ASSEMBLY_FILE_DETAILED_INFORMATION, * PASSEMBLY_FILE_DETAILED_INFORMATION;
typedef const ASSEMBLY_FILE_DETAILED_INFORMATION* PCASSEMBLY_FILE_DETAILED_INFORMATION;

//
// compatibility with old names
// The new names use "file" consistently.
//
#define  _ASSEMBLY_DLL_REDIRECTION_DETAILED_INFORMATION  _ASSEMBLY_FILE_DETAILED_INFORMATION
#define   ASSEMBLY_DLL_REDIRECTION_DETAILED_INFORMATION   ASSEMBLY_FILE_DETAILED_INFORMATION
#define  PASSEMBLY_DLL_REDIRECTION_DETAILED_INFORMATION  PASSEMBLY_FILE_DETAILED_INFORMATION
#define PCASSEMBLY_DLL_REDIRECTION_DETAILED_INFORMATION PCASSEMBLY_FILE_DETAILED_INFORMATION

typedef struct _ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION {
    DWORD ulFlags;
    DWORD ulEncodedAssemblyIdentityLength;      // in bytes
    DWORD ulManifestPathType;                   // ACTIVATION_CONTEXT_PATH_TYPE_*
    DWORD ulManifestPathLength;                 // in bytes
    LARGE_INTEGER liManifestLastWriteTime;      // FILETIME
    DWORD ulPolicyPathType;                     // ACTIVATION_CONTEXT_PATH_TYPE_*
    DWORD ulPolicyPathLength;                   // in bytes
    LARGE_INTEGER liPolicyLastWriteTime;        // FILETIME
    DWORD ulMetadataSatelliteRosterIndex;

    DWORD ulManifestVersionMajor;               // 1
    DWORD ulManifestVersionMinor;               // 0
    DWORD ulPolicyVersionMajor;                 // 0
    DWORD ulPolicyVersionMinor;                 // 0
    DWORD ulAssemblyDirectoryNameLength;        // in bytes

    PCWSTR lpAssemblyEncodedAssemblyIdentity;
    PCWSTR lpAssemblyManifestPath;
    PCWSTR lpAssemblyPolicyPath;
    PCWSTR lpAssemblyDirectoryName;

    DWORD  ulFileCount;
} ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION, * PACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION;

typedef const struct _ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION* PCACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION;

typedef enum
{
    ACTCTX_RUN_LEVEL_UNSPECIFIED = 0,
    ACTCTX_RUN_LEVEL_AS_INVOKER,
    ACTCTX_RUN_LEVEL_HIGHEST_AVAILABLE,
    ACTCTX_RUN_LEVEL_REQUIRE_ADMIN,
    ACTCTX_RUN_LEVEL_NUMBERS
} ACTCTX_REQUESTED_RUN_LEVEL;

typedef struct _ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION {
    DWORD ulFlags;
    ACTCTX_REQUESTED_RUN_LEVEL  RunLevel;
    DWORD UiAccess;
} ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION, * PACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION;

typedef const struct _ACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION* PCACTIVATION_CONTEXT_RUN_LEVEL_INFORMATION;

typedef enum
{
    ACTCTX_COMPATIBILITY_ELEMENT_TYPE_UNKNOWN = 0,
    ACTCTX_COMPATIBILITY_ELEMENT_TYPE_OS,
    ACTCTX_COMPATIBILITY_ELEMENT_TYPE_MITIGATION,
    ACTCTX_COMPATIBILITY_ELEMENT_TYPE_MAXVERSIONTESTED
} ACTCTX_COMPATIBILITY_ELEMENT_TYPE;

typedef struct _COMPATIBILITY_CONTEXT_ELEMENT {
    GUID Id;
    ACTCTX_COMPATIBILITY_ELEMENT_TYPE Type;
    ULONGLONG MaxVersionTested;
} COMPATIBILITY_CONTEXT_ELEMENT, * PCOMPATIBILITY_CONTEXT_ELEMENT;

typedef const struct _COMPATIBILITY_CONTEXT_ELEMENT* PCCOMPATIBILITY_CONTEXT_ELEMENT;

#if (_MSC_FULL_VER >= 140041204) && !defined(MIDL_PASS) && !defined(RC_INVOKED)
#define POINTER_SIGNED __sptr
#define POINTER_UNSIGNED __uptr
#else
#define POINTER_SIGNED
#define POINTER_UNSIGNED
#endif

//
// System time is represented with the following structure:
//

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, * PSYSTEMTIME, * LPSYSTEMTIME;


typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, * PTIME_ZONE_INFORMATION, * LPTIME_ZONE_INFORMATION;

typedef struct _TIME_DYNAMIC_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[32];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[32];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
    WCHAR TimeZoneKeyName[128];
    BOOLEAN DynamicDaylightTimeDisabled;
} DYNAMIC_TIME_ZONE_INFORMATION, * PDYNAMIC_TIME_ZONE_INFORMATION;

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, * PRECT, NEAR* NPRECT, FAR* LPRECT;

typedef const RECT FAR* LPCRECT;

typedef struct _RECTL       /* rcl */
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECTL, * PRECTL, * LPRECTL;

typedef const RECTL FAR* LPCRECTL;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, * PPOINT, NEAR* NPPOINT, FAR* LPPOINT;

typedef struct _POINTL      /* ptl  */
{
    LONG  x;
    LONG  y;
} POINTL, * PPOINTL;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, * PSIZE, * LPSIZE;

typedef SIZE               SIZEL;
typedef SIZE* PSIZEL, * LPSIZEL;

typedef struct tagPOINTS
{
#ifndef _MAC
    SHORT   x;
    SHORT   y;
#else
    SHORT   y;
    SHORT   x;
#endif
} POINTS, * PPOINTS, * LPPOINTS;

typedef struct tagACTCTX_SECTION_KEYED_DATA_2600 {
    ULONG cbSize;
    ULONG ulDataFormatVersion;
    PVOID lpData;
    ULONG ulLength;
    PVOID lpSectionGlobalData;
    ULONG ulSectionGlobalDataLength;
    PVOID lpSectionBase;
    ULONG ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG ulAssemblyRosterIndex;
} ACTCTX_SECTION_KEYED_DATA_2600, * PACTCTX_SECTION_KEYED_DATA_2600;
typedef const ACTCTX_SECTION_KEYED_DATA_2600* PCACTCTX_SECTION_KEYED_DATA_2600;

typedef struct tagACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA {
    PVOID lpInformation;
    PVOID lpSectionBase;
    ULONG ulSectionLength;
    PVOID lpSectionGlobalDataBase;
    ULONG ulSectionGlobalDataLength;
} ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA, * PACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;
typedef const ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA* PCACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA;

typedef struct tagACTCTX_SECTION_KEYED_DATA {
    ULONG cbSize;
    ULONG ulDataFormatVersion;
    PVOID lpData;
    ULONG ulLength;
    PVOID lpSectionGlobalData;
    ULONG ulSectionGlobalDataLength;
    PVOID lpSectionBase;
    ULONG ulSectionTotalLength;
    HANDLE hActCtx;
    ULONG ulAssemblyRosterIndex;
    // 2600 stops here
    ULONG ulFlags;
    ACTCTX_SECTION_KEYED_DATA_ASSEMBLY_METADATA AssemblyMetadata;
} ACTCTX_SECTION_KEYED_DATA, * PACTCTX_SECTION_KEYED_DATA;
typedef const ACTCTX_SECTION_KEYED_DATA* PCACTCTX_SECTION_KEYED_DATA;

EXTERN_C
NTSYSAPI
PVOID
NTAPI
RtlPcToFileHeader(
    _In_ PVOID PcValue,
    _Out_ PVOID* BaseOfImage
);

//
// Service Types (Bit Mask)
//
#define SERVICE_KERNEL_DRIVER          0x00000001
#define SERVICE_FILE_SYSTEM_DRIVER     0x00000002
#define SERVICE_ADAPTER                0x00000004
#define SERVICE_RECOGNIZER_DRIVER      0x00000008

#define SERVICE_DRIVER                 (SERVICE_KERNEL_DRIVER | \
                                        SERVICE_FILE_SYSTEM_DRIVER | \
                                        SERVICE_RECOGNIZER_DRIVER)

#define SERVICE_WIN32_OWN_PROCESS      0x00000010
#define SERVICE_WIN32_SHARE_PROCESS    0x00000020
#define SERVICE_WIN32                  (SERVICE_WIN32_OWN_PROCESS | \
                                        SERVICE_WIN32_SHARE_PROCESS)

#define SERVICE_USER_SERVICE           0x00000040
#define SERVICE_USERSERVICE_INSTANCE   0x00000080

#define SERVICE_USER_SHARE_PROCESS     (SERVICE_USER_SERVICE | \
                                        SERVICE_WIN32_SHARE_PROCESS)
#define SERVICE_USER_OWN_PROCESS       (SERVICE_USER_SERVICE | \
                                        SERVICE_WIN32_OWN_PROCESS)

#define SERVICE_INTERACTIVE_PROCESS    0x00000100
#define SERVICE_PKG_SERVICE            0x00000200

#define SERVICE_TYPE_ALL               (SERVICE_WIN32  | \
                                        SERVICE_ADAPTER | \
                                        SERVICE_DRIVER  | \
                                        SERVICE_INTERACTIVE_PROCESS | \
                                        SERVICE_USER_SERVICE | \
                                        SERVICE_USERSERVICE_INSTANCE | \
                                        SERVICE_PKG_SERVICE)

//
// Start Type
//

#define SERVICE_BOOT_START             0x00000000
#define SERVICE_SYSTEM_START           0x00000001
#define SERVICE_AUTO_START             0x00000002
#define SERVICE_DEMAND_START           0x00000003
#define SERVICE_DISABLED               0x00000004

//
// Error control type
//
#define SERVICE_ERROR_IGNORE           0x00000000
#define SERVICE_ERROR_NORMAL           0x00000001
#define SERVICE_ERROR_SEVERE           0x00000002
#define SERVICE_ERROR_CRITICAL         0x00000003

#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


//
//

typedef enum _LOGICAL_PROCESSOR_RELATIONSHIP {
    RelationProcessorCore,
    RelationNumaNode,
    RelationCache,
    RelationProcessorPackage,
    RelationGroup,
    RelationProcessorDie,
    RelationNumaNodeEx,
    RelationProcessorModule,
    RelationAll = 0xffff
} LOGICAL_PROCESSOR_RELATIONSHIP;

#define LTP_PC_SMT 0x1

//
//

typedef enum _PROCESSOR_CACHE_TYPE {
    CacheUnified,
    CacheInstruction,
    CacheData,
    CacheTrace,
    CacheUnknown
} PROCESSOR_CACHE_TYPE, * PPROCESSOR_CACHE_TYPE;

//
//

#define CACHE_FULLY_ASSOCIATIVE 0xFF

typedef struct _CACHE_DESCRIPTOR {
    BYTE   Level;
    BYTE   Associativity;
    WORD   LineSize;
    DWORD  Size;
    PROCESSOR_CACHE_TYPE Type;
} CACHE_DESCRIPTOR, * PCACHE_DESCRIPTOR;

typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION {
    ULONG_PTR   ProcessorMask;
    LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    union {
        struct {
            BYTE  Flags;
        } ProcessorCore;
        struct {
            DWORD NodeNumber;
        } NumaNode;
        CACHE_DESCRIPTOR Cache;
        ULONGLONG  Reserved[2];
    } DUMMYUNIONNAME;
} SYSTEM_LOGICAL_PROCESSOR_INFORMATION, * PSYSTEM_LOGICAL_PROCESSOR_INFORMATION;

typedef struct _PROCESSOR_RELATIONSHIP {
    BYTE  Flags;
    BYTE  EfficiencyClass;
    BYTE  Reserved[20];
    WORD   GroupCount;
    _Field_size_(GroupCount) GROUP_AFFINITY GroupMask[ANYSIZE_ARRAY];
} PROCESSOR_RELATIONSHIP, * PPROCESSOR_RELATIONSHIP;

typedef struct _NUMA_NODE_RELATIONSHIP {
    DWORD NodeNumber;
    BYTE  Reserved[18];
    WORD   GroupCount;
    union {
        GROUP_AFFINITY GroupMask;
        _Field_size_(GroupCount)
            GROUP_AFFINITY GroupMasks[ANYSIZE_ARRAY];
    } DUMMYUNIONNAME;
} NUMA_NODE_RELATIONSHIP, * PNUMA_NODE_RELATIONSHIP;

typedef struct _CACHE_RELATIONSHIP {
    BYTE  Level;
    BYTE  Associativity;
    WORD   LineSize;
    DWORD CacheSize;
    PROCESSOR_CACHE_TYPE Type;
    BYTE  Reserved[18];
    WORD   GroupCount;
    union {
        GROUP_AFFINITY GroupMask;
        _Field_size_(GroupCount)
            GROUP_AFFINITY GroupMasks[ANYSIZE_ARRAY];
    } DUMMYUNIONNAME;
} CACHE_RELATIONSHIP, * PCACHE_RELATIONSHIP;

typedef struct _PROCESSOR_GROUP_INFO {
    BYTE  MaximumProcessorCount;
    BYTE  ActiveProcessorCount;
    BYTE  Reserved[38];
    KAFFINITY ActiveProcessorMask;
} PROCESSOR_GROUP_INFO, * PPROCESSOR_GROUP_INFO;

typedef struct _GROUP_RELATIONSHIP {
    WORD   MaximumGroupCount;
    WORD   ActiveGroupCount;
    BYTE  Reserved[20];
    _Field_size_(ActiveGroupCount) PROCESSOR_GROUP_INFO GroupInfo[ANYSIZE_ARRAY];
} GROUP_RELATIONSHIP, * PGROUP_RELATIONSHIP;

_Struct_size_bytes_(Size) struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX {
    LOGICAL_PROCESSOR_RELATIONSHIP Relationship;
    DWORD Size;
    union {
        PROCESSOR_RELATIONSHIP Processor;
        NUMA_NODE_RELATIONSHIP NumaNode;
        CACHE_RELATIONSHIP Cache;
        GROUP_RELATIONSHIP Group;
    } DUMMYUNIONNAME;
};

typedef struct _SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, * PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX;

typedef enum _CPU_SET_INFORMATION_TYPE {
    CpuSetInformation
} CPU_SET_INFORMATION_TYPE, * PCPU_SET_INFORMATION_TYPE;

_Struct_size_bytes_(Size) struct _SYSTEM_CPU_SET_INFORMATION {
    DWORD Size;
    CPU_SET_INFORMATION_TYPE Type;
    union {
        struct {
            DWORD Id;
            WORD   Group;
            BYTE  LogicalProcessorIndex;
            BYTE  CoreIndex;
            BYTE  LastLevelCacheIndex;
            BYTE  NumaNodeIndex;
            BYTE  EfficiencyClass;
            union {

#define SYSTEM_CPU_SET_INFORMATION_PARKED 0x1
#define SYSTEM_CPU_SET_INFORMATION_ALLOCATED 0x2
#define SYSTEM_CPU_SET_INFORMATION_ALLOCATED_TO_TARGET_PROCESS 0x4
#define SYSTEM_CPU_SET_INFORMATION_REALTIME 0x8

                BYTE  AllFlags;
                struct {
                    BYTE  Parked : 1;
                    BYTE  Allocated : 1;
                    BYTE  AllocatedToTargetProcess : 1;
                    BYTE  RealTime : 1;
                    BYTE  ReservedFlags : 4;
                } DUMMYSTRUCTNAME;
            } DUMMYUNIONNAME2;

            union {
                DWORD Reserved;
                BYTE  SchedulingClass;
            };

            DWORD64 AllocationTag;
        } CpuSet;
    } DUMMYUNIONNAME;
};

typedef struct _SYSTEM_CPU_SET_INFORMATION SYSTEM_CPU_SET_INFORMATION, * PSYSTEM_CPU_SET_INFORMATION;

#endif // !_WINDOWS_