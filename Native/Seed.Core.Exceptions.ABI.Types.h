#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"

//
// PMFN - Pointer to Member Function
//
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

#ifdef __INTELLISENSE__
typedef ThrowInfo _ThrowInfo;
#endif


//
// On P7 we don't have a registration node, just a pointer to the stack frame base
//
typedef Seed::SizeT EHRegistrationNode;

/////////////////////////////////////////////////////////////////////////////
//
// The NT Exception record that we use to pass information from the throw to
// the possible catches.
//
// The constants in the comments are the values we expect.
// This is based on the definition of EXCEPTION_RECORD in winnt.h.
//

#pragma pack(push, EHExceptionRecord, 8)
typedef struct EHExceptionRecord {
	unsigned long	ExceptionCode;			// The code of this exception. (= EH_EXCEPTION_NUMBER)
	unsigned long	ExceptionFlags;			// Flags determined by NT
	struct _EXCEPTION_RECORD *	ExceptionRecord;	// An extra exception record (not used)
	void *		ExceptionAddress;		// Address at which exception occurred
	unsigned long 		NumberParameters;		// Number of extended parameters. (= EH_EXCEPTION_PARAMETERS)
	struct EHParameters {
		unsigned long		magicNumber;		// = EH_MAGIC_NUMBER1
		void *		pExceptionObject;	// Pointer to the actual object thrown
		ThrowInfo*	pThrowInfo;		// Description of thrown object
		void *		pThrowImageBase;	// Image base of thrown object
		} params;
} EHExceptionRecord;

#pragma pack(pop, EHExceptionRecord)

struct DispatcherContext {
	long long ControlPc;
	long long ImageBase;
    PRUNTIME_FUNCTION FunctionEntry;
    ULONG_PTR EstablisherFrame;
    ULONG64 TargetIp;
    PCONTEXT ContextRecord;
    PVOID LanguageHandler;
    PVOID HandlerData;
    PUNWIND_HISTORY_TABLE HistoryTable;
    ULONG ScopeIndex;
};					// changed the case of the name to conform to EH conventions