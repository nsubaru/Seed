// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Threading.TLS.GlobalTLSBlock.h"

#include "Seed.Core.Exceptions.ABI.SEH.h"
#include "Seed.Core.Exceptions.ABI.Public.h"
#include "Seed.Core.Exceptions.ABI.Defines.h"
#include "Seed.Core.Exceptions.ABI.Types.h"
#include "Seed.Core.Exceptions.ABI.Internal.Types.h"
#include "Seed.Core.Exceptions.ABI.Internal.Tls.h"
#include "Seed.Core.Exceptions.ABI.Internal.Inlines.h"
#include "Seed.Core.Exceptions.ABI.Internal.HighLevelStructs.h"

static const EHExceptionRecord ExceptionTemplate = { // A generic exception record
		EH_EXCEPTION_NUMBER,            // Exception number
		EXCEPTION_NONCONTINUABLE,       // Exception flags (we don't do resume)
		nullptr,                           // Additional record (none)
		nullptr,                           // Address of exception (OS fills in)
		4,        // Number of parameters
		{   (unsigned long)EH_MAGIC_NUMBER1,           // Our version control magic number
			nullptr,                       // pExceptionObject
			nullptr,
			nullptr                        // Image base of thrown object
			}                      // pThrowInfo
	};

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
//      synthesize the -export directive below.
//
extern "C" __declspec(noreturn) void __stdcall _CxxThrowException(
    void *pExceptionObject, // The object thrown
    _ThrowInfo *pThrowInfo  // Everything we need to know about it
) {
    //EHTRACE_FMT1("Throwing object @ 0x%p", pExceptionObject);
    auto pTI = reinterpret_cast<ThrowInfo *>(pThrowInfo);
    ULONG_PTR magicNumber = EH_MAGIC_NUMBER1;

    void *throwImageBase = RtlPcToFileHeader(const_cast<void *>(static_cast<const void *>(pTI)), &throwImageBase);

    // If the throw info indicates this throw is from a pure region,
    // set the magic number to the Pure one, so only a pure-region
    // catch will see it.
    //
    // Also use the Pure magic number on Win64 if we were unable to
    // determine an image base, since that was the old way to determine
    // a pure throw, before the TI_IsPure bit was added to the FuncInfo
    // attributes field.

    union RawExceptionInfo {
		EXCEPTION_RECORD NtExceptionInfo;
		EHExceptionRecord CxxExceptionInfo;
	};

	RawExceptionInfo ThisException = { 0 };
	ThisException.CxxExceptionInfo = ExceptionTemplate;    // This exception

    //
	// Fill in the blanks:
	//
	ThisException.CxxExceptionInfo.params.pExceptionObject = pExceptionObject;
	ThisException.CxxExceptionInfo.params.pThrowInfo = pTI;
	ThisException.CxxExceptionInfo.params.pThrowImageBase = throwImageBase;

    // Hand it off to the OS:
    RtlRaiseException(reinterpret_cast<EXCEPTION_RECORD*>(&ThisException.NtExceptionInfo));
}