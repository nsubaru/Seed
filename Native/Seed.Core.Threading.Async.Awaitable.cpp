#include "Seed.Core.Threading.Async.Awaitable.h"

using namespace Seed;
using namespace Seed::Core;
using namespace Seed::Core::Threading;

Seed::Core::RTTI::RttiTypeRecord AwaitablePayload::GetTargetObjIntefaceType() const
{
    return TypeRecord<IAwaitable*>();
}

Containers::Tuple<RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RawMemBlk, Handle, RawMemBlk> AwaitablePayload::GetAwaitObjectInformation()
{
	return Containers::Tuple<RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RTTI::RttiTypeRecord, RawMemBlk, Handle, RawMemBlk>(
		This->task->GetAwaitableType(), GetTargetObjIntefaceType(), TypeRecord<AwaitablePayload>(), (RawMemBlk) & *This->task, This->task->GetAwaitableHandle(), this);
}