#pragma once
#include "Seed.Core.Threading.Impl.h"

#include "Seed.Core.Exceptions.ABI.SEH.h"
#include "Seed.Core.Exceptions.ABI.Public.h"
#include "Seed.Core.Exceptions.ABI.Defines.h"
#include "Seed.Core.Exceptions.ABI.Types.h"
#include "Seed.Core.Exceptions.ABI.Internal.Types.h"
#include "Seed.Core.Exceptions.ABI.Internal.Tls.h"
#include "Seed.Core.Exceptions.ABI.Internal.Inlines.h"
#include "Seed.Core.Exceptions.ABI.Internal.HighLevelStructs.h"


////////////////////////////////////////////////////////////////////////////////
//
// __InternalCxxFrameHandler - the frame handler for all functions with C++ EH
// information.
//
// If exception is handled, this doesn't return; otherwise, it returns
// ExceptionContinueSearch.
//
// Note that this is called three ways:
//     From __CxxFrameHandler: primary usage, called to inspect whole function.
//         CatchDepth == 0, pMarkerRN == nullptr
//     From CatchGuardHandler: If an exception occurred within a catch, this is
//         called to check for try blocks within that catch only, and does not
//         handle unwinds.
//     From TranslatorGuardHandler: Called to handle the translation of a
//         non-C++ EH exception.  Context considered is that of parent.
EXCEPTION_DISPOSITION InternalCxxFrameHandler(
    EHExceptionRecord* pExcept,         // Information for this exception
    EHRegistrationNode* pRN,            // Dynamic information for this frame
    CONTEXT* pContext,                  // Context info
    DispatcherContext* pDC,             // Context within subject frame
    FuncInfo4* pFuncInfo,               // Static information for this frame
    int CatchDepth,                     // How deeply nested are we?
    EHRegistrationNode* pMarkerRN,      // Marker node for when checking inside
                                        //  catch block
    BOOLEAN recursive                   // Are we handling a translation?
);

////////////////////////////////////////////////////////////////////////////////
//
// FrameUnwindToState - Unwind this frame until specified state is reached.
//
// Returns:
//     No return value.
//
// Side Effects:
//     All objects on frame which go out of scope as a result of the unwind are
//       destructed.
//     Registration node is updated to reflect new state.
//
// Usage:
//      This function is called both to do full-frame unwind during the unwind
//      phase (targetState = -1), and to do partial unwinding when the current
//      frame has an appropriate catch.
void FrameUnwindToState(
    EHRegistrationNode* pRN,            // Registration node for subject
                                        //   function
    DispatcherContext* pDC,             // Context within subject frame
    FuncInfo4* pFuncInfo,               // Static information for subject
                                        //   function
    __ehstate_t targetState             // State to unwind to
);

__ehstate_t StateFromIp(
    FuncInfo4* pFuncInfo,
    DispatcherContext* pDC,
    uintptr_t           Ip
);

//
// This routine returns TRUE if we are executing from within a catch.  Otherwise, FALSE is returned.
//
bool ExecutionInCatch(
    DispatcherContext*  /*pDC*/,
    FuncInfo4* pFuncInfo
);

//
// Returns the establisher frame pointers. For catch handlers it is the parent's frame pointer.
//
EHRegistrationNode* GetEstablisherFrame
(
    EHRegistrationNode* pRN,
    DispatcherContext* pDC,
    FuncInfo4* pFuncInfo,
    EHRegistrationNode* pEstablisher
);

// The name of this function is rather misleading. This function won't really unwind
// to empty state. This function will unwind to lowest possible state for current block.
// Here is an example
//
// try {
//   // State = 1;
// } catch (...) {
//   // State when we enter catch is 2
//   // State inside catch is 3-5;
// }
// if __FrameUnwindToEmptyState is called for the main function, the target state will
// be -1 but if __FrameUnwindToEmptyState is called for catch block, the target state for
// __FrameUnwindToState will be 2 not -1. This way we are able to unwind the stack block
// by block not the whole function in single call.
void FrameUnwindToEmptyState(
    EHRegistrationNode* pRN,
    DispatcherContext* pDC,
    FuncInfo4* pFuncInfo
);

int TypeMatchHelper(
    typename HandlerType4* pCatch,   // Type of the 'catch' clause
    CatchableType* pCatchable,       // Type conversion under consideration
    ThrowInfo* pThrow                // General information about the thrown
                                     //   type.
);

