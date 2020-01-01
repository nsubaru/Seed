// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Base.h"
#include "Seed.Core.Exceptions.h"

#include "Seed.Core.Exceptions.Internals.h"
#include "Seed.Core.Exceptions.Internals.SEH.h"

#include "Seed.Core.CoreLog.Managed.h"

#include "typeinfo"

#include "Seed.Core.Threading.Impl.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

#pragma warning(push)
//This internal implementation file which can use internal API functions
#pragma warning(disable: 4996)
//RtlUnwingEx for unwind we return NULL as we don't know what type and value expect parent function
//and after unwind we jump to catch block as it's exception handling
#pragma warning(disable: 6387)
//Using old(C) style enum
#pragma warning(disable: 26812)

#undef throw

__forceinline ExceptionInfoT* GetThreadExceptionInfo() {
    return &Seed::Core::Threading::GetTLS()->ExceptionInfo;
}

#define ThreadExceptionInfo GetThreadExceptionInfo()

extern void* CxxCallCatchBlock(
    EXCEPTION_RECORD* pExcept
);

using namespace Seed;

extern "C" void* __cdecl _CallSettingFrame(
    void* handler,
    EHRegistrationNode * pEstablisher,
    ULONG               NLG_CODE);

[[noreturn]] InternalApiCall void EmergencyShutdown(const ConstAString& File, const ConstAString& Function, UInt64 Line, const ConstString& Reason) {
    using SafeString = BaseString<Memory::ExceptionHeap>;
    Exception::ExeptionLocation location{ File,Function,Line };
    toCoreLog(SafeString(L"Аварійне завершення ядра. Причина ") + SafeString(Reason) + SafeString(L" Розташування виклику: ") + location.toString());
    NtTerminateProcess(NtCurrentProcess(), STATUS_INTERNAL_ERROR);
}

template<typename T>
__forceinline T* GetByOffset(const SizeT Base, const SizeT Offset) {
    return reinterpret_cast<T*>(Base + Offset);
}

int StateFromIp(FuncInfo* pFuncInfo, DISPATCHER_CONTEXT* pDC) {
    unsigned int        index;          //  loop control variable

    UInt64 Ip = pDC->ControlPc;
    //
    // If this context came from an unwind to a call, then the ControlPc points
    // to a return address, which could put us at the start of a neighboring
    // scope. To correct for this, back the PC up by the minimum instruction
    // size to ensure we are in the same scope as the original call opcode.
    //

    if (reinterpret_cast<DispatcherContextAMD64*>(pDC)->ControlPcIsUnwound) {
        Ip -= 4;
    }

    auto* pIPtoStateMap = GetByOffset<IptoStateMapEntry>(pDC->ImageBase, pFuncInfo->dispIPtoStateMap);
    for (index = 0; index < pFuncInfo->nIPMapEntries; index++) {
        if (Ip < (UInt64)(pDC->ImageBase + pIPtoStateMap[index].Ip)) {
            break;
        }
    }

    if (index == 0) {
        // We are at the first entry, could be an error

        return -1;
    }

    // We over-shot one iteration; return state from the previous slot

    return GetByOffset<IptoStateMapEntry>(pDC->ImageBase, pFuncInfo->dispIPtoStateMap)[index - 1].State;
}

__forceinline int StateFromControlPc(FuncInfo* pFuncInfo, DISPATCHER_CONTEXT* pDC) {
    return StateFromIp(pFuncInfo, pDC);
}

__forceinline void SetState(EHRegistrationNode* pRN, FuncInfo* pFuncInfo, __ehstate_t newState) {
    *(DWORD*)(pFuncInfo->dispUwindHelp + (SizeT)*pRN) = newState;
}

__ehstate_t GetCurrentState(EHRegistrationNode* pRN, DISPATCHER_CONTEXT* pDC, FuncInfo* pFuncInfo) {
    if (*(__ehstate_t*)(pFuncInfo->dispUwindHelp + (SizeT)*pRN) == -2)
        return StateFromControlPc(pFuncInfo, pDC);
    else
        return *(__ehstate_t*)(pFuncInfo->dispUwindHelp + (SizeT)*pRN);
}

__int64 __fastcall FrameUnwindFilter(PEXCEPTION_POINTERS pExPtrs)
{
    unsigned int v3;

    v3 = pExPtrs->ExceptionRecord->ExceptionCode;
    if (v3 == -532462766 || v3 == -532459699)
    {
        if (ThreadExceptionInfo->_ProcessingThrow > 0)
        {
            --ThreadExceptionInfo->_ProcessingThrow;
        }
    }
    else
    {
        if (v3 == -529697949)
        {
            ThreadExceptionInfo->_ProcessingThrow = 0;
            EmergencyShutdown(ExceptionPosition, L"Помилка розкрутки стеку");
        }
    }
    return 0;
}

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
    DISPATCHER_CONTEXT* pDC,             // Context within subject frame
    FuncInfo* pFuncInfo,                // Static information for subject
                                        //   function
    __ehstate_t targetState             // State to unwind to
)
{
    typedef void(__cdecl* action)(void);

    UInt64 UnwindImageBase = ThreadExceptionInfo->_ImageBase;
    __ehstate_t curState = GetCurrentState(pRN, pDC, pFuncInfo);

    __try {
        while (curState != -1 && curState > targetState)
        {
            __ehstate_t nxtState = GetByOffset<UnwindMapEntry>(pFuncInfo->dispUnwindMap, UnwindImageBase)[curState].toState;

            __try {
                //Find unwind action for function
                auto pAction = (action)GetByOffset<action>(
                    GetByOffset<UnwindMapEntry>(pFuncInfo->dispUnwindMap, UnwindImageBase)[curState].action,
                    pDC->ImageBase);
                //If not null
                if ((SizeT)pAction != UnwindImageBase) {
                    // Before calling unwind action, adjust state as if it were
                    // already completed:
                    SetState(pRN, pFuncInfo, nxtState);

                    //Call action
                    _CallSettingFrame(pAction,
                        pRN,
                        0x103);

                    ThreadExceptionInfo->_ImageBase = UnwindImageBase;
                }
            }
            __except (FrameUnwindFilter(GetExceptionInformation())) {
                ; // Deliberately do nothing
            }

            curState = nxtState;
        }
    }
    __finally {
        if (ThreadExceptionInfo->_ProcessingThrow > 0) {
            ThreadExceptionInfo->_ProcessingThrow--;
        }
    }
}

//
// Returns the establisher frame pointers. For catch handlers it is the parent's frame pointer.
//
EHRegistrationNode* GetEstablisherFrame(
    EHRegistrationNode* pRN,
    DISPATCHER_CONTEXT* pDC,
    FuncInfo* pFuncInfo,
    EHRegistrationNode* pEstablisher
)
{
    TryBlockMapEntry* pEntry;
    HandlerType* pHandler;
    ULONG_PTR HandlerAdd;
    unsigned num_of_try_blocks = pFuncInfo->nTryBlocks;
    unsigned index, i;
    __ehstate_t curState;

    curState = StateFromControlPc(pFuncInfo, pDC);
    *pEstablisher = *pRN;
    for (index = num_of_try_blocks; index > 0; index--) {
        pEntry = &(GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap)[index - 1]);
        if (curState > pEntry->tryHigh&& curState <= pEntry->catchHigh) {
            // Get target function begin address
            HandlerAdd = RtlLookupFunctionEntry(pDC->ControlPc, (PDWORD64)ThreadExceptionInfo->_ImageBase, nullptr)->BeginAddress;
            // Get catch handler address.
            pHandler = GetByOffset<HandlerType>(pDC->ImageBase, pEntry->dispHandlerArray);
            for (i = 0;
                i < (unsigned)pEntry->nCatches &&
                static_cast<ULONG_PTR>(pHandler[i].dispOfHandler) != HandlerAdd
                ; i++);
            if (i < (unsigned)pEntry->nCatches) {
                *pEstablisher = GetByOffset<EHRegistrationNode>((SizeT)*pRN, pHandler[i].dispFrame);
                break;
            }
        }
    }
    return pEstablisher;
}

