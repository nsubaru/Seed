#pragma once
#include "Seed.Core.Exceptions.Processing.SeedRTData.h"

#ifdef _M_CEE_PURE
    #define __CLRCALL_PURE_OR_CDECL __clrcall
#else
    #define __CLRCALL_PURE_OR_CDECL __cdecl
#endif

#define __CRTDECL __CLRCALL_PURE_OR_CDECL

#define RENAME_BASE_PTD(x) x

typedef void (__CRTDECL* unexpected_handler )();
typedef void (__CRTDECL* unexpected_function)();

typedef struct RENAME_BASE_PTD(__vcrt_ptd)
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

    #if defined _M_X64 || defined _M_ARM || defined _M_ARM64 || defined _M_HYBRID
    void*              _pExitContext;
    void*              _pUnwindContext;
    void*              _pFrameInfoChain;
    uintptr_t          _ImageBase;
    uintptr_t          _ThrowImageBase;
    void*              _pForeignException;
    int                _CatchStateInParent;   // Used to link together the catch funclet with the parent. During dispatch contains state associated
                                              // with catch in the parent. During unwind represents the current unwind state that is resumed to
                                              // during collided unwind and used to look for handlers of the throwing dtor.
    #elif defined _M_IX86
    void*              _pFrameInfoChain;
    #endif

} RENAME_BASE_PTD(__vcrt_ptd);

__vcrt_ptd* __vcrt_getptd();

extern "C" void __cdecl unexpected() noexcept(false);