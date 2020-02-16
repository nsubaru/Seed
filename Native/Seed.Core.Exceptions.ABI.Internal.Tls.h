#pragma once
#include "Seed.Core.Exceptions.ABI.Base.h"


typedef void (__cdecl* unexpected_handler )();
typedef void (__cdecl* unexpected_function)();

struct TlsExceptionData
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
    Seed::SizeT         _ImageBase;
    Seed::SizeT         _ThrowImageBase;
    void*              _pForeignException;
    int                _CatchStateInParent;   // Used to link together the catch funclet with the parent. During dispatch contains state associated
                                              // with catch in the parent. During unwind represents the current unwind state that is resumed to
                                              // during collided unwind and used to look for handlers of the throwing dtor.
};

TlsExceptionData* GetThreadExceptionData();