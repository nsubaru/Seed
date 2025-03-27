#pragma once
#include "Seed.Core.Base.TypeTraits.Base.h"

/// <summary>
/// Compile-time verifications namespace
/// </summary>
namespace Seed::Core::TypeTraits
{
    /// <summary>
    /// TypeTrait for checking is type T is task
    /// </summary>
    /// <typeparam name="T">Type for checking</typeparam>
    template<typename>
    struct TaskT: LogicFalse {
        using T = void;
        using Payload = void;
    };

    /// <summary>
    /// TypeTrait for checking is type T is task
    /// </summary>
    /// <typeparam name="T">Type for checking</typeparam>
    template<typename T>
    constexpr bool IsTaskV = TaskT<T>::State;

    /// <summary>
    /// TypeTrait for checking is type T is void task (task that does not hold value)
    /// </summary>
    /// <typeparam name="T">Type for checking</typeparam>
    template<typename T>
    constexpr bool IsVoidTaskV = TaskT<T>::State && IsVoidV<typename TaskT<T>::T>;

    /// <summary>
    /// TypeTrait for checking is type T is non void task (task that hold value)
    /// </summary>
    /// <typeparam name="T">Type for checking</typeparam>
    template<typename T>
    constexpr bool IsNonVoidTaskV = TaskT<T>::State && !IsVoidV<typename TaskT<T>::T>;

    /// <summary>
    /// Constexpr for checking is type T is task that hold value of type U
    /// </summary>
    /// <typeparam name="T">Type for checking</typeparam>
    /// <typeparam name="U">Requested value type</typeparam>
    template<typename T, typename U>
    constexpr bool IsTypedTaskV = TaskT<T>::State && IsSameV<typename TaskT<T>::T, U>;
}