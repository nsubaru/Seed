// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Seed.Core.Memory.h"
#include "Seed.Core.Exceptions.Internals.h"

/////////////////////////////////////////////////////////////////////////////
//
// _CxxThrowException - implementation of 'throw'
//
// Description:
//      Builds the NT Exception record, and calls the NT runtime to initiate
//      exception processing.
//
//      Why is pThrowInfo defined as _ThrowInfo?  Because _ThrowInfo is secretly
//      snuck into the compiler, as is the prototype for _CxxThrowException, so
//      we have to use the same type to keep the compiler happy.
//
//      Another result of this is that _CRTIMP can't be used here.  Instead, we
//      synthesisze the -export directive below.
//
// Returns:
//      NEVER.  (until we implement resumable exceptions, that is)
//
extern "C" __declspec(noreturn) void __stdcall
_CxxThrowException(
	void* pExceptionObject,   // The object thrown
	_ThrowInfo* pThrowInfo          // Everything we need to know about it
) {
	static const EHExceptionRecord ExceptionTemplate = { // A generic exception record
		EH_EXCEPTION_NUMBER,            // Exception number
		EXCEPTION_NONCONTINUABLE,       // Exception flags (we don't do resume)
		nullptr,                           // Additional record (none)
		nullptr,                           // Address of exception (OS fills in)
		4,        // Number of parameters
		{   EH_MAGIC_NUMBER1,           // Our version control magic number
			nullptr,                       // pExceptionObject
			nullptr,
			nullptr                        // Image base of thrown object
			}                      // pThrowInfo
	};
	union RawExceptionInfo {
		EXCEPTION_RECORD NtExceptionInfo;
		EHExceptionRecord CxxExceptionInfo;
	};

	RawExceptionInfo ThisException = { 0 };
	ThisException.CxxExceptionInfo = ExceptionTemplate;    // This exception

	ThrowInfo* pTI = (ThrowInfo*)pThrowInfo;

	//
	// Fill in the blanks:
	//
	ThisException.CxxExceptionInfo.params.pExceptionObject = pExceptionObject;
	ThisException.CxxExceptionInfo.params.pThrowInfo = pTI;
	PVOID ThrowImageBase = RtlPcToFileHeader((PVOID)pTI, &ThrowImageBase);
	ThisException.CxxExceptionInfo.params.pThrowImageBase = ThrowImageBase;

	//
	// If the throw info indicates this throw is from a pure region,
	// set the magic number to the Pure one, so only a pure-region
	// catch will see it.
	//
	// Also use the Pure magic number on Win64 if we were unable to
	// determine an image base, since that was the old way to determine
	// a pure throw, before the TI_IsPure bit was added to the FuncInfo
	// attributes field.
	//

	//
	// Hand it off to the OS:
	//
	
	RtlRaiseException(reinterpret_cast<EXCEPTION_RECORD*>(&ThisException.NtExceptionInfo));
}