// This function returns the try block for the given state if the state is in a
// catch; otherwise, nullptr is returned.
TryBlockMapEntry* CatchTryBlock(
    FuncInfo* pFuncInfo,
    __ehstate_t         curState
) {
    TryBlockMapEntry* pEntry;
    unsigned num_of_try_blocks = pFuncInfo->nTryBlocks;
    unsigned index;

    for (index = num_of_try_blocks; index > 0; index--) {
        pEntry = &(GetByOffset<TryBlockMapEntry>(ThreadExceptionInfo->_ImageBase, pFuncInfo->dispTryBlockMap)[index - 1]);
        if (curState > pEntry->tryHigh&& curState <= pEntry->catchHigh) {
            return pEntry;
        }
    }

    return nullptr;
}

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
    DISPATCHER_CONTEXT* pDC,
    FuncInfo* pFuncInfo
)
{
    __ehstate_t         stateFromControlPC;
    TryBlockMapEntry* pEntry;
    EHRegistrationNode  EstablisherFramePointers, * pEstablisher;

    pEstablisher = GetEstablisherFrame(pRN,
        pDC,
        pFuncInfo,
        &EstablisherFramePointers);

    stateFromControlPC = StateFromControlPc(pFuncInfo, pDC);
    pEntry = CatchTryBlock(pFuncInfo, stateFromControlPC);

    FrameUnwindToState(pEstablisher, pDC, pFuncInfo,
        pEntry == nullptr ? EH_EMPTY_STATE : pEntry->tryHigh);
}

__forceinline ESTypeList* getESTypes(FuncInfo* pFuncInfo)
{
    if (pFuncInfo->dispESTypeList == 0)
        return nullptr;
    return GetByOffset<ESTypeList>(ThreadExceptionInfo->_ImageBase, pFuncInfo->dispESTypeList);
}

__forceinline bool isNoExcept(FuncInfo* pFuncInfo) {
    return (pFuncInfo->EHFlags & 4) != 0;
}

__forceinline __ehstate_t GetUnwindTryBlock(
    EHRegistrationNode* pRN,
    DISPATCHER_CONTEXT* pDC,
    FuncInfo* pFuncInfo
)
{
    EHRegistrationNode pEstablisher;

    return *(unsigned int*)(*(Int64*)GetEstablisherFrame(pRN, pDC, pFuncInfo, &pEstablisher)
        + pFuncInfo->dispUwindHelp
        + 4);
}

__forceinline void SetUnwindTryBlock(EHRegistrationNode* pRN, DISPATCHER_CONTEXT* pDC, FuncInfo* pFuncInfo, __ehstate_t curState) {
    EHRegistrationNode pEstablisher = *GetEstablisherFrame(pRN, pDC, pFuncInfo, &pEstablisher);
    if (curState > (Int64)* (DWORD*)((SizeT)pEstablisher + pFuncInfo->dispUwindHelp + 4))
        *(DWORD*)((SizeT)pEstablisher + pFuncInfo->dispUwindHelp + 4) = curState;
}

__forceinline __ehstate_t GetHandlerSearchState(
    EHRegistrationNode* pRN,
    DISPATCHER_CONTEXT* pDC,
    FuncInfo* pFuncInfo
)
{
    __ehstate_t curState = StateFromControlPc(pFuncInfo, pDC);
    EHRegistrationNode EstablisherFrame;
    GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFrame);
    if (curState > GetUnwindTryBlock(pRN, pDC, pFuncInfo)) {
        SetState(&EstablisherFrame, pFuncInfo, curState);
        SetUnwindTryBlock(pRN, pDC, pFuncInfo, /*curTry*/ curState);
    }
    else {
        curState = GetUnwindTryBlock(pRN, pDC, pFuncInfo);
    }

    return curState;
}

int TypeMatch(
    HandlerType* pCatch,                  // Type of the 'catch' clause
    CatchableType* pCatchable,  // Type conversion under consideration
    ThrowInfo* pThrow, // General information about the thrown type.
    size_t ImageBase
) {
    // First, check for match with ellipsis:
    auto typeInfo = GetByOffset<TypeDescriptor>(ImageBase, pCatch->dispType);

    if (typeInfo != nullptr && typeInfo->name[0] == '\0') {
        return TRUE;
    }

    if (pCatch->adjectives & HT_IsBadAllocCompat && pCatchable->properties & CT_IsStdBadAlloc)
        return TRUE;

    // Not ellipsis; the basic types match if it's the same record *or* the
    // names are identical.
    auto typeCatch = GetByOffset<TypeDescriptor>(ImageBase, pCatch->dispType);
    auto typeCatchable = GetByOffset<TypeDescriptor>(ImageBase, pCatchable->pType);
    toCoreLog(L"Matching catch closure. TypeCatch: \"" + String(typeCatch->name) + L"\" TypeCatchable: \"" + String(typeCatchable->name) + L"\"");
    if (typeCatch != typeCatchable && !CStr::Compare(typeCatch->name, typeCatchable->name))
        return FALSE;

    // Basic types match.  The actual conversion is valid if:
    //   caught by ref if ref required *and*
    //   the qualifiers are compatible *and*
    //   the alignments match *and*
    //   the volatility matches
    return (!(pCatchable->properties & CT_ByReferenceOnly) || pCatch->adjectives & HT_IsReference)
        && (!(pThrow->attributes & TI_IsConst) || pCatch->adjectives & HT_IsConst)
        && (!(pThrow->attributes & TI_IsUnaligned) || pCatch->adjectives & HT_IsUnaligned)
        && (!(pThrow->attributes & TI_IsVolatile) || pCatch->adjectives & HT_IsVolatile);
}

//////////////////////////////////////////////////////////////////////////////////
// IsInExceptionSpec - Checks if the exception matches the exception specification
//                     list. Returns TRUE if it does, otherwise FALSE
//
static BOOLEAN IsInExceptionSpec(
    EHExceptionRecord* pExcept,         // Information for this (logical)
                                        //   exception
    ESTypeList* pESTypeList             // Static information for subject frame
)
{
    //DASSERT(pESTypeList != nullptr);
    if (!pESTypeList)
    {
        EmergencyShutdown(ExceptionPosition, L"Покажчик на дані про тип помилки в блоці catch не може бути nullptr");
    }

    BOOLEAN bFoundMatchingTypeInES = FALSE;

    __int32 const* ppCatchable;
    CatchableType* pCatchable;
    int catchables;

    CatchableTypeArray* pCatchableTypeArray;

    // for every type in the exception spec...
    for (int i = 0; i < pESTypeList->nCount; i++)
    {
        // for all types that thrown object can be converted to...
        pCatchableTypeArray = GetByOffset<CatchableTypeArray>(
            (SizeT)pExcept->params.pThrowImageBase,
            pExcept->params.pThrowInfo->pCatchableTypeArray);

        ppCatchable = pCatchableTypeArray->arrayOfCatchableTypes;
        HandlerType* catchs = GetByOffset<HandlerType>((SizeT)pExcept->params.pThrowImageBase, pESTypeList->dispTypeArray);

        for (catchables = pCatchableTypeArray->nCatchableTypes;
            catchables > 0; catchables--, ppCatchable++) { //-V2528
            pCatchable = GetByOffset<CatchableType>((SizeT)pExcept->params.pThrowImageBase, *ppCatchable);

            if (TypeMatch(&catchs[i], pCatchable, (ThrowInfo*)pExcept->params.pThrowImageBase, (SizeT)pExcept->params.pThrowImageBase))
            {
                bFoundMatchingTypeInES = TRUE;
                break;
            }
        }
    }

    return bFoundMatchingTypeInES;
}

//////////////////////////////////////////////////////////////////////////////////
// Is_bad_exception_allowed - checks if BadException belongs to the list
//
BOOLEAN Is_bad_exception_allowed(ESTypeList* pESTypeList, size_t ImageBase)
{
    for (int i = 0; i < pESTypeList->nCount; i++)
    {
        if (*GetByOffset<type_info>(ImageBase,
            GetByOffset<HandlerType>(ImageBase, pESTypeList->dispTypeArray)->dispType) == typeid(BadException))
            return TRUE;
    }

    return FALSE;
}

__forceinline void _CallMemberFunction0(void* const pthis, void* const pmfn)
{
    ((void(__fastcall*)(void* const))pmfn)(pthis);
}

void __cdecl __DestructExceptionObject(
    EHExceptionRecord* pExcept,
    BOOLEAN fThrowNotAllowed,
    Int64 arg
) {
    if (pExcept
        && pExcept->ExceptionCode == EH_EXCEPTION_NUMBER
        && pExcept->NumberParameters == 4
        && (pExcept->params.magicNumber == EH_MAGIC_NUMBER1
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER2
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER3)
        && pExcept->params.pThrowInfo)
    {
        if (pExcept->params.pThrowInfo->pmfnUnwind)
        {
            _CallMemberFunction0(
                pExcept->params.pExceptionObject,
                (char*)pExcept->params.pThrowImageBase + pExcept->params.pThrowInfo->pmfnUnwind);
        }
        else if (pExcept->params.pThrowInfo->attributes & 0x10)
        {
            auto v3 = *(Int64*)pExcept->params.pExceptionObject;
            if (v3)
                (*(void(__fastcall**)(__int64, __int64, __int64))(*(Int64*)v3 + 16i64))(v3, fThrowNotAllowed, arg);
        }
    }
}

