namespace Seed.XMLDocGen;

internal sealed class RuntimePage : ISourcePage
{
    public string FileName => "runtime.md";

    public string Title => "Runtime and source SDK";

    public string Render(PageContext context) => """
        # Runtime and source SDK

        The native source SDK is divided into MinCore, Core, HTTP, TLS and KMS shared items.
        Core initialization is separate from runtime composition. The runtime entry
        point registers services, starts them, waits for shutdown and stops them before
        DI teardown. Exceptions use the custom runtime ABI and carry diagnostics.

        Containers, smart pointers and refcounted values use Seed allocator and lifetime
        rules. Dispose begins at refcount zero; cleanup that can reenter disposal uses
        the named invalid refcount sentinel under the owning state gate.

        Async work uses Seed tasks, threadables and the unified thread pool. Preserve
        payload cleanup, MarkFinished ordering, context propagation and interruption.
        An ordinary noexcept boundary can terminate the process during interruption.

        Reflected records use CTTI declarations. RTTI descriptors and service/test
        registrations require retained linker-section entries from their owning
        instantiation units. Native binary layout and metadata depend on the producing
        runtime version; there is no stable binary ABI promise in version 0.1.

        Sources: [component map](components.md), [agent conventions](../AGENTS.md).
        """;
}
