# Seed anti-patterns

- Loading Win32 DLLs into the NTAPI native runtime.
- Replacing Seed allocators/containers/async machinery with unrelated standard
  abstractions without checking ownership and context behavior.
- Bypassing CTTI/RTTI with compiler RTTI for a reflected interface.
- Declaring ordinary native callbacks/helpers noexcept despite interruption.
- Treating disposal as if its refcount were still positive.
- Moving domain service registration into Core merely because it depends on Core.
- Relying on inheritance alone to emit a retained linker descriptor.
- Disabling certificate validation by default or copying keys into ordinary memory.
- Broad privilege requests at startup for operations that are not active.
- Generating catalogs or scripts that mention projects absent from this checkout.
- Reusing metadata fixtures from a different producing runtime.
- Claiming protocol/release support from compilation alone.