////////////////////////////////////////////////////////////////////////////////
//
// __TypeMatch - Check if the catch type matches the given throw conversion.
//
// Returns:
//     TRUE if the catch can catch using this throw conversion, FALSE otherwise.
int TypeMatch(
    HandlerType4* pCatch,
    CatchableType* pCatchable,
    ThrowInfo* pThrow
);

//////////////////////////////////////////////////////////////////////////////////
// IsInExceptionSpec - Checks if the exception matches the exception specification
//                     list. Returns TRUE if it does, otherwise FALSE
//
BOOLEAN IsInExceptionSpec(
    EHExceptionRecord* pExcept,         // Information for this (logical) exception
    ESTypeList* pESTypeList             // Static information for subject frame
);

//////////////////////////////////////////////////////////////////////////////////
// Is_bad_exception_allowed - checks if std::bad_exception belongs to the list
//
BOOLEAN Is_bad_exception_allowed(ESTypeList* pESTypeList);

DWORD _FilterSetCurrentException(EXCEPTION_POINTERS* pointers, BOOLEAN fThrowNotAllowed);

////////////////////////////////////////////////////////////////////////////////
//
// __DestructExceptionObject - Call the destructor (if any) of the original
//   exception object.
//
// Returns: None.
//
// Side-effects:
//     Original exception object is destructed.
//
// Notes:
//     If destruction throws any exception, and we are destructing the exception
//       object as a result of a new exception, we give up.  If the destruction
extern "C" void __cdecl __DestructExceptionObject(
    EHExceptionRecord * pExcept,         // The original exception record
    BOOLEAN fThrowNotAllowed            // TRUE if destructor not allowed to
                                        //   throw
);

TryBlockMap4::IteratorPair GetRangeOfTrysToCheck(
    TryBlockMap4& tryBlockMap,
    __ehstate_t    curState,
    DispatcherContext* /*pDC*/,
    FuncInfo4* /*pFuncInfo*/,
    int            /*CatchDepth*/
);

// Call the SEH to EH translator.
int SehTransFilter(
    EXCEPTION_POINTERS* ExPtrs,
    EHExceptionRecord* pExcept,
    EHRegistrationNode* pRN,
    CONTEXT* pContext,
    DispatcherContext* pDC,
    FuncInfo4* pFuncInfo,
    BOOL* pResult
);

BOOL _CallSETranslator(
    EHExceptionRecord* pExcept,    // The exception to be translated
    EHRegistrationNode* pRN,        // Dynamic info of function with catch
    CONTEXT* pContext,   // Context info
    DispatcherContext* pDC,        // More dynamic info of function with catch (ignored)
    FuncInfo4* pFuncInfo,  // Static info of function with catch
    ULONG                CatchDepth,  // How deeply nested in catch blocks are we?
    EHRegistrationNode* pMarkerRN   // Marker for parent context
);

////////////////////////////////////////////////////////////////////////////////
//
// FindHandlerForForeignException - We've got an exception which wasn't ours.
//     Try to translate it into C++ EH, and also check for match with ellipsis.
//
// Description:
//     If an SE-to-EH translator has been installed, call it.  The translator
//     must throw the appropriate typed exception or return.  If the translator
//     throws, we invoke FindHandler again as the exception filter.
//
// Returns:
//     Returns if exception was not fully handled.
//     No return value.
//
// Assumptions:
//     Only called if there are handlers in this function.

void FindHandlerForForeignException(
    EHExceptionRecord* pExcept,         // Information for this (logical)
                                        //   exception
    EHRegistrationNode* pRN,            // Dynamic information for subject frame
    CONTEXT* pContext,                  // Context info
    DispatcherContext* pDC,             // Context within subject frame
    FuncInfo4* pFuncInfo,               // Static information for subject frame
    __ehstate_t curState,               // Current state
    int catchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode* pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
);