struct __range {
    unsigned int start;
    unsigned int end;
}
GetRangeOfTrysToCheck(
    TryBlockMap& tryBlockMap,
    __ehstate_t    curState,
    int            /*CatchDepth*/
)
{
    TryBlockMapEntry* pEntry, * pCurCatchEntry = nullptr;
    FuncInfo* pFuncInfo = tryBlockMap.pFuncInfo;
    DISPATCHER_CONTEXT* pDC = tryBlockMap.pDC;
    unsigned num_of_try_blocks = pFuncInfo->nTryBlocks;
    unsigned int index;
    __ehstate_t ipState = StateFromControlPc(pFuncInfo, pDC);

    //DASSERT(num_of_try_blocks > 0);

    unsigned start = static_cast<unsigned>(-1);
    unsigned end = start;
    pEntry = GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap);
    for (index = num_of_try_blocks; index > 0; index--) {
        if (ipState > pEntry[index].tryHigh&& ipState <= pEntry[index].catchHigh) {
            break;
        }
    }
    if (index) {
        pCurCatchEntry = &(GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap)[index - 1]);
    }
    pEntry = GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap);
    for (index = 0; index < num_of_try_blocks; index++) {
        // if in catch block, check for try-catch only in current block
        if (pCurCatchEntry) {
            if (pEntry[index].tryLow <= pCurCatchEntry->tryHigh ||
                pEntry[index].tryHigh > pCurCatchEntry->catchHigh)
                continue;
        }
        if (curState >= pEntry[index].tryLow && curState <= pEntry[index].tryHigh) {
            if (start == -1) {
                start = index;
            }
            end = index;
        }
    }

    // change to be (start, end]
    ++end;
    if (start == -1) {
        start = 0;
        end = 0;
    }

    return { start,end };
}

__declspec(noreturn)
void UnwindNestedFrames( //-V2537
    EHRegistrationNode* pFrame,            // Unwind up to (but not including) this frame
    EHExceptionRecord* pExcept,           // The exception that initiated this unwind
    CONTEXT* pContext,          // Context info for current exception
    EHRegistrationNode* pEstablisher,
    void* Handler,
    FuncInfo* pFuncInfo,
    __ehstate_t         TargetUnwindState,
    __ehstate_t         CatchState,
    HandlerType* pCatch,
    DISPATCHER_CONTEXT* pDC,
    BOOLEAN             recursive
)
{
    static const EXCEPTION_RECORD ExceptionTemplate = // A generic exception record
    {
        (DWORD)STATUS_UNWIND_CONSOLIDATE,         // STATUS_UNWIND_CONSOLIDATE
        EXCEPTION_NONCONTINUABLE,          // Exception flags (we don't do resume)
        nullptr,                           // Additional record (none)
        nullptr,                           // Address of exception (OS fills in)
        15,                                // Number of parameters
        {   EH_MAGIC_NUMBER1,              // Our version control magic number
            0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0
        }                                  // pThrowInfo
    };

    volatile auto cp = pDC;

    CONTEXT Context = { 0 };

    PCONTEXT UnwindContext = &Context;

    EXCEPTION_RECORD ExceptionRecord = ExceptionTemplate;
    ExceptionRecord.ExceptionInformation[0] = (ULONG_PTR)CxxCallCatchBlock;
    // Address of call back function
    ExceptionRecord.ExceptionInformation[1] = (ULONG_PTR)pEstablisher;
    // Used by callback function
    ExceptionRecord.ExceptionInformation[2] = (ULONG_PTR)Handler;
    // Used by callback function to call catch block
    ExceptionRecord.ExceptionInformation[3] = (ULONG_PTR)TargetUnwindState;
    // Used by CxxFrameHandler to unwind to target_state
    ExceptionRecord.ExceptionInformation[4] = (ULONG_PTR)pContext;
    // used to set pCurrentExContext in callback function
    ExceptionRecord.ExceptionInformation[5] = (ULONG_PTR)pFuncInfo;
    // Used in callback function to set state on stack to -2
    ExceptionRecord.ExceptionInformation[6] = (ULONG_PTR)pExcept;
    // Used for passing current Exception
    ExceptionRecord.ExceptionInformation[7] = (ULONG_PTR)recursive;
    // Used for translated Exceptions
    ExceptionRecord.ExceptionInformation[8] = EH_MAGIC_NUMBER1;
    // Used in __InternalCxxFrameHandler to detected if it's being
    // called from _UnwindNestedFrames.

    RtlUnwindEx((void*)*pFrame,
        (void*)pDC->ControlPc,    // Address where control left function
        &ExceptionRecord,
        NULL,
        UnwindContext,
        (PUNWIND_HISTORY_TABLE)cp->HistoryTable);
}

//
// This routine returns TRUE if we are executing from within a catch.  Otherwise, FALSE is returned.
//
__forceinline bool ExecutionInCatch(
    DISPATCHER_CONTEXT* pDC,
    FuncInfo* pFuncInfo
)
{
    __ehstate_t curState = StateFromControlPc(pFuncInfo, pDC);
    return CatchTryBlock(pFuncInfo, curState) ? TRUE : FALSE;
}

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
__forceinline void* __AdjustPointer(
    void* pThis,                        // Address point of exception object
    const PMD& pmd                      // Generalized pointer-to-member
                                        //   descriptor
) {
    char* pRet = (char*)pThis + pmd.mdisp;

    if (pmd.pdisp >= 0) {
        pRet += *(__int32*)((char*) * (ptrdiff_t*)((char*)pThis + pmd.pdisp)
#if defined(_WIN64)
            +(unsigned __int64)pmd.vdisp
#else
            + pmd.vdisp
#endif
            );
        pRet += pmd.pdisp;
    }

    return pRet;
}

