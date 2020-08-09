// This is an independent project of an individual developer. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include "Seed.Core.Math.SafeInt.h"

#define SEED_RT_ALLOW_INTERNAL_NT_API
#include "Seed.Core.Base.NTAPI.h"

unsigned __int64 Seed::Core::Math::Internal::SafeIntBase::_umul128Impl(unsigned __int64 Multiplier, unsigned __int64 Multiplicand, unsigned __int64* HighProduct) {
    return _umul128(Multiplier, Multiplicand, HighProduct);
}