/*
* Here We find what is the actual State of current function. The way we
* do this is first get State from ControlPc.
*
* Remember we have __GetUnwindTryBlock to remember the last State for which
* Exception was handled and __GetCurrentState for retriving the current
* state of the function. Please Note that __GetCurrentState is used
* primarily for unwinding purpose.
*
* Also remember that all the catch blocks act as funclets. This means that
* ControlPc for all the catch blocks are different from ControlPc of parent
* catch block or function.
*
* take a look at this example
* try {
*   // STATE1 = 1
*   try {
*     // STATE2
*     // THROW
*   } catch (...) { // CatchB1
*     // STATE3
*     // RETHROW OR NEW THROW
*   }
* } catch (...) { // CatchB2
* }
*
* If we have an exception comming from STATE3, the FindHandler will be
* called for CatchB1, at this point we do the test which State is our
* real state, curState from ControlPc or state from __GetUnwindTryBlock.
* Since curState from ControlPc is greater, we know that real State is
* curState from ControlPc and thus we update the UnwindTryBlockState.
*
* On further examination, we found out that there is no handler within
* this catch block, we return without handling the exception. For more
* info on how we determine if we have handler, have a look at
* GetRangeOfTrysToCheck.
*
* Now FindHandler will again be called for parent function. Here again
* we test which is real State, state from ControlPc or State from
* __GetUnwindTryBlock. This time state from __GetUnwindTryBlock is correct.
*
* Also look at code in __CxxCallCatchBlock, you will se that as soon as we get
* out of last catch block, we reset __GetUnwindTryBlock state to -1.
*/
inline __ehstate_t GetHandlerSearchState(
    EHRegistrationNode* /*pRN*/,
    DispatcherContext* pDC,
    FuncInfo4* pFuncInfo
);

void FindHandler(
    EHExceptionRecord* pExcept,         // Information for this (logical)
                                        //   exception
    EHRegistrationNode* pRN,            // Dynamic information for subject frame
    CONTEXT* pContext,                  // Context info
    DispatcherContext* pDC,             // Context within subject frame
    FuncInfo4* pFuncInfo,               // Static information for subject frame
    BOOLEAN recursive,                  // TRUE if we're handling the
                                        //   translation
    int CatchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode* pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
);

////////////////////////////////////////////////////////////////////////////////
//
// __AdjustPointer - Adjust the pointer to the exception object to a pointer to a
//   base instance.
//
// Output:
//     The address point of the base.
//
// Side-effects:
//     NONE.
void* __AdjustPointer(
    void* pThis,                        // Address point of exception object
    const PMD& pmd                      // Generalized pointer-to-member
                                        //   descriptor
);

////////////////////////////////////////////////////////////////////////////////
//
// BuildCatchObjectHelper - Copy or construct the catch object from the object thrown.
//
// Returns:
//     0 if nothing to be done for constructing object from caller side
//     1 if single parameter constructor is to be called.
//     2 if two parameter constructor is to be called.
//
// Side-effects:
//     A buffer in the subject function's frame is initialized.
//
// Open issues:
//     What happens if the constructor throws?  (or faults?)
int BuildCatchObjectHelperInternal(
    EHExceptionRecord* pExcept,      // Original exception thrown
    void* pRN,                       // This is a pointer to the object
                                     // that we want to build while doing
                                     // COM+ eh. If we are in our own eh,
                                     // then this is a Registration node of
                                     // catching function
    HandlerType4* pCatch,            // The catch clause that got it
    CatchableType* pConv             // The conversion to use
);

int BuildCatchObjectHelper(
    EHExceptionRecord* pExcept,
    void* pRN,
    HandlerType4* pCatch,
    CatchableType* pConv
);

////////////////////////////////////////////////////////////////////////////////
//
// BuildCatchObjectInternal - Copy or construct the catch object from the object thrown.
//
// Returns:
//     nothing.
//
// Side-effects:
//     A buffer in the subject function's frame is initialized.
//
// Open issues:
//     What happens if the constructor throws?  (or faults?)
void BuildCatchObjectInternal(
    EHExceptionRecord* pExcept,         // Original exception thrown
    void* pRN,                          // This is a pointer to the object
                                        // that we want to build while doing
                                        // COM+ eh. If we are in our own eh,
                                        // then this is a Registration node of
                                        // catching function
    HandlerType4* pCatch,    // The catch clause that got it
    CatchableType* pConv                // The conversion to use
);

/////////////////////////////////////////////////////////////////////////////
//
// _CreateFrameInfo - Save the frame information for this scope just before
//  calling the catch block.  Put it at the head of the linked list.  For
//  x86, all we need to save is the pointer to the exception object, so we
//  can determine when that object is no longer used by any nested catch
//  handler and can thus be destroyed on exiting from a catch.
//
// Returns:
//      Pointer to the frame info (the first input argument).
//
FRAMEINFO* __cdecl _CreateFrameInfo(
    FRAMEINFO* pFrameInfo,
    PVOID       pExceptionObject
);

