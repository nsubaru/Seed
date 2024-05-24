#include "Seed.Core.Threading.Async.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Threading.TLS.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Threading;
using namespace Seed::Core::Containers;

/*
 * PDebugRightsCoAwaitsSecFunct MUST be an extern const variable, which will be aliased to
 * PDefDebugRightsSecFunct if no real user definition is present, thanks to the
 * alternatename directive. Details: https://devblogs.microsoft.com/oldnewthing/20200731-00/?p=104024
 */

extern "C" extern void(*PDebugRightsCoAwaitsSecFunct)();
extern "C" extern void(*PDefDebugRightsCoAwaitsSecFunct)() = NULL;

#pragma comment(linker, "/alternatename:PDebugRightsCoAwaitsSecFunct=PDefDebugRightsCoAwaitsSecFunct")

static Containers::ConcurencyList<CoAwaitInformation>& CurentlyActiveTasks()
{
    static auto curentlyActiveTasks = Containers::ConcurencyList<CoAwaitInformation>();
    return curentlyActiveTasks;
}

Containers::ConcurencyList<CoAwaitInformation>::Iterator Seed::Core::Threading::StoreTaskInformation(
    Containers::Tuple<RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RawMemBlk, Handle, RawMemBlk>&& payloadInformation,
    RTTI::RttiTypeRecord taskType,      
    RawMemBlk taskAddress, 
    RawMemBlk coroAddress)
{
    auto&& [ioObjType, ioObjIterface, payloadType, ioObjIntefaceAddr, ioObjNtHandle, payloadAddress] = payloadInformation;

    return CurentlyActiveTasks().AddLast(CoAwaitInformation{
        .TaskType                      = taskType,
        .TaskObjAddress                = taskAddress,
        .CoreAddress                   = coroAddress,
        .InitiatedThreadId             = Threading::GetTID(),
        .InitiatedThreadThreadableType = Threading::GetTLS()->Threadable->GetThreadableType(),
        .TaskRequestorObjectType       = ioObjType,
        .TaskRequestorInterfaceType    = ioObjIterface,
        .TaskRequestorInterfaceAddress = ioObjIntefaceAddr,
        .TaskNtObjectHandle            = ioObjNtHandle,
        .ThreadableContextId           = Threading::GetTLS()->contexts->GetObjId(),
        .PayloadType                   = payloadType,
        .PayloadAddress                = payloadAddress
    });
}

Containers::Array<CoAwaitInformation> Seed::Core::Threading::GetAllCoAwaits()
{
    if (PDebugRightsCoAwaitsSecFunct != NULL)
    {
        PDebugRightsCoAwaitsSecFunct();
    }

    return CurentlyActiveTasks().Select<Containers::Array>();
}

RTTI::RttiTypeRecord Seed::Core::Threading::AwaitTimePayload::GetTargetObjIntefaceType() const
{
    return TypeRecord<RawMemBlk>();
}

Containers::Tuple<RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RawMemBlk, Handle, RawMemBlk> Seed::Core::Threading::AwaitTimePayload::GetAwaitObjectInformation()
{
    return Containers::Tuple<RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RawMemBlk, Handle, RawMemBlk>(
        TypeRecord<RawMemBlk>(), GetTargetObjIntefaceType(), TypeRecord<AwaitTimePayload>(), tpHandle, nullptr, this);
}