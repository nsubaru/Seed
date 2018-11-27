#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"

#include "Seed.Core.Base.NTAPI.h"
#include "Seed.Core.Exceptions.Internals.h"
#include "Seed.Core.Exceptions.Internals.SEH.h"

#include "typeinfo"

extern void * InternalCxxCallCatchBlock(
	EXCEPTION_RECORD *pExcept
);

extern EXCEPTION_DISPOSITION InternalCxxFrameHandler3(
	EHExceptionRecord  *pExcept,         // Information for this exception
	EHRegistrationNode RN,               // Dynamic information for this frame
	CONTEXT            *pContext,        // Context info
	DISPATCHER_CONTEXT  *pDC              // More dynamic info for this frame
);

void * CxxCallCatchBlock(
	EXCEPTION_RECORD *pExcept
)
{
	return InternalCxxCallCatchBlock(pExcept);
}

extern "C" EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3(
	EHExceptionRecord  *pExcept,         // Information for this exception
	EHRegistrationNode RN,               // Dynamic information for this frame
	CONTEXT            *pContext,        // Context info
	DISPATCHER_CONTEXT  *pDC              // More dynamic info for this frame
) {
	return InternalCxxFrameHandler3(pExcept, RN, pContext, pDC);
}