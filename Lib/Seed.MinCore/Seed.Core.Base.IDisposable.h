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
}