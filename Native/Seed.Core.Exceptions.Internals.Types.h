#pragma once
#include "Seed.Core.Base.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

typedef	int	PMFN;					// Image relative offset of Member Function

//
// ThrowInfo - information describing the thrown object, statically built
// at the throw site.
//
// pExceptionObject (the dynamic part of the throw; see below) is always a
// reference, whether or not it is logically one.  If 'isSimpleType' is true,
// it is a reference to the simple type, which is 'size' bytes long.  If
// 'isReference' and 'isSimpleType' are both false, then it's a UDT or
// a pointer to any type (i.e. pExceptionObject points to a pointer).  If it's
// a pointer, copyFunction is NULL, otherwise it is a pointer to a copy
// constructor or copy constructor closure.
//
// The pForwardCompat function pointer is intended to be filled in by future
// versions, so that if say a DLL built with a newer version (say C10) throws,
// and a C9 frame attempts a catch, the frame handler attempting the catch (C9)
// can let the version that knows all the latest stuff do the work.
//
typedef const struct _s_ThrowInfo {
	unsigned int	attributes;							// Throw Info attributes (Bit field)
	PMFN			pmfnUnwind;							// Destructor to call when exception has been handled or aborted
	int				pForwardCompat;						// Image relative offset of Forward compatibility frame handler
	int				pCatchableTypeArray;				// Image relative offset of CatchableTypeArray
} ThrowInfo;

/////////////////////////////////////////////////////////////////////////////
//
// The NT Exception record that we use to pass information from the throw to
// the possible catches.
//
// The constants in the comments are the values we expect.
// This is based on the definition of EXCEPTION_RECORD in winnt.h.
//
#if defined(_WIN64)
#pragma pack(push, EHExceptionRecord, 8)
#endif
typedef struct EHExceptionRecord {
	unsigned long	ExceptionCode;			// The code of this exception. (= EH_EXCEPTION_NUMBER)
	unsigned long	ExceptionFlags;			// Flags determined by NT
	struct _EXCEPTION_RECORD *	ExceptionRecord;	// An extra exception record (not used)
	void *		ExceptionAddress;		// Address at which exception occurred
	unsigned long 		NumberParameters;		// Number of extended parameters. (= EH_EXCEPTION_PARAMETERS)
	struct EHParameters {
		unsigned long long		magicNumber;		// = EH_MAGIC_NUMBER1
		void *		pExceptionObject;	// Pointer to the actual object thrown
		ThrowInfo*	pThrowInfo;		// Description of thrown object
		void *		pThrowImageBase;	// Image base of thrown object
	} params;
} EHExceptionRecord;

#if defined(_WIN64)
#pragma pack(pop, EHExceptionRecord)
#endif

//On ARM64 we don't have a registration node, just a pointer to the stack frame base
typedef void* EHRegistrationNode;

#pragma warning (push)
#pragma warning (disable: 4201)

typedef union _UNWIND_CODE {
	struct {
		unsigned char CodeOffset;
		unsigned char UnwindOp : 4;
		unsigned char OpInfo : 4;
	};
	unsigned short FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;
typedef struct _UNWIND_INFO {
	unsigned char Version : 3;
	unsigned char Flags : 5;
	unsigned char SizeOfProlog;
	unsigned char CountOfCodes;
	unsigned char FrameRegister : 4;
	unsigned char FrameOffset : 4;
	UNWIND_CODE UnwindCode[1];
	/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes+1)&~1)-1];
	 *  union {
	 *      OPTIONAL unsigned long ExceptionHandler;
	 *      OPTIONAL unsigned long FunctionEntry;
	 *  };
	 *  OPTIONAL unsigned long ExceptionData[];
	 */
} UNWIND_INFO, *PUNWIND_INFO;

#pragma warning (pop)

typedef struct _UNWIND_HISTORY_TABLE* PUNWIND_HISTORY_TABLE;

//
// Current state of a function.
// -1 is the 'blank' state, i.e. there is nothing to unwind, no try blocks active.
//

typedef int __ehstate_t;		// The type of a state index

/*
 * The magicNumber here is incremented with every compiler change that does not
 * break backwards compatibility. If for some reason backward compatibility
 * should be broken, then we will add new handler. What this means is that
 * current handler functions can assume that the structure layout that they
 * know about will remain the same and so even if magicNumber > my_magicNumber,
 * the handler can assume that what all it needs is there. The magicNumber will
 * be revised every time new data is added at the end of this structure.
 */
