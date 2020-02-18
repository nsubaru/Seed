#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"
#include "Seed.Core.Exceptions.ABI.Defines.h"
#include "Seed.Core.Exceptions.ABI.Types.h"

extern "C" DECLSPEC_GUARD_SUPPRESS EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler4(
    EHExceptionRecord * pExcept,            // Information for this exception
    EHRegistrationNode RN,                  // Dynamic information for this frame
    CONTEXT * pContext,                     // Context info
    DispatcherContext * pDC                 // More dynamic info for this frame
);