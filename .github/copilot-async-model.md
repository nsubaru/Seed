# Seed async model

Native async work uses BaseTask, IAwaitable, IThreadable, UnifiedThreadPool and
service-aware context helpers. Preserve payload cleanup, MarkFinished ordering,
RawException ownership, co-await diagnostics, metrics and context blocks.

Threadable interruption can redirect execution and throw InterruptException at
any instruction. Do not introduce noexcept on normal runtime methods. Narrow
compiler/coroutine-required exception specifications need a documented reason
and must prevent interruption from escaping that boundary.

Keep runtime-managed tasks alive until completion or cancellation cleanup.
Cancellation/disconnect must release session resources without taking ownership
of long-lived runtime services away from their composition owner.

KMS finite commands own a Core worker and a session cancellation token. The
transport signals cancellation on disconnect or shutdown and joins the worker
before releasing its callback state or accepting another console. Native test
runs observe cancellation outside case exception handling, before and after
complete cases; current-case cleanup finishes first. Do not replace this with
forced termination during file writes, configuration changes or service mutation.

Use native asynchronous IO wrappers for sockets and pipes. Account for partial
IO, deadlines, malformed input and cancellation. Thread-pool callbacks retain
the service context rather than bypassing it with unrelated raw callbacks.

An IO callback may finish before the initiating coroutine checks readiness.
Payload completion hooks must have one owner. If the callback already owns
completion, the awaiter must still publish its continuation so the callback can
resume and release its task. Request cleanup and socket registration must never
run a second time through the synchronous readiness path.

An overlapped wait timeout is distinct from native IO completion. Cancel the exact
request and wait for its completion event before releasing its IO status block or
buffer. A raw IOverlappedTask is lazy: start it through co_await; ordinary Task
coroutine wrappers can then be joined from synchronous control code.
