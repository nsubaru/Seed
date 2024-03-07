#include "Seed.Core.Threading.Async.Awaitable.h"

Seed::Core::RTTI::RttiTypeRecord Seed::Core::Threading::AwaitablePayload::GetTargetObjIntefaceType() const
{
    return TypeRecord<IAwaitable*>();
}