__declspec(noreturn, noinline)
void throwInternalError(const Seed::ConstAString& File, const Seed::ConstAString& FunctName, SizeT Line) {
    throw InternalError(File, FunctName, Line);
}

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
    EHExceptionRecord* pExcept, // Original exception thrown
    void* pRN,                  // This is a pointer to the object
                                // that we want to build while doing
                                // COM+ eh. If we are in our own eh,
                                // then this is a Registration node of
                                // catching function
    HandlerType* pCatch,        // The catch clause that got it
    CatchableType* pConv        // The conversion to use
)
{
    int retval = 0;

    // If the catch is by ellipsis, then there is no object to construct.
    // If the catch is by type(No Catch Object), then leave too!
    auto typeData = GetByOffset<TypeDescriptor>((SizeT)pExcept->params.pThrowImageBase, pCatch->dispType);
    if (typeData == nullptr || typeData->name[0] == '\0')
        if (!(pCatch->dispCatchObj) && pCatch->adjectives & HT_IsComplusEh)
            return 0;

    void** pCatchBuffer;
    if (pCatch->adjectives & HT_IsComplusEh)
    {
        pCatchBuffer = (void**)pRN;
    }
    else
    {
        pCatchBuffer = GetByOffset<void*>(*(SizeT*)pRN, pCatch->dispCatchObj);
    }
    __try {
        if (pCatch->adjectives & HT_IsReference) {
            // The catch is of form 'reference to T'.  At the throw point we
            // treat both 'T' and 'reference to T' the same, i.e.
            // pExceptionObject is a (machine) pointer to T.  Adjust as
            // required.
            if (pExcept != nullptr && pCatchBuffer != nullptr) {
                *pCatchBuffer = pExcept->params.pExceptionObject;
                *pCatchBuffer = __AdjustPointer(*pCatchBuffer,
                    pConv->thisDisplacement);
            }
            else {
                throwInternalError(ExceptionPosition);
            }
        }
        else {
            __DestructExceptionObject(pExcept, TRUE, 1);
            throwInternalError(ExceptionPosition);
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        __DestructExceptionObject(pExcept, TRUE, 1);   // destroy the original object
        throwInternalError(ExceptionPosition);
    }

    return retval;
}

__forceinline void _CallMemberFunction1(void* const pthis, void* const pmfn, void* const pthat)
{
    ((void(__vectorcall*)(void* const, void* const))pmfn)(pthis, pthat);
}

__forceinline void  _CallMemberFunction2(void* const pthis, void* const pmfn, void* const pthat, const int val2)
{
    ((void(__vectorcall*)(void* const, void* const, UInt64))pmfn)(pthis, pthat, (unsigned int)val2);
}

inline void BuildCatchObject(
    EHExceptionRecord* pExcept,
    void* pRN,
    HandlerType* pCatch,
    CatchableType* pConv
)
{
    void** pCatchBuffer;

    pCatchBuffer = GetByOffset<void*>((SizeT)pRN, pCatch->dispCatchObj);

    BuildCatchObjectHelperInternal(pExcept, pRN, pCatch, pConv);
}

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
void __cdecl CatchIt( //-V2537
    DISPATCHER_CONTEXT* pDC,               // Context within subject frame
    EHExceptionRecord* pExcept,           // The exception thrown
    EHRegistrationNode* pRN,              // Dynamic info of function with catch
    CONTEXT* pContext,                    // Context info
    FuncInfo* pFuncInfo,      // Static info of function with catch
    HandlerType* pCatch,      // The catch clause selected
    CatchableType* pConv,                 // The rules for making the conversion
    TryBlockMapEntry* pEntry, // Description of the try block
    int CatchDepth,                       // How many catches are we nested in?
    EHRegistrationNode* pMarkerRN,        // Special node if nested in catch
    BOOLEAN IsRethrow,                     // Is this a rethrow ?
    BOOLEAN recursive
) {
    // These parameters are not used in some compilations

//	auto cp = pEntry;
    typedef void(*pCatchT)(...);
    //	volatile auto pCatchC = pCatch;
    volatile pCatchT pCatchF = (pCatchT)GetByOffset<pCatchT>(ThreadExceptionInfo->_ImageBase, pCatch->dispOfHandler);
    //	volatile TypeDescriptor * CatchType = GetByOffset<TypeDescriptor>(ThreadExceptionInfo->_ImageBase, pCatch->dispType);
    //	volatile auto cppDC = pDC;

    EHRegistrationNode EstablisherFramePointers;
    EHRegistrationNode* pEstablisher = GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);

    // Copy the thrown object into a buffer in the handler's stack frame,
    // unless the catch was by elipsis (no conversion) OR the catch was by
    // type without an actual 'catch object'.

    if (pConv != nullptr) {
        BuildCatchObject(pExcept, pEstablisher, pCatch, pConv);
    }

    // Unwind stack objects to the entry of the try that caught this exception.

    // This call will never return. This call will end up calling CxxCallCatchBlock
    // through RtlUnwind (STATUS_CONSULIDATE_FRAMES) mechanism.
    UnwindNestedFrames(
        pRN,
        pExcept,
        pContext,
        pEstablisher,
        pCatchF,
        pFuncInfo,
        pEntry->tryLow,
        pEntry->catchHigh,
        pCatch,
        pDC,
        recursive
    );
}

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
    DISPATCHER_CONTEXT* pDC,             // Context within subject frame
    FuncInfo* pFuncInfo,    // Static information for subject frame
    __ehstate_t curState,               // Current state
    int catchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode* pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
)
{
    // We don't want to touch BreakPoint generated Exception.
    if (pExcept->ExceptionCode == STATUS_BREAKPOINT) {
        return;
    }

    switch (pExcept->ExceptionCode) {
#if !(PreReleaseMode || DebugMode)
    case STATUS_BREAKPOINT:
        Seed::toCoreLog(L"Критична помилка: Відбувається відлагодження");
        break;
#endif
    case STATUS_HEAP_CORRUPTION:
        Seed::toCoreLog(L"Критична помилка: Куча пам'яті пошкоджена");
#if (PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        NtTerminateProcess(NtCurrentProcess(), STATUS_HEAP_CORRUPTION);
        break;
    case STATUS_ACCESS_VIOLATION:
    {
        Seed::toCoreLog(L"Критична помилка: Спроба використати не існуючу ділянку пам'яті");
#if (PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        EXCEPTION_RECORD* sehException = (EXCEPTION_RECORD*)pExcept; //-V1027
        NullPointException::Types type;
        switch (sehException->ExceptionInformation[0])
        {
        case 0:
            type = NullPointException::Types::Read;
            break;
        case 1:
            type = NullPointException::Types::Write;
            break;
        case 8:
            type = NullPointException::Types::Execute;
            break;
        default:
            type = NullPointException::Types::Read;
            break;
        }
        Throw(Seed::NullPointException(ExceptionPosition, sehException->ExceptionInformation[1], type));
    }
    break;
    default:
        Seed::BaseString<Seed::Core::Memory::ExceptionHeap> str(L"Критична помилка: Невідома помилка. Код: ");
        str +=
            Seed::BaseString<Seed::Core::Memory::ExceptionHeap>::UInt64asHex(pExcept->ExceptionCode);
        Seed::toCoreLog(str);
#if !(PreReleaseMode || DebugMode)
        __debugbreak();
#endif
        NtTerminateProcess(NtCurrentProcess(), pExcept->ExceptionCode);
        break;
    }

    //if (__pSETranslator != nullptr && __pSETranslator != RtlEncodePointer(nullptr)) {
    //	// Call the translator.  If the translator knows what to
    //	// make of it, it will throw an appropriate C++ exception.
    //	// We intercept it and use it (recursively) for this
    //	// frame.  Don't recurse more than once.

    //	if (_CallSETranslator(pExcept, pRN, pContext, pDC, pFuncInfo,
    //		catchDepth, pMarkerRN)) {
    //		return;
    //	}
    //}

    auto tryBlockMap = TryBlockMap{ pFuncInfo, pDC };

    //DASSERT(tryBlockMap.getNumTryBlocks() > 0);

    if (pFuncInfo->nTryBlocks > 0)
    {
        // Didn't have a translator, or the translator returned normally (i.e.
        // didn't translate it).  Still need to check for match with ellipsis:
        auto startStop = GetRangeOfTrysToCheck(tryBlockMap, curState, catchDepth);
        // Scan the try blocks in the function:
        for (auto iter = startStop.start; iter < startStop.end; ++iter)
        {
            auto tryBlock = GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap + sizeof(TryBlockMapEntry) * iter);
            // If the try-block was in scope...
            if (tryBlock->tryLow > curState || curState > tryBlock->tryHigh) {
                continue;
            }

            // *and* the last catch in that try is an ellipsis (no other can be)
            auto handler = &GetByOffset<HandlerType>(pDC->ImageBase, tryBlock->dispHandlerArray)[tryBlock->nCatches];
            auto type = GetByOffset<TypeDescriptor>(pDC->ImageBase, handler->dispType);
            if (!((type != nullptr && type->name[0] == '\0') && !(handler->adjectives & HT_IsStdDotDot))) {
                continue;
            }

            // Found an ellipsis.  Handle exception.
            CatchIt(
                pDC,
                pExcept,
                pRN,
                pContext,
                pFuncInfo,
                handler,
                nullptr,
                tryBlock,
                catchDepth,
                pMarkerRN,
                TRUE,
                FALSE
            );
        }

        // If it returns, handler re-threw.  Keep searching.
    } // Search for try

    // If we got here, that means we didn't have anything to do with the
    // exception.  Continue search.
}

