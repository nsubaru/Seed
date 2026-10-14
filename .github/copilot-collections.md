# Seed collections

Use Seed containers and their LINQ-like operations. Verify local iterator and
ownership semantics before substituting standard containers or range utilities.
Null iterators and explicit materialization types are intentional contracts.

List and concurrency-list iterators have different lifetime/locking rules.
Preserve owner references while asynchronous work retains an iterator. Snapshot
under the relevant lock when returning owned values to callers.

SensitiveBlob, SensitiveArray and sensitive allocators hold private material.
Do not copy secret bytes into ordinary Blob/String values for convenience.
Memory::Copy takes source, destination, size; check this order at each call.

Use Result, Optional, Either and UnionType with their established construction,
destruction and move behavior. Do not simplify away payload cleanup or replace
their diagnostics/exception behavior with standard-library equivalents.
