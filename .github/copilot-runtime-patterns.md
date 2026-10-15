# Seed runtime patterns

Exceptions derive from the local runtime exception family and preserve
SourceLocation, clone/throw behavior and exception-heap string ownership. Add
natvis for new debugger-heavy exception and ownership types.

Services use IDisposable and the RefCountClass family with a private Data block
when implementation belongs in a `.cpp`. Prefer RefCountClassOutsideHeader and
RefCountClassOutside. Declare reflected callable members with CttiClassRecord.
Core services register through ICoreBootUpService; other components use
IBootUpService in their own instantiation unit.

The RefCountClass destructor calls Dispose after the refcount reaches zero.
Do not increment that count at disposal entry. Cleanup that touches alias or
weak-reference wrappers can reenter Dispose; mark exclusive disposal with
RefCountT::InvalidRefCountValue inside the owning lock/state gate.

Linker registration needs a retained section entry. Put the forced-reference or
explicit template-instantiation carrier after the complete concrete type in its
owning instantiation unit. Verify exactly one entry in the MAP. Disabling debug
information in heavy instantiation units is intentional.

Native test groups are leaves: a namespace containing registered tests must not
also contain nested namespaces with registered tests. Use a sibling group when
extending a component whose existing tests already occupy the parent namespace.
The test-discovery structure check enforces this convention.

Core initialization owns foundational stages and teardown. Applications compose
their selected services after DI is ready and stop them before DI teardown.
Preserve shutdown ordering, context, diagnostics and interruption behavior.

## Refcounted value class

This header/implementation pair shares an owning string. Copies share `Data`;
moves transfer its pointer. The macro supplies the copy/move operations and
destructor, so do not add another destructor or delete `This` in `Dispose()`.
Put the files in the owning component and add both to its shared items.

```cpp
// Example.SharedLabel.h
#pragma once
#include "Seed.Core.String.h"

namespace Seed::Examples
{
    /// <summary>
    /// Shares an immutable owning label between copied value wrappers.
    /// </summary>
    class SharedLabel final : public Core::IDisposable {
    private:
        struct Data;
        Data* This;

    protected:
        void Dispose() override;

    public:
        /// <summary>
        /// Copies the supplied text into shared owning storage.
        /// </summary>
        /// <param name="value">Text copied before this constructor returns.</param>
        explicit SharedLabel(Core::ConstString value);

        /// <summary>
        /// Copies the shared label into an owning string.
        /// </summary>
        /// <returns>Text that remains valid independently of this wrapper.</returns>
        Core::String GetText() const;

    public:
        RefCountClassOutsideHeader(SharedLabel);
    };
} // namespace Seed::Examples
```

```cpp
// Example.SharedLabel.cpp
#include "Example.SharedLabel.h"

namespace Seed::Examples
{
    struct SharedLabel::Data {
        Core::RefCountT RefCount;
        Core::String Value;

        Data(Core::ConstString value) : RefCount(1),
                                        Value(value) {
        }
    };

    RefCountClassOutside(SharedLabel);

    SharedLabel::SharedLabel(Core::ConstString value) : This(new Data(value)) {
    }

    void SharedLabel::Dispose() {
    }

    Core::String SharedLabel::GetText() const {
        return This->Value;
    }
} // namespace Seed::Examples
```

`Dispose()` runs after `RefCount` reaches zero. Here the generated destructor
deletes `Data`, which releases its string; there is no extra resource to stop.
For an IO owner, cancel/join pending work before releasing its buffers. If cleanup
can release an alias/weak wrapper that reenters disposal, claim exclusive disposal
under its lock/state gate and set `RefCountT::InvalidRefCountValue` there. Do not
apply the sentinel or invent refcount increments for ordinary independent fields.
Do not invoke methods on a moved-from wrapper. Add natvis when the new wrapper
needs a useful debugger view; [RsaSigner](../Native/Seed.Core.Crypt.RsaSigner.h)
and [TcpByteStream](../Native/Seed.Core.IO.Network.ByteStream.h) are shipped examples.

## Exception class

Derive from `Core::Exception`, keep diagnostic text in the exception heap, clone
the concrete type and throw through the runtime macro. This example inherits
the base's diagnostic state and formatting rather than allocating a regular
runtime string in the exception. No ordinary method is declared `noexcept`.

```cpp
// Example.FailureException.h
#pragma once
#include "Seed.Core.Exceptions.Base.h"

namespace Seed::Examples
{
    /// <summary>
    /// Reports a failed demonstration operation with its source diagnostics.
    /// </summary>
    class FailureException final : public Core::Exception {
    public:
        /// <summary>
        /// Captures the failure message and the location where it occurred.
        /// </summary>
        /// <param name="location">Source position of the failed operation.</param>
        /// <param name="message">Message copied into exception-owned storage.</param>
        FailureException(Core::SourceLocation location, Core::ConstString message) : Exception(location, message) {}

        /// <summary>
        /// Copies the failure and its inherited diagnostic state.
        /// </summary>
        /// <param name="other">Failure whose diagnostic state is retained.</param>
        FailureException(const FailureException& other) : Exception(other) {}

        /// <summary>
        /// Transfers the failure and its inherited diagnostic state.
        /// </summary>
        /// <param name="other">Failure supplying the diagnostic state.</param>
        FailureException(FailureException&& other) : Exception(Core::MoveRValue(other)) {}

        /// <summary>
        /// Formats the inherited failure diagnostics on the exception heap.
        /// </summary>
        /// <returns>Exception-owned diagnostic text.</returns>
        Core::ExceptionString toString() const override {
            return Exception::toString();
        }

        /// <summary>
        /// Copies this concrete failure for runtime exception ownership.
        /// </summary>
        /// <returns>An owning holder for the copied concrete exception.</returns>
        Core::ExceptionHolderType Clone() const override {
            return Core::ExceptionHolderType(new FailureException(*this));
        }

        /// <summary>
        /// Throws this concrete failure through Seed's exception ABI.
        /// </summary>
        [[noreturn]] void ThrowMe() override;

        /// <summary>
        /// Releases the inherited exception-owned diagnostic state.
        /// </summary>
        ~FailureException() override {}
    };
}
```