////////////////////////////////////////////////////////////////////////////////
//
// ExFilterRethrow - Exception filter for re-throw exceptions.
//
// Returns:
//     EXCEPTION_EXECUTE_HANDLER - exception was a re-throw
//     EXCEPTION_CONTINUE_SEARCH - anything else
//
// Side-effects: sets rethrow = TRUE if exception objects of the two Exception matches

//__declspec(guard(ignore)) 
int ExFilterRethrow(
    EXCEPTION_POINTERS* pExPtrs,
    EHExceptionRecord* pOldExcept,
    int* rethrow
);

// Wraps around ExFilterRethrow to set _CatchStateInParent if this isn't a rethrow.
//__declspec(guard(ignore)) 
int ExFilterRethrowFH4(
    EXCEPTION_POINTERS* pExPtrs,
    EHExceptionRecord* pOldExcept,
    __ehstate_t HandlerSearchState,
    int* rethrow
);

__declspec(noreturn)
void __RethrowException(EHExceptionRecord* pThisException);

/////////////////////////////////////////////////////////////////////////////
//
// _FindAndUnlinkFrame - Pop the frame information for this scope that was
//  pushed by _CreateFrameInfo.  This should be the first frame in the list,
//  but the code will look for a nested frame and pop all frames, just in
//  case.
//
extern "C" void __cdecl _FindAndUnlinkFrame(
    FRAMEINFO * pFrameInfo
);

/////////////////////////////////////////////////////////////////////////////
//
// _IsExceptionObjectToBeDestroyed - Determine if an exception object is still
//  in use by a more deeply nested catch frame, or if it unused and should be
//  destroyed on exiting from the current catch block.
//
// Returns:
//      TRUE if exception object not found and should be destroyed.
//
extern "C" BOOL __cdecl _IsExceptionObjectToBeDestroyed(
    PVOID pExceptionObject
);

void* CxxCallCatchBlock(
    EXCEPTION_RECORD* pExcept
);

void UnwindNestedFrames(
    EHRegistrationNode* pFrame,            // Unwind up to (but not including) this frame
    EHExceptionRecord* pExcept,           // The exception that initiated this unwind
    CONTEXT* pContext,          // Context info for current exception
    EHRegistrationNode* pEstablisher,
    void* Handler,
    FuncInfo4*          /*pFuncInfo*/,
    __ehstate_t         TargetUnwindState,
    __ehstate_t         CatchState,        // State outside of current try but inside of any enclosing trys
    HandlerType4* pCatch,
    DispatcherContext* pDC,
    BOOLEAN             recursive
);

////////////////////////////////////////////////////////////////////////////////
//
// CatchIt - A handler has been found for the thrown type.  Do the work to
//   transfer control.
//
// Description:
//     Builds the catch object
//     Unwinds the stack to the point of the try
//     Calls the address of the handler (funclet) with the frame set up for that
//       function but without resetting the stack.
//     Handler funclet returns address to continue execution, or nullptr if the
//       handler re-threw ("throw;" lexically in handler)
//     If the handler throws an EH exception whose exception info is nullptr, then
//       it's a re-throw from a dynamicly enclosed scope.
//
// It is an open question whether the catch object is built before or after the local unwind.
//
// Returns:
//     No return value.  Returns iff handler re-throws.
void CatchIt(
    EHExceptionRecord* pExcept,           // The exception thrown
    EHRegistrationNode* pRN,              // Dynamic info of function with catch
    CONTEXT* pContext,                    // Context info
    DispatcherContext* pDC,               // Context within subject frame
    FuncInfo4* pFuncInfo,                 // Static info of function with catch
    HandlerType4* pCatch,                 // The catch clause selected
    CatchableType* pConv,                 // The rules for making the conversion
    TryBlockMapEntry4* pEntry,            // Description of the try block
    int CatchDepth,                       // How many catches are we nested in?
    EHRegistrationNode* pMarkerRN,        // Special node if nested in catch
    BOOLEAN IsRethrow,                    // Is this a rethrow ?
    BOOLEAN recursive                     // Is this from a translated exception ?
);

////////////////////////////////////////////////////////////////////////////////
//
// __FrameUnwindFilter - Allows possibility of continuing through SEH during
//   unwind.
//
extern "C" int __cdecl __FrameUnwindFilter(
    EXCEPTION_POINTERS * pExPtrs
);