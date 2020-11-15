#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"
#include "Seed.Core.Exceptions.ABI.Defines.h"

typedef void (__cdecl* unexpected_handler )();
typedef void (__cdecl* unexpected_function)();

struct TlsExceptionData
{
    // C++ Exception Handling (EH) state
    unsigned long      _NLG_dwCode = 0;      // Required by NLG routines
    unexpected_handler _unexpected = 0;      // unexpected() routine
    void*              _translator = 0;      // S.E. translator
    void*              _purecall = 0;        // called when pure virtual happens
    void*              _curexception = 0;    // current exception
    void*              _curcontext = 0;      // current exception context
    int                _ProcessingThrow = 0; // for uncaught_exception
    int                _cxxReThrow = 0;      // true if it's a rethrown C++ exception
    void*              _curexcspec = 0;      // for handling exceptions thrown from std::unexpected

    void*              _pExitContext = 0;
    void*              _pUnwindContext = 0;
    void*              _pFrameInfoChain = 0;
    Seed::SizeT         _ImageBase = 0;
    Seed::SizeT         _ThrowImageBase = 0;
    void*              _pForeignException = 0;
    int                _CatchStateInParent = INVALID_CATCH_SPECIFIC_STATE;   
                                                // Used to link together the catch funclet with the parent. During dispatch contains state associated
                                                // with catch in the parent. During unwind represents the current unwind state that is resumed to
                                                // during collided unwind and used to look for handlers of the throwing dtor.
    
    void*              _throwObjStorageForStoredException = 0; //Stack allocated stored storage 

    bool               _isInCatch = false;            //Use for storing exception
};

TlsExceptionData* GetThreadExceptionData();