```cpp
// Example.FailureException.cpp
#include "Example.FailureException.h"

void Seed::Examples::FailureException::ThrowMe() {
    Throw(*this);
}
```

At a call site, use `Throw(FailureException(SourcePosition, L"Operation failed"))`
and catch the Seed exception family. Dynamic extra fields must use
`Core::ExceptionString`, with copy/move/clone behavior that retains those fields.
If assignment is needed, follow the concrete neighboring exception's complete
reconstruction pattern, including self-assignment handling. Preserve `RawException`
when transferring a failure through async payloads. Add a matching natvis entry;
see [OperationTimeoutException](../Native/Seed.Core.IO.Exceptions.OperationTimeoutException.h)
and [exception visualizers](../Native/Seed.Core.Exceptions.natvis).

## Service and IoC dependency helper

A service is also a refcounted value wrapper. Its constructor declares the
dependencies it receives, while the separate registration helper declares the
same types in the same order for IoC. The example below reports the number of
active runtime mappings using the shipped `RuntimeManagementService`.

```cpp
// Example.ImageCountService.h
#pragma once
#include "Seed.Core.Services.Base.h"
#include "Seed.Core.Services.RuntimeManagementService.h"

namespace Seed::Examples
{
    /// <summary>
    /// Reports the number of active Seed runtime image mappings.
    /// </summary>
    class ImageCountService final : public Core::IDisposable {
    public:
        CttiClassRecord(
            ImageCountService,
            NoCttiInterfaces(),
            NoCttiConstructors(ImageCountService),
            CttiMethodGroup(CttiMethodRecord(ImageCountService, GetImageCount)),
            CttiProperties(),
            CttiStaticMethodGroup()
        )

    private:
        struct Data;
        Data* This;

    protected:
        void Dispose() override;

    public:
        /// <summary>
        /// Retains the service that enumerates active runtime mappings.
        /// </summary>
        /// <param name="runtimeManagement">Dependency supplying current runtime mappings.</param>
        explicit ImageCountService(Core::Services::RuntimeManagementService runtimeManagement);

        /// <summary>
        /// Counts the mappings in an owning runtime-instance snapshot.
        /// </summary>
        /// <returns>Number of mappings in the captured snapshot.</returns>
        Core::SizeT GetImageCount() const;

    public:
        RefCountClassOutsideHeader(ImageCountService);
    };
}
```

```cpp
// Example.ImageCountService.cpp
#include "Example.ImageCountService.h"

using namespace Seed;
using namespace Seed::Examples;

struct ImageCountService::Data {
    Core::RefCountT RefCount;
    Core::Services::RuntimeManagementService RuntimeManagement;

    Data(Core::Services::RuntimeManagementService runtimeManagement) : RefCount(1),
                                                                        RuntimeManagement(Core::MoveRValue(runtimeManagement)) {
    }
};

RefCountClassOutside(ImageCountService);

ImageCountService::ImageCountService(Core::Services::RuntimeManagementService runtimeManagement)
    : This(new Data(Core::MoveRValue(runtimeManagement))) {}

void ImageCountService::Dispose() {}

Core::SizeT ImageCountService::GetImageCount() const {
    auto instances = This->RuntimeManagement.GetActiveRuntimeInstances();

    return instances.Count;
}
```

The non-Core component's instantiation unit owns this helper and its retained
descriptor carrier. The helper expresses constructor dependencies; the explicit
`IGeneralBootUpService` instantiation forces the `.DIBT$m` section entry to survive.
Do not assume that inheriting the CRTP helper alone instantiates its inline statics.

```cpp
// Example.Instantiations.cpp
#include "Example.ImageCountService.h"
#include "Seed.Core.Services.BootupServicesManager.h"

using namespace Seed;
using namespace Seed::Examples;

/// <summary>
/// Declares the image-count service and its ordered IoC constructor dependencies.
/// </summary>
class ImageCountServiceDiRegistration final : 
    protected Core::Services::IBootUpService<
        ImageCountService,
        Core::Services::RuntimeManagementService> {
};

template class Seed::Core::Services::IGeneralBootUpService<false, Seed::Examples::ImageCountService, Seed::Core::Services::RuntimeManagementService>;

template Seed::Examples::ImageCountService Seed::Core::DI::DependencyInjection::Resolve<Seed::Examples::ImageCountService>(bool);
template Seed::Core::RTTI::RttiTypeRecord TypeRecord<Seed::Examples::ImageCountService>();
```

Use `ICoreBootUpService` and `IGeneralBootUpService<true, ...>` only for a genuine
Core-owned service in Core's instantiation unit. Use `IBootUpService` and `false`
for other components. Configure heavy instantiation units with debug information
disabled, then verify exactly one descriptor for the service in the linked MAP.
Resolve through the owning `DI::IDI` after registration, and stop owned workers
before dependency teardown. Services with runtime-entry-context cleanup implement
`IRuntimeShutdownService`; [DHCP maintenance](../Native/Seed.Core.Services.Dhcp.LeaseMaintenance.h)
shows that contract and its cancellation/join ordering.
