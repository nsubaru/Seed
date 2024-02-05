#include "Seed.Core.Threading.DataFlowController.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API

#include "Seed.Core.Threading.TLS.h"

Seed::Core::Threading::DataFlowController::ConsumerLockReleaser::~ConsumerLockReleaser()
{
    Interlocked::Store(This.isProducerWork, false);
    Internals::ReleaseLockExclusiveSRWLock(&This.lock);

    // If GetTLS()->ExceptionInfo._ProcessingThrow != 0 then we in FrameUnwindToState (Seed.Core.Exceptions.ABI.Internal.API.cpp) 
    // this function call dtors of all objects in scope durring exception processing. 
    // For us it's mean that we called durring throwing exception, so we must skip waking condition variable 
    if (GetTLS()->ExceptionInfo._ProcessingThrow == 0)
    {
        Internals::WakeOneCondVal(&This.consumer);
    }
}

Seed::Core::Threading::DataFlowController::ProducerLockReleaser::~ProducerLockReleaser()
{
    Interlocked::Store(This.isConsumerWork, false);
    Internals::ReleaseLockExclusiveSRWLock(&This.lock);

    // If GetTLS()->ExceptionInfo._ProcessingThrow != 0 then we in FrameUnwindToState (Seed.Core.Exceptions.ABI.Internal.API.cpp) 
    // this function call dtors of all objects in scope durring exception processing. 
    // For us it's mean that we called durring throwing exception, so we must skip waking condition variable 
    if (GetTLS()->ExceptionInfo._ProcessingThrow == 0)
    {
        Internals::WakeOneCondVal(&This.producer);
    }
}