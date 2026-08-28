#pragma once
#include "Seed.Core.Base.h"

#ifndef SEED_RT_ALLOW_INTERNAL_NT_API
#define SEED_MEMORY_PREFETCH_UNDEFINE_INTERNAL_NT_API
#define SEED_RT_ALLOW_INTERNAL_NT_API
#endif

#include <xmmintrin.h>

#ifdef SEED_MEMORY_PREFETCH_UNDEFINE_INTERNAL_NT_API
#undef SEED_RT_ALLOW_INTERNAL_NT_API
#undef SEED_MEMORY_PREFETCH_UNDEFINE_INTERNAL_NT_API
#endif

namespace Seed::Core::Memory
{
    /// <summary>
    /// Requests that the cache line containing <paramref name="address"/> is prefetched into L1.
    /// The address must already be validated and remain readable for the duration of the hint.
    /// </summary>
    template<typename T>
    ForceInline void PrefetchToL1(const T* address) noexcept
    {
        if (address != nullptr)
        {
            _mm_prefetch(reinterpret_cast<const char*>(address), _MM_HINT_T0);
        }
    }
}