void FindHandler(
    EHExceptionRecord* pExcept,         // Information for this (logical)
                                        //   exception
    EHRegistrationNode* pRN,            // Dynamic information for subject frame
    CONTEXT* pContext,                  // Context info
    DISPATCHER_CONTEXT* pDC,             // Context within subject frame
    typename FuncInfo* pFuncInfo,    // Static information for subject frame
    BOOLEAN recursive,                  // TRUE if we're handling the
                                        //   translation
    int CatchDepth,                     // Level of nested catch that is being
                                        //   checked
    EHRegistrationNode* pMarkerRN       // Extra marker RN for nested catch
                                        //   handling
)
{
    BOOLEAN IsRethrow = FALSE;
    BOOLEAN gotMatch = FALSE;

    // Get the current state (machine-dependent)
    __ehstate_t curState = -1;

    curState = GetHandlerSearchState(pRN, pDC, pFuncInfo);

    //DASSERT(curState >= EH_EMPTY_STATE && curState < T::GetMaxState(pDC, pFuncInfo));

    // Check if it's a re-throw.  Use the exception we stashed away if it is.
    if (
        pExcept->ExceptionCode == EH_EXCEPTION_NUMBER && pExcept->NumberParameters == 3
        && (pExcept->params.magicNumber == EH_MAGIC_NUMBER1
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER2
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER3)
        && pExcept->params.pThrowInfo != nullptr) {
        if (ThreadExceptionInfo->_curexception == nullptr) {
            // Oops!  User re-threw a non-existent exception!  Let it propagate.
            throw InternalError(ExceptionPosition);
        }

        pExcept = (EHExceptionRecord*)ThreadExceptionInfo->_curexception;
        pContext = (CONTEXT*)ThreadExceptionInfo->_curcontext;
        IsRethrow = TRUE;
        ThreadExceptionInfo->_ThrowImageBase = (ptrdiff_t)pExcept->params.pThrowImageBase;

        //DASSERT(_ValidateRead(pExcept));
        //DASSERT(!PER_IS_MSVC_EH(pExcept) || PER_PTHROW(pExcept) != nullptr);

        //
        // We know it is a rethrow -- did we come here as a result of an
        // exception re-thrown from CallUnexpected() ?
        //
        if (ThreadExceptionInfo->_curexcspec != nullptr)
        {
            ESTypeList* pCurrentFuncInfo = (ESTypeList*)ThreadExceptionInfo->_curexcspec;   // remember it in a local variable
            ThreadExceptionInfo->_curexcspec = nullptr;   // and reset it immediately -- so we don't forget to do it later

            // Does the exception thrown by CallUnexpected belong to the exception specification?

            if (IsInExceptionSpec(pExcept, pCurrentFuncInfo))
            {
                // Yes it does -- so "continue the search for another handler at the call of the function
                // whose exception-specification was violated"
                ;
            }
            else
            {
                // Nope, it does not. Is std::bad_exception allowed by the spec?

                if (Is_bad_exception_allowed(pCurrentFuncInfo, (SizeT)pExcept->params.pThrowInfo))
                {
                    // yup -- so according to the standard, we need to replace the thrown
                    // exception by an implementation-defined object of the type std::bad_exception
                    // and continue the search for another handler at the call of the function
                    // whose exception-specification was violated.

                    // Just throw bad_exception -- we will then come into FindHandler for the third time --
                    // but make sure we will not get here again

                    __DestructExceptionObject(pExcept, TRUE, 1);   // destroy the original object

                    throw BadException(ExceptionPosition);
                }
                else
                {
                    EmergencyShutdown(ExceptionPosition, L"Bad exception not allowed");
                }
            }
        }
    }

    auto tryBlockMap = TryBlockMap{ pFuncInfo, pDC };

    if (pExcept->ExceptionCode == EH_EXCEPTION_NUMBER && pExcept->NumberParameters == 4
        && (pExcept->params.magicNumber == EH_MAGIC_NUMBER1
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER2
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER3)) {
        // Looks like it's ours.  Let's see if we have a match:
        //
        // First, determine range of try blocks to consider:
        // Only try blocks which are at the current catch depth are of interest.

        if (tryBlockMap.pFuncInfo->nTryBlocks > 0)
        {
            auto startStop = GetRangeOfTrysToCheck(tryBlockMap, curState, CatchDepth);
            // Scan the try blocks in the function:
            for (auto iter = startStop.start; iter < startStop.end; ++iter) {
                auto tryBlock = GetByOffset<TryBlockMapEntry>(pDC->ImageBase, pFuncInfo->dispTryBlockMap + sizeof(TryBlockMapEntry) * iter);
                CatchableType* pCatchable;
                CatchableTypeArray* ppCatchable;

                if (tryBlock->tryLow > curState || curState > tryBlock->tryHigh) {
                    continue;
                }

                // Try block was in scope for current state.  Scan catches for this
                // try:

                SizeT HandleCount = tryBlock->nCatches;
                HandlerType* handlerItem = GetByOffset<HandlerType>(pDC->ImageBase, tryBlock->dispHandlerArray);
                for (SizeT i = 0; i < HandleCount; i++)
                {
                    // Scan all types that thrown object can be converted to:
                    //pExcept->params.pThrowInfo->pCatchableTypeArray
                    ppCatchable = GetByOffset<CatchableTypeArray>(pDC->ImageBase, pExcept->params.pThrowInfo->pCatchableTypeArray);
                    for (int catchables = ppCatchable->nCatchableTypes;
                        catchables > 0; catchables--, ppCatchable++) { //-V2528
                        pCatchable = GetByOffset<CatchableType>(ThreadExceptionInfo->_ThrowImageBase, *(int*)ppCatchable->arrayOfCatchableTypes);

                        if (!TypeMatch(&handlerItem[i], pCatchable, pExcept->params.pThrowInfo, ThreadExceptionInfo->_ImageBase)) {
                            continue;
                        }

                        // OK.  We finally found a match.  Activate the catch.  If
                        // control gets back here, the catch did a re-throw, so
                        // keep searching.

                        gotMatch = TRUE;

                        CatchIt(
                            pDC,
                            pExcept,
                            pRN,
                            pContext,
                            pFuncInfo,
                            handlerItem + i,
                            pCatchable,
                            tryBlock,
                            CatchDepth,
                            pMarkerRN,
                            IsRethrow,
                            recursive
                        );
                        goto NextTryBlock; //-V779 //-V2535
                    } // Scan posible conversions
                } // Scan catch clauses
            NextTryBlock:;
            } // Scan try blocks
        } // if FUNC_NTRYBLOCKS( pFuncInfo ) > 0

        {
            //
            // We haven't found the match -- let's look at the exception spec and see if our try
            // matches one of the listed types.
            //
            // This block also handles noexcept termination if the current function is noexcept and
            // we are not handling a nested throw or re-throw. e.g.:
            //
            // try {
            //   try { throw 1; } (1)
            //   catch(int) { throw; /* or throw <something new>; */ } (2)
            // } catch(int) {} (3)
            //
            // During the initial search for a handler when throwing the original exception (1)
            // we find and execute catch handler (2). This throws, which re-enters this function
            // while in the context of a catch, but which cannot find a handler. We cannot terminate here,
            // we're in a recursive context. Just let it go, and we'll return back to the original search
            // from (1) and find (3). If we fail to find a handler, e.g. (3) didn't match, we would
            // then terminate at this point as we are not in a catch block.
            //
            // Catch block detection uses CatchDepth on X86, or _ExecutionInCatch on other platforms. On
            // these platforms CatchDepth is always 0 - we don't maintain a stack of entered try/catch
            // states.
            if (!gotMatch && pFuncInfo->magicNumber >= EH_MAGIC_NUMBER2 &&
                (getESTypes(pFuncInfo) != nullptr || (isNoExcept(pFuncInfo) &&
                    !ExecutionInCatch(pDC, pFuncInfo)
                    )))
            {
                // Are we noexcept?
                if (isNoExcept(pFuncInfo))
                {
                    EmergencyShutdown(ExceptionPosition, L"Не можливо виконувати розкрутку стекту для noexcept функцій");
                }

                if (!IsInExceptionSpec(pExcept, getESTypes(pFuncInfo)))
                {
                    // Nope, it does not. Call unexpected

                    //
                    // We must unwind the stack before calling unexpected -- this makes it work
                    // as if it were inside catch(...) clause
                    //
                    EHRegistrationNode EstablisherFramePointers;
                    EHRegistrationNode* pEstablisher = GetEstablisherFrame(pRN, pDC, pFuncInfo, &EstablisherFramePointers);
                    UnwindNestedFrames(pRN,
                        pExcept,
                        pContext,
                        pEstablisher,
                        nullptr,
                        pFuncInfo,
                        EH_EMPTY_STATE,
                        EH_EMPTY_STATE,
                        nullptr,
                        pDC,
                        recursive
                    );
                }
            }
        }
    } // It was a C++ EH exception
    else
    {
        // Not ours.  But maybe someone told us how to make it ours.

        if (pFuncInfo->nTryBlocks > 0)
        {
            if (!recursive) {
                FindHandlerForForeignException(pExcept, pRN, pContext, pDC,
                    pFuncInfo, curState, CatchDepth, pMarkerRN);
            }
            else {
                // We're recursive, and the exception wasn't a C++ EH!
                // Translator threw something uninteligable.

                // Two choices here: we could let the new exception take over, or we could abort.
                // We throw internal error.

                throw InternalError(ExceptionPosition);
            }
        }
    } // It wasn't our exception

    //DASSERT(_pCurrentFuncInfo == nullptr);   // never leave it initialized with something
}

__forceinline EXCEPTION_DISPOSITION InternalLanguageSpecficHandle(
    EHExceptionRecord* pExcept,        // Information for this exception
    EHRegistrationNode* pRN,            // Dynamic information for this frame
    CONTEXT* pContext,                  // Context info
    DISPATCHER_CONTEXT* pDC,             // Context within subject frame
    FuncInfo* pFuncInfo,    // Static information for this frame
    int CatchDepth,                     // How deeply nested are we?
    EHRegistrationNode* pMarkerRN,      // Marker node for when checking inside
                                        //  catch block
    BOOLEAN recursive                   // Are we handling a translation?
) {
    //Verify

    //Is unwind?
    if (pExcept->ExceptionFlags & 0x66) { //0x66 Unwind mask
        if ((pExcept->ExceptionFlags & 0x20) && pExcept->ExceptionCode == STATUS_UNWIND_CONSOLIDATE) {
            PEXCEPTION_RECORD pSehExcept = (PEXCEPTION_RECORD)pExcept;
            __ehstate_t target_state = (__ehstate_t)pSehExcept->ExceptionInformation[3];
            FrameUnwindToState((EHRegistrationNode*)pSehExcept->ExceptionInformation[1],
                pDC,
                pFuncInfo,
                target_state);
            return ExceptionContinueSearch;
        }

        FrameUnwindToEmptyState(pRN, pDC, pFuncInfo);
        return ExceptionContinueSearch;
    }

    if (pFuncInfo->nTryBlocks != 0
        //
        // If the function has no try block, we still want to call the
        // frame handler if there is an exception specification
        //
        || (pFuncInfo->magicNumber >= EH_MAGIC_NUMBER2 && getESTypes(pFuncInfo) != nullptr)
        || (pFuncInfo->magicNumber >= EH_MAGIC_NUMBER3 && isNoExcept(pFuncInfo) != 0)
        ) {
        FindHandler(pExcept, pRN, pContext, pDC, pFuncInfo, recursive, CatchDepth, pMarkerRN);
    }
    return ExceptionContinueSearch;
}

