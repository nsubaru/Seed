#pragma once
#include "Seed.Core.Base.Definitions.h"
#include "Seed.Core.Base.Types.h"

namespace Seed::Core
{
    /// <summary>
    /// Class resources disposing interface with reference count
    /// </summary>
    VirtualClass IDisposable
    {
    public:
        [[msvc::noop_dtor]] virtual ~IDisposable() = 0 {};

    protected:
        /// <summary>
        /// A method for resource disposing, call automatically by destructor in classes with reference count
        /// </summary>
        virtual void Dispose() = 0;
    };

    /// <summary>
    /// Extended class resources disposing interface with reference count
    /// </summary>
    VirtualClass IExtendedDisposable
    {
    public:
        [[msvc::noop_dtor]] virtual ~IExtendedDisposable() = 0 {};

    protected:
        /// <summary>
        /// A method for resource disposing, call automatically by destructor in classes with reference count
        /// </summary>
        /// <returns>
        /// If true is returned then object data will be deleted, otherwise it remain after destruction of object completes
        /// </returns>
        virtual bool Dispose() = 0;
    };
}

namespace Seed::Core::Concepts
{
    template<typename T>
    concept SimpleDisposable = requires (T & obj) {
        { obj.Dispose() } -> Void;
    };

    template<typename T>
    concept ExtendedDisposable = (requires (T & obj) {
        { obj.Dispose() } -> IsSame<bool>;
    } && !SimpleDisposable<T>);

    template<typename T>
    concept IsDisposable = SimpleDisposable<T> || ExtendedDisposable<T>;
}