typedef const struct _s_FuncInfo
{
	unsigned int		magicNumber : 29;		// Identifies version of compiler
	unsigned int		bbtFlags : 3;			// flags that may be set by BBT processing
	__ehstate_t			maxState;			// Highest state number plus one (thus number of entries in unwind map)
	int					dispUnwindMap;		// Image relative offset of the unwind map
	unsigned int		nTryBlocks;			// Number of 'try' blocks in this function
	int					dispTryBlockMap;	// Image relative offset of the handler map
	unsigned int		nIPMapEntries;		// # entries in the IP-to-state map. NYI (reserved)
	int					dispIPtoStateMap;	// Image relative offset of the IP to state map
	int					dispUwindHelp;		// Displacement of unwind helpers from base
	int					dispESTypeList;		// Image relative list of types for exception specifications
	int					EHFlags;			// Flags for some features.
} FuncInfo;

//SEH Exeption code for C++ exeptions
#define EH_EXCEPTION_NUMBER	('seed' | 0xE0000000)

// EH_MAGIC_NUMBER3     Used in the FuncInfo if FuncInfo::EHFlags is valid, so
//                      we can check if the function was compiled -EHs or -EHa.
#define EH_MAGIC_NUMBER3        0x19930522

typedef void(__cdecl* unexpected_handler)();
typedef void(__cdecl* unexpected_function)();
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//
// Per-thread data
//
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct
{
	// C++ Exception Handling (EH) state
	unsigned long      _NLG_dwCode;      // Required by NLG routines
	unexpected_handler _unexpected;      // unexpected() routine
	void*              _translator;      // S.E. translator
	void*              _purecall;        // called when pure virtual happens
	void*              _curexception;    // current exception
	void*              _curcontext;      // current exception context
	int                _ProcessingThrow; // for uncaught_exception
	void*              _curexcspec;      // for handling exceptions thrown from std::unexpected
	int                _cxxReThrow;      // true if it's a rethrown C++ exception
	void*              _pExitContext;
	void*              _pUnwindContext;
	void*              _pFrameInfoChain;
	unsigned __int64          _ImageBase;
	unsigned __int64          _ThrowImageBase;
	void*              _pForeignException;
	int                _HandlerSearchState;  // Used when exceptions occur in catch funclets in Pass 1 of SEH handling, represents a state in the throwing catch funclet
											 // so unwind can properly search through parent catch funclets.
	__int64          _UnwindStateorOffset; // In Pass 2 of SEH handling, represents the start state of the try enclosing the catch funclet. All states within the try
											 // will have been already unwound before calling the catch, so this prevents us from unwinding those states twice.
											 // Also used if an SEH exception is thrown out of a dtor, in which case it's encoded as an offset into the UW map.
	bool               _IsOffset;            // Whether the value in _UnwindStateorOffset is an offset from the start of the UW map or an actual state.
} tls_exeption_info_t;

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
#define EH_MAGIC_NUMBER1        0x19930520
#define EH_MAGIC_NUMBER2        0x19930521
#define EH_MAGIC_NUMBER3        0x19930522

typedef struct IptoStateMapEntry {
	unsigned int	Ip;		// Image relative offset of IP
	__ehstate_t		State;
} IptoStateMapEntry;

//
// UnwindMapEntry - Description of each state transition for unwinding
//	the stack (i.e. calling destructors).
//
// The unwind map is an array, indexed by current state.  Each entry specifies
// the state to go to during unwind, and the action required to get there.
// Note that states are represented by a signed integer, and that the 'blank'
// state is -1 so that the array remains 0-based (because by definition there
// is never any unwind action to be performed from state -1).  It is also
// assumed that state indices will be dense, i.e. that there will be no gaps of
// unused state indices in a function.
//
typedef const struct _s_UnwindMapEntry {
	__ehstate_t	toState;					// State this action takes us to
	int			action;						// Image relative offset of funclet
} UnwindMapEntry;

//
// HandlerMapEntry - associates a handler list (sequence of catches) with a
//	range of eh-states.
//
typedef const struct _s_TryBlockMapEntry {
	__ehstate_t		tryLow;				// Lowest state index of try
	__ehstate_t		tryHigh;			// Highest state index of try
	__ehstate_t		catchHigh;			// Highest state index of any associated catch
	int				nCatches;			// Number of entries in array
	int				dispHandlerArray;	// Image relative offset of list of handlers for this try
} TryBlockMapEntry;

