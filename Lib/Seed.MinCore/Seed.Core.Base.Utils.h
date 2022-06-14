#pragma once
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"
#include "Seed.Core.Base.Swap.h"

namespace Seed::Core
{
    /// <summary>
    /// Changes the order of elements in "Array" array
    /// </summary>
    /// <param name="Array">Array</param>
    /// <param name="size">Number of elements</param>
    template<typename T>
    DeprecatedCall(L"Working with C-shaped arrays") __forceinline void Reverse(T* Array, SizeT size) {
        for (SizeT i = 0; i < size / 2; i++) {
            Swap(Array[i], Array[(size - 1) - i]);
        }
    }

    /// <summary>
    /// Changes the order of elements in "Array" array
    /// </summary>
    /// <param name="Array">Array</param>
    /// <param name="size">Number of elements</param>
    template<typename T, SizeT TN>
    ForceInline constexpr void Reverse(T(&array)[TN]) {
        for (SizeT i = 0; i < TN / 2; i++) {
            Swap(array[i], array[(TN - 1) - i]);
        }
    }

    /// <summary>
    /// "Compile-time" - maximal finding function between two objects
    /// </summary>
    /// <typeparam name="T">Arguments type</typeparam>
    /// <param name="v1">First argument</param>
    /// <param name="v2">Second argument</param>
    /// <returns>The greatest argument</returns>
    template<typename T>
    __forceinline constexpr T Max(const T& v1, const T& v2) { return (v1 > v2) ? v1 : v2; }

    /// <summary>
    /// "Compile-time" minimal finding function between two objects
    /// </summary>
    /// <typeparam name="T">Arguments type</typeparam>
    /// <param name="v1">First argument</param>
    /// <param name="v2">Second argument</param>
    /// <returns>The least argument</returns>
    template<typename T>
    __forceinline constexpr T Min(T v1, T v2) { return (v1 < v2) ? v1 : v2; }
}