#define pFrameInfoChain   (*((FRAMEINFO **)    &(ThreadExceptionInfo->_pFrameInfoChain)))

__forceinline
FRAMEINFO* __cdecl _CreateFrameInfo(
    FRAMEINFO* pFrameInfo,
    PVOID       pExceptionObject
) {
    ThreadExceptionInfo->_pFrameInfoChain = nullptr;
    pFrameInfo->pExceptionObject = pExceptionObject;
    pFrameInfo->pNext = (pFrameInfo < ThreadExceptionInfo->_pFrameInfoChain) ? ThreadExceptionInfo->_pFrameInfoChain : nullptr;
    ThreadExceptionInfo->_pFrameInfoChain = pFrameInfo;
    return pFrameInfo;
}

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
) {
    // Get the exception record thrown (don't care about other info)
    EHExceptionRecord* pExcept = (EHExceptionRecord*)pExPtrs->ExceptionRecord;
    *rethrow = 0;
    if (pExcept->ExceptionCode == EH_EXCEPTION_NUMBER && pExcept->NumberParameters == 3
        && (pExcept->params.magicNumber == EH_MAGIC_NUMBER1
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER2
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER3) &&
        pExcept->params.pThrowInfo == pOldExcept->params.pThrowInfo)
        *rethrow = 1;
    // Check if it's ours and it's has no exception information.
    if (pExcept->ExceptionCode == EH_EXCEPTION_NUMBER && pExcept->NumberParameters == 3
        && (pExcept->params.magicNumber == EH_MAGIC_NUMBER1
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER2
            || pExcept->params.magicNumber == EH_MAGIC_NUMBER3)
        && pExcept->params.pThrowInfo == nullptr) {
        // In a rethrow, ExceptionCode isn't set to EH_EXCEPTION_NUMBER even for a
        // C++ Exception. Hence we use cxxReThrow in __InternalCxxFrameHandler
        // to check for a C++ Exception
        ThreadExceptionInfo->_cxxReThrow = TRUE;
        *rethrow = 1;
        return EXCEPTION_EXECUTE_HANDLER;
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

__forceinline __declspec(noreturn)
void __RethrowException(EHExceptionRecord* pThisException)
{
    EXCEPTION_RECORD record = { 0 };
    record.ExceptionCode = pThisException->ExceptionCode; //-V522
    record.ExceptionFlags = pThisException->ExceptionFlags;

    record.ExceptionInformation[0] = static_cast<ULONG_PTR>(pThisException->params.magicNumber);
    record.ExceptionInformation[1] = reinterpret_cast<ULONG_PTR>(pThisException->params.pExceptionObject);
    record.ExceptionInformation[2] = reinterpret_cast<ULONG_PTR>(pThisException->params.pThrowInfo);
    record.ExceptionInformation[3] = reinterpret_cast<ULONG_PTR>(pThisException->params.pThrowImageBase);

    RtlRaiseException(&record);
}

bool _IsExceptionObjectToBeDestroyed(void* pExceptionObject)
{
    void** i;

    for (i = (void**)ThreadExceptionInfo->_pFrameInfoChain; i; i = (void**)i[1])
    {
        if (*i == pExceptionObject)
            return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////
//
// _FindAndUnlinkFrame - Pop the frame information for this scope that was
//  pushed by _CreateFrameInfo.  This should be the first frame in the list,
//  but the code will look for a nested frame and pop all frames, just in
//  case.
//
void _FindAndUnlinkFrame(
    FRAMEINFO* pFrameInfo
) {
    //DASSERT(pFrameInfo == pFrameInfoChain);

    for (FRAMEINFO* pCurFrameInfo = pFrameInfoChain;
        pCurFrameInfo != nullptr;
        pCurFrameInfo = pCurFrameInfo->pNext)
    {
        if (pFrameInfo == pCurFrameInfo) {
            pFrameInfoChain = pCurFrameInfo->pNext;
            return;
        }
    }

    EmergencyShutdown(ExceptionPosition, L"Список покажчиків на помилки пошкоджений");
}

void ThrowBadException(const ConstAString& File, const ConstAString& Function, UInt64 Line) {
#pragma warning(push)
#pragma warning(disable: 4996)
    ___cxxthrow(Exceptions::BadException(File, Function, Line));
#pragma warning(pop)
}

void* InternalCxxCallCatchBlock(
    EXCEPTION_RECORD* pExcept
)
{
    int rethrow = 0, TranslatedCatch = 0;
    void* handlerAddress;
    void* continuationAddress = nullptr;
    FuncInfo* pFuncInfo;
    FRAMEINFO FrameInfo = { 0 };
    CONTEXT* pContext, * pSaveContext;
    EHRegistrationNode* pEstablisherFrame;
    EHExceptionRecord* pThisException, * pSaveException, * pForeignException = nullptr;

    pSaveContext = (CONTEXT*)ThreadExceptionInfo->_curcontext;
    pSaveException = (EHExceptionRecord*)ThreadExceptionInfo->_curexception;

    // Copy Necessary Information which is passed from UnwindNestedFrames.
    pThisException = (EHExceptionRecord*)pExcept->ExceptionInformation[6];
    pFuncInfo = (FuncInfo*)pExcept->ExceptionInformation[5];
    pContext = (CONTEXT*)pExcept->ExceptionInformation[4];
    handlerAddress = (void*)pExcept->ExceptionInformation[2];
    pEstablisherFrame = (EHRegistrationNode*)pExcept->ExceptionInformation[1];

    //VerifyCpuContext(pContext);

    ThreadExceptionInfo->_curcontext = pContext;
    ThreadExceptionInfo->_curexception = pThisException;

    FRAMEINFO* pFrameInfo = _CreateFrameInfo(&FrameInfo,
        (void*)((EHExceptionRecord*)ThreadExceptionInfo->_curexception)->params.pExceptionObject);

    if (pExcept->ExceptionInformation[7]) {
        // Stuff for SehTranslation book keeping
        // This Exception Object is Translation of Foreign Exception And should
        // be destroyed in any case. If there is rethrow, throw ForeignException.
        // IMPORTANT: No one else can rethrow this Exception Object.
        TranslatedCatch = 1;
        pForeignException = (EHExceptionRecord*)ThreadExceptionInfo->_pForeignException;
        // End Translation Stuff
    }

    __try {
        __try {
            ThreadExceptionInfo->_isCatchBlockProceding = true;
            continuationAddress = _CallSettingFrame(handlerAddress,
                pEstablisherFrame,
                0x100);
            ThreadExceptionInfo->_isCatchBlockProceding = false;
        }
        __except (ExFilterRethrow(exception_info(),
            pThisException,
            &rethrow)) {
            ThreadExceptionInfo->_isCatchBlockProceding = false;
            rethrow = 1;
            ThreadExceptionInfo->_cxxReThrow = FALSE;
            if (TranslatedCatch == 1) {
                // Note in case of rethrow, no one else can convert rethrow to
                // pThisException. This means only this except will deal with conversion
                // of rethrow to pThisException. Instead of pThisException, we
                // will throw original Foreign Exception. Also we will need to
                // destroy Exception Object before Raising Foreign Exception.
                __DestructExceptionObject(pThisException, TRUE, 0);
                __RethrowException(pForeignException);
            }
            else {
                __RethrowException(pThisException);
            }
        }
    }
    __finally {
        _FindAndUnlinkFrame(pFrameInfo);
        if (!rethrow &&
            pThisException->ExceptionCode == EH_EXCEPTION_NUMBER && pThisException->NumberParameters == 4
            && (pThisException->params.magicNumber == EH_MAGIC_NUMBER1
                || pThisException->params.magicNumber == EH_MAGIC_NUMBER2
                || pThisException->params.magicNumber == EH_MAGIC_NUMBER3) &&
            _IsExceptionObjectToBeDestroyed(pThisException->params.pExceptionObject))
        {
            __DestructExceptionObject(pThisException, TRUE, 0);
        }
        ThreadExceptionInfo->_curexception = pSaveException;
        ThreadExceptionInfo->_curcontext = pSaveContext;

        *GetByOffset<UInt64>((SizeT)pEstablisherFrame, pFuncInfo->dispUwindHelp) = -2;
    }
    return continuationAddress;
}

EXCEPTION_DISPOSITION InternalCxxFrameHandler3(
    EHExceptionRecord* pExcept,         // Information for this exception
    EHRegistrationNode RN,               // Dynamic information for this frame
    CONTEXT* pContext,        // Context info
    DISPATCHER_CONTEXT* pDC              // More dynamic info for this frame
) {
    ThreadExceptionInfo->_ImageBase = pDC->ImageBase;
    ThreadExceptionInfo->_ThrowImageBase = (UInt64)pExcept->params.pThrowImageBase;
    FuncInfo* pFuncInfo = GetByOffset<FuncInfo>(pDC->ImageBase, *(PULONG)pDC->HandlerData);
    return InternalLanguageSpecficHandle(pExcept, &RN, pContext, pDC, pFuncInfo, 0, nullptr, FALSE);
}

#if defined (_M_CEE)
#define CALLTYPE __clrcall
#define CALEETYPE __clrcall
#define __RELIABILITY_CONTRACT \
    [System::Runtime::ConstrainedExecution::ReliabilityContract( \
        System::Runtime::ConstrainedExecution::Consistency::WillNotCorruptState, \
        System::Runtime::ConstrainedExecution::Cer::Success)]
#define ASSERT_UNMANAGED_CODE_ATTRIBUTE [System::Security::Permissions::SecurityPermissionAttribute(System::Security::Permissions::SecurityAction::Assert, UnmanagedCode = true)]
#define SECURITYCRITICAL_ATTRIBUTE [System::Security::SecurityCritical]
#define SECURITYSAFECRITICAL_ATTRIBUTE [System::Security::SecuritySafeCritical]
#else
#define CALEETYPE __stdcall
#define __RELIABILITY_CONTRACT
#define ASSERT_UNMANAGED_CODE_ATTRIBUTE
#define SECURITYCRITICAL_ATTRIBUTE
#define SECURITYSAFECRITICAL_ATTRIBUTE
#if defined (_M_IX86)
#define CALLTYPE __thiscall
#else
#define CALLTYPE __stdcall
#endif
#endif

__RELIABILITY_CONTRACT
ASSERT_UNMANAGED_CODE_ATTRIBUTE
SECURITYCRITICAL_ATTRIBUTE
static int ArrayUnwindFilter(EXCEPTION_POINTERS* pExPtrs)
{
    EHExceptionRecord* pExcept = (EHExceptionRecord*)pExPtrs->ExceptionRecord;

    switch (pExcept->ExceptionCode) //-V2520
    {
    case EH_EXCEPTION_NUMBER:
        EmergencyShutdown(ExceptionPosition, L"Dtors can't throw exceptions");
        //terminate();
#ifdef ALLOW_UNWIND_ABORT
    case EH_ABORT_FRAME_UNWIND_PART:
        return EXCEPTION_EXECUTE_HANDLER;
#endif
    default:
        return EXCEPTION_CONTINUE_SEARCH;
    }
}

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
void CALEETYPE __ArrayUnwind(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    int         count,              // Number of elements in the array
    void(CALLTYPE* pDtor)(void*)    // The destructor to call
)
{
    // 'unwind' rest of array

    __try
    {
        while (--count >= 0)
        {
            ptr = (char*)ptr - size;
            (*pDtor)(ptr);
        }
    }
    __except (ArrayUnwindFilter(exception_info()))
    {
        ; // Deliberately do nothing
    }
}

__RELIABILITY_CONTRACT
inline void CALEETYPE __ehvec_ctor(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    size_t      count,              // Number of elements in the array
    void(CALLTYPE* pCtor)(void*),   // Constructor to call
    void(CALLTYPE* pDtor)(void*)    // Destructor to call should exception be thrown
)
{
    size_t i = 0;      // Count of elements constructed
    int success = 0;

    __try
    {
        // Construct the elements of the array
        for (; i < count; i++)
        {
            (*pCtor)(ptr);
            ptr = (char*)ptr + size;
        }
        success = 1;
    }
    __finally
    {
        if (!success)
            __ArrayUnwind(ptr, size, (int)i, pDtor);
    }
}

__RELIABILITY_CONTRACT
SECURITYCRITICAL_ATTRIBUTE
inline void CALEETYPE __ehvec_dtor(
    void* ptr,                // Pointer to array to destruct
    size_t      size,               // Size of each element (including padding)
    //  int         count,              // Number of elements in the array
    size_t      count,              // Number of elements in the array
    void(CALLTYPE* pDtor)(void*)    // The destructor to call
)
{
    _Analysis_assume_(count > 0);

    int success = 0;

    // Advance pointer past end of array
    ptr = (char*)ptr + size * count;

    __try
    {
        // Destruct elements
        while (count-- > 0)
        {
            ptr = (char*)ptr - size;
            (*pDtor)(ptr);
        }
        success = 1;
    }
    __finally
    {
        if (!success)
            __ArrayUnwind(ptr, size, (int)count, pDtor);
    }
}

extern void* InternalHeapFree(void* MemBlk);
extern void* InternalHeapAllocate(Seed::SizeT Size);

_Ret_notnull_ _Post_writable_byte_size_(_Size)
extern "C" void* __cdecl _alloca(_In_ size_t _Size);

union RawExceptionInfo {
    EXCEPTION_RECORD NtExceptionInfo;
    EHExceptionRecord CxxExceptionInfo;
};

void NtStatusExceptionMethodsCallsLogging(ConstAString& FuncName) {
    toCoreLog(L"NtException copy ctor" + String(FuncName));
}

void ShowExData(void* ptr, const ConstString& Label) {
    using SafeString = BaseString<Memory::ExceptionHeap>;
    RawExceptionInfo* record = (RawExceptionInfo*)ptr;
    NtException& ex = (NtException&)record->CxxExceptionInfo.params.pExceptionObject;
    toCoreLog(L"Відлагодження: " + SafeString(Label));
}

////////////////////////////////////////////////////////////////////////////////
//
// void _CallCopyCtor();
//
// Helper function for calling the copy-ctor on the C++ exception object.  It reads
// the passed info throw info and determine the type of exception.  If it's a simple type
// it calls memcpy.  It's a UDT with copy-ctor, it uses the copy-ctor.
//
void _CallCopyCtor(_Out_writes_bytes_(size) void* dst, _In_reads_bytes_(size) void* src, size_t size, _In_ const CatchableType* const pType, void* ThrowImageBase)
{
    if (pType->properties & CT_IsSimpleType || // this is a simple type
        pType->copyFunction == NULL    // this is a user defined type without copy ctor
        )
    {
        // just copy with memcpy
        Seed::Memory::Copy(src, dst, size);
    }
    else
    {
        try
        {
            // it's a user defined type with a copy ctor
            if (pType->properties & CT_HasVirtualBase)
            {
                // this exception got a virtual base
                _CallMemberFunction2((char*)dst,
                    GetByOffset<void*>((SizeT)ThrowImageBase, pType->copyFunction),
                    __AdjustPointer(src,
                        pType->thisDisplacement), 1);
            }
            else
            { //-V565
                _CallMemberFunction1((char*)dst,
                    GetByOffset<void*>((SizeT)ThrowImageBase, pType->copyFunction),
                    __AdjustPointer(src,
                        pType->thisDisplacement));
            }
        }
        catch (...)
        {
            // the copy-Ctor() threw.  We need to terminate().
            //terminate();
            EmergencyShutdown(ExceptionPosition, L"copy-Ctor throw exception");
        }
    }
}

extern "C" extern void __chkstk();

////////////////////////////////////////////////////////////////////////////////
//
// void _CopyException();
//
// Builds an __ExceptionPtr with the given c++ exception object and its corresponding
// throwinfo.
//
// This function returns the static copy of exception_ptr which points to a
// std::bad_alloc exception in out-of-memory situation.
//
void _CopyException(RawExceptionInfo* orig, RawExceptionInfo* copy)
{
    //copying the _EXCEPTION_RECORD (for SEH exceptions)
    copy->NtExceptionInfo.ExceptionCode = orig->NtExceptionInfo.ExceptionCode;
    copy->NtExceptionInfo.ExceptionFlags = orig->NtExceptionInfo.ExceptionFlags;
    copy->NtExceptionInfo.ExceptionRecord = NULL; // We don't chain SEH exceptions
    copy->NtExceptionInfo.ExceptionAddress = NULL; // Useless field to copy->  It will be overwritten by RaiseException()
    copy->NtExceptionInfo.NumberParameters = orig->NtExceptionInfo.NumberParameters;

    // copying any addition parameters
    for (ULONG i = 0; i < copy->NtExceptionInfo.NumberParameters && i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        copy->NtExceptionInfo.ExceptionInformation[i] = orig->NtExceptionInfo.ExceptionInformation[i];
    }
    // NULLing out any un-used parameters
    for (ULONG i = copy->NtExceptionInfo.NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        copy->NtExceptionInfo.ExceptionInformation[i] = 0;
    }

    if (orig->CxxExceptionInfo.ExceptionCode == EH_EXCEPTION_NUMBER &&
        orig->CxxExceptionInfo.NumberParameters == EH_EXCEPTION_PARAMETERS && (
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER1 ||
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER2 ||
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER3))
    {
        // this is a C++ exception
        // we need to copy the C++ exception object

        // nulling out the Exception object pointer, because we haven't copied it yet
        copy->CxxExceptionInfo.params.pExceptionObject = NULL;

        ThrowInfo* pThrow = orig->CxxExceptionInfo.params.pThrowInfo;
        if (orig->CxxExceptionInfo.params.pExceptionObject == NULL ||
            pThrow == NULL ||
            pThrow->pCatchableTypeArray == NULL
            )
        {
            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
            //terminate();
            EmergencyShutdown(ExceptionPosition, L"Throw info empty");
        }

        // we finally got the type info we want
#define THROW_CTLIST_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->arrayOfCatchableTypes)
#define THROW_CTARRAY_IB(t,ib)			((CatchableTypeArray*)((ib) + (t).pCatchableTypeArray))
#define _GetThrowImageBase() (SizeT)orig->CxxExceptionInfo.params.pThrowImageBase
        const auto typeInfo = (CatchableType*)
            (*(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase()); //-V1004
        void* pExceptionBuffer = NULL;

        if (sizeof(NtException) != typeInfo->sizeOrOffset)
            toCoreLog(L"Попередження: розмір типу не збігається з інформацією з метаданих виконавчого файла");
        //TODO: Analyze memory leak
        pExceptionBuffer = InternalHeapAllocate(typeInfo->sizeOrOffset);

        _CallCopyCtor(pExceptionBuffer, orig->CxxExceptionInfo.params.pExceptionObject, typeInfo->sizeOrOffset, typeInfo, orig->CxxExceptionInfo.params.pThrowImageBase);

        copy->CxxExceptionInfo.params.pExceptionObject = pExceptionBuffer;
    }
}

Seed::Exception::RawException Seed::Core::Exceptions::Exception::StoreCurrentException()
{
    if (ThreadExceptionInfo->_isCatchBlockProceding) {
        RawExceptionInfo* current = static_cast<RawExceptionInfo*>(ThreadExceptionInfo->_curexception);
        RawExceptionInfo* record = static_cast<RawExceptionInfo*>(InternalHeapAllocate(sizeof(RawExceptionInfo)));
        RtlSecureZeroMemory(record, sizeof(RawExceptionInfo));
        _CopyException(current, record);
        return RawException(record);
    }
    ___cxxthrow(Exceptions::NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
}

void ThrowExceptionFromExceptionStorage(EXCEPTION_RECORD record) {
    RtlRaiseException(&record);
}

void Seed::Core::Exceptions::Exception::ThrowRawException(RawException&& exception)
{
    if (exception.ptr == nullptr)
        ___cxxthrow(Exceptions::NullPointException(ExceptionPosition, 0, NullPointException::Types::Read));
    EXCEPTION_RECORD record = { 0 };
    EXCEPTION_RECORD* ex = (EXCEPTION_RECORD*)exception.ptr;
    auto copy = (RawExceptionInfo*)&record;
    auto orig = (RawExceptionInfo*)ex;

    //copying the _EXCEPTION_RECORD (for SEH exceptions)
    copy->NtExceptionInfo.ExceptionCode = orig->NtExceptionInfo.ExceptionCode;
    copy->NtExceptionInfo.ExceptionFlags = orig->NtExceptionInfo.ExceptionFlags;
    copy->NtExceptionInfo.ExceptionRecord = NULL; // We don't chain SEH exceptions
    copy->NtExceptionInfo.ExceptionAddress = NULL; // Useless field to copy->  It will be overwritten by RaiseException()
    copy->NtExceptionInfo.NumberParameters = orig->NtExceptionInfo.NumberParameters;

    // copying any addition parameters
    for (ULONG i = 0; i < copy->NtExceptionInfo.NumberParameters && i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        copy->NtExceptionInfo.ExceptionInformation[i] = orig->NtExceptionInfo.ExceptionInformation[i];
    }
    // NULLing out any un-used parameters
    for (ULONG i = copy->NtExceptionInfo.NumberParameters; i < EXCEPTION_MAXIMUM_PARAMETERS; ++i)
    {
        copy->NtExceptionInfo.ExceptionInformation[i] = 0;
    }

    if (orig->CxxExceptionInfo.ExceptionCode == EH_EXCEPTION_NUMBER &&
        orig->CxxExceptionInfo.NumberParameters == EH_EXCEPTION_PARAMETERS && (
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER1 ||
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER2 ||
            orig->CxxExceptionInfo.params.magicNumber == EH_MAGIC_NUMBER3))
    {
        // this is a C++ exception
        // we need to copy the C++ exception object

        // nulling out the Exception object pointer, because we haven't copied it yet
        copy->CxxExceptionInfo.params.pExceptionObject = NULL;

        ThrowInfo* pThrow = orig->CxxExceptionInfo.params.pThrowInfo;
        if (orig->CxxExceptionInfo.params.pExceptionObject == NULL ||
            pThrow == NULL ||
            pThrow->pCatchableTypeArray == NULL
            )
        {
            // No ThrowInfo exists.  If this were a C++ exception, something must have corrupted it.
            //terminate();
            EmergencyShutdown(ExceptionPosition, L"Throw info empty");
        }

        // we finally got the type info we want
        //auto typeInfo = GetByOffset<CatchableType>((SizeT)orig->CxxExceptionInfo.params.pThrowImageBase, pThrow->pCatchableTypeArray);
#define THROW_CTLIST_IB(t,ib)			(THROW_CTARRAY_IB(t,ib)->arrayOfCatchableTypes)
#define THROW_CTARRAY_IB(t,ib)			((CatchableTypeArray*)((ib) + (t).pCatchableTypeArray))
#define _GetThrowImageBase() (SizeT)orig->CxxExceptionInfo.params.pThrowImageBase
        const auto typeInfo = (CatchableType*)
            (*(int*)THROW_CTLIST_IB(*pThrow, _GetThrowImageBase()) + _GetThrowImageBase()); //-V1004
        //		const auto typeInfo2 = ;
        //		const auto typeInfo = (CatchableType*)(*(int*)
        //			GetByOffset<CatchableTypeArray>(
        //			(SizeT)orig->CxxExceptionInfo.params.pThrowImageBase,
        //				pThrow->pCatchableTypeArray
        //				) + (SizeT)orig->CxxExceptionInfo.params.pThrowImageBase);
        void* pExceptionBuffer = NULL;

        InternalHeapFree(ThreadExceptionInfo->throwObjStorageForStoredException);

        if (sizeof(NtException) != typeInfo->sizeOrOffset)
            toCoreLog(L"Попередження: розмір типу не збігається з інформацією з метаданих виконавчого файла");

        ThreadExceptionInfo->throwObjStorageForStoredException = InternalHeapAllocate(typeInfo->sizeOrOffset);

        pExceptionBuffer = ThreadExceptionInfo->throwObjStorageForStoredException;

        _CallCopyCtor(pExceptionBuffer, orig->CxxExceptionInfo.params.pExceptionObject, typeInfo->sizeOrOffset, typeInfo, orig->CxxExceptionInfo.params.pThrowImageBase);

        copy->CxxExceptionInfo.params.pExceptionObject = pExceptionBuffer;
    }
    ThrowExceptionFromExceptionStorage(record);
}

Seed::Core::Exceptions::Exception::RawException::~RawException()
{
    if (this->ptr != nullptr) {
        RawExceptionInfo* data = (RawExceptionInfo*)this->ptr;
        __DestructExceptionObject(&data->CxxExceptionInfo, TRUE, 0);
        data->CxxExceptionInfo.params.pExceptionObject = InternalHeapFree(data->CxxExceptionInfo.params.pExceptionObject);
        this->ptr = InternalHeapFree(this->ptr);
    }
}

#pragma warning(pop)