//
// TypeDescriptor - per-type record which uniquely identifies the type.
//
// Each type has a decorated name which uniquely identifies it, and a hash
// value which is computed by the compiler.  The hash function used is not
// important; the only thing which is essential is that it be the same for
// all time.
//
// The special type '...' (ellipsis) is represented by a null name.
//
#pragma warning(push)
#pragma warning(disable:4200)	// nonstandard extension used: array of runtime bound
#pragma pack(push, TypeDescriptor, 8)

typedef struct TypeDescriptor
{
	const void * pVFTable;	// Field overloaded by RTTI
	void *	spare;			// reserved, possible for RTTI
	char			name[];			// The decorated name of the type; 0 terminated.
} TypeDescriptor;

#pragma pack(pop, TypeDescriptor)
#pragma warning(pop)

//
// HandlerType - description of a single 'catch'
//
typedef const struct _s_HandlerType {
	unsigned int	adjectives;			// Handler Type adjectives (bitfield)
	int				dispType;			// Image relative offset of the corresponding type descriptor
	int				dispCatchObj;		// Displacement of catch object from base
	int				dispOfHandler;		// Image relative offset of 'catch' code
	int				dispFrame;			// displacement of address of function frame wrt establisher frame
} HandlerType;

constexpr auto EH_EMPTY_STATE = -1;

typedef const struct _s_ESTypeList
{
	int 			nCount;			// how many types are there
	int				dispTypeArray;	// offset of list of types in exception specification
}ESTypeList;

//
// PMD - Pointer to Member Data: generalized pointer-to-member descriptor
//
typedef struct PMD
{
	int	mdisp;	// Offset of intended data within base
	int	pdisp;	// Displacement to virtual base pointer
	int	vdisp;	// Index within vbTable to offset of base
} PMD;

//
// CatchableType - description of a type that can be caught.
//
// Note:  although isSimpleType can be part of ThrowInfo, it is more
//		  convenient for the run-time to have it here.
//
typedef const struct _s_CatchableType {
	unsigned int		properties;			// Catchable Type properties (Bit field)
	int					pType;				// Image relative offset of TypeDescriptor
	PMD 				thisDisplacement;	// Pointer to instance of catch type within thrown object.
	int					sizeOrOffset;		// Size of simple-type object or offset into
											//  buffer of 'this' pointer for catch object
	PMFN				copyFunction;		// Copy constructor or CC-closure
} CatchableType;

//
// CatchableTypeArray - array of pointers to catchable types, with length
//
#pragma warning (push)
#pragma warning (disable:4200)	// nonstandard extension used: array of runtime bound
typedef const struct _s_CatchableTypeArray {
	int	nCatchableTypes;
	int				arrayOfCatchableTypes[];	// Image relative offset of Catchable Types
} CatchableTypeArray;
#pragma warning (pop)

#define HT_IsConst			0x00000001		// type referenced is 'const' qualified
#define HT_IsVolatile		0x00000002		// type referenced is 'volatile' qualified
#define HT_IsUnaligned		0x00000004		// type referenced is 'unaligned' qualified
#define HT_IsReference		0x00000008		// catch type is by reference
#define HT_IsResumable		0x00000010		// the catch may choose to resume (Reserved)
#define HT_IsStdDotDot      0x00000040      // the catch is std C++ catch(...) which is supposed to catch only C++ exceptions
#define HT_IsBadAllocCompat 0x00000080      // the WinRT type can catch a std::bad_alloc
#define HT_IsComplusEh      0x80000000      // Is handling within complus EH

#define CT_IsSimpleType			0x00000001		// type is a simple type
#define CT_ByReferenceOnly		0x00000002		// type must be caught by reference
#define CT_HasVirtualBase		0x00000004		// type is a class with virtual bases
#define CT_IsWinRTHandle		0x00000008		// type is a winrt handle
#define CT_IsStdBadAlloc		0x00000010		// type is a std::bad_alloc

#define TI_IsConst			0x00000001		// thrown object has const qualifier
#define TI_IsVolatile		0x00000002		// thrown object has volatile qualifier
#define TI_IsUnaligned		0x00000004		// thrown object has unaligned qualifier
#define TI_IsPure			0x00000008		// object thrown from a pure module
#define TI_IsWinRT          0x00000010      // object thrown is a WinRT Exception

struct TryBlockMap {
	FuncInfo * pFuncInfo;
	DISPATCHER_CONTEXT * pDC;
};

struct TryBlockMapIterator {
	TryBlockMap * entrys;
	unsigned int Index;
};

struct TryBlockMapIteratorPair {
	TryBlockMapIterator start;
	TryBlockMapIterator end;
};

typedef struct FrameInfo {
	PVOID               pExceptionObject;
	struct FrameInfo *  pNext;
} FRAMEINFO;
