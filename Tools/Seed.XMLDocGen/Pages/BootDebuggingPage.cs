namespace Seed.XMLDocGen;

internal sealed class BootDebuggingPage : ISourcePage
{
    public string FileName => "boot-debugging.md";

    public string Title => "Debug native Seed during Windows boot with WinDbg";

    public string Render(PageContext context) => """
        # Debug native Seed during Windows boot with WinDbg

        This is a manual test-VM workflow for a native user-space executable observed
        through Windows kernel debugging. Seed does not become a Windows kernel driver.
        Keep the exact Seed.Runtime.exe and private PDB from the same build; if using
        linker-map diagnostics, retain the map from that same link as well.

        Prepare a disposable x64 VM with a recoverable snapshot, a configured KDNET
        connection, and a native boot-launch arrangement that starts Seed.Runtime.exe.
        Supply a writable configuration location in that VM. Windows Session Manager's
        BootExecute is a possible native-image launch point; edit it only in the test
        VM while preserving its existing entries and a recovery route. The managed
        console and Seed.Starter are post-boot adapters and must not be used as native
        BootExecute images. This repository does not configure the host's boot settings.

        Boot execution of the full networking composition is not yet a validated
        support target: AFD, NSI and network configuration availability depend on the
        launch stage. Capture the actual startup failure if an early dependency is
        unavailable. Ordinary-user post-boot startup remains the standard validation.

        Attach WinDbg from the debugging host to the VM using its configured transport,
        port and key before the image starts. Add private symbols/source directories
        using paths on the debugging host, for example:

        ```text
        .symfix C:\Symbols
        .sympath+ P:\Sources\Seed\Artifacts\x64\Debug\Runtime
        .srcpath+ P:\Sources\Seed\Native
        !gflag +ksl
        sxe ld:Seed.Runtime.exe
        g
        ```

        ksl is the documented kernel-symbol-loading flag. Private PDBs and the source
        path provide source debugging; the historical ksls spelling is not a separate
        source-debugging flag. At the image-load break, inspect the current stack. If
        still in process creation, use gu as needed to return to a stable boundary.
        Identify the Seed EPROCESS with !process 0 0 (or !process 0 0 Seed.Runtime.exe),
        then request invasive process context:

        ```text
        .process /i <Seed EPROCESS address>
        g
        .reload /user
        lm
        x <module-name-from-lm>!*
        ```

        The required g activates the requested process and breaks back into the
        debugger. Use the module name WinDbg reports, which can differ from the
        filename spelling. Set bu <module-name-from-lm>!EntryPoint or a matching Core
        initialization symbol found with x, then continue. Keep track of process
        context after resuming; it is not permanent across arbitrary execution.
        For inspection without invasive switching, .process /p /r <EPROCESS> translates
        user pages and reloads user symbols. Do not substitute it for the activation
        needed to set user-space breakpoints in a live kernel session.

        Use sxe av for access violations, !gflag +soe when investigating stop-on-exception
        behavior, !sym noisy for symbol-load failures, !peb for image/environment state,
        and k / kp for the active stack. Verify the exact image/PDB identity and inspect
        loaded modules before interpreting missing symbols. Threadable interruption and
        the custom exception ABI can produce expected debugger events; distinguish
        those from the failure being investigated.

        Resume the VM and restore its debugging/boot configuration or revert the
        snapshot when finished. Do not apply test-VM boot changes to the development
        host. See [Microsoft process contexts](https://learn.microsoft.com/en-us/windows-hardware/drivers/debuggercmds/-process--set-process-context-)
        and [the ksl flag](https://learn.microsoft.com/en-us/windows-hardware/drivers/debugger/enable-loading-of-kernel-debugger-symbols).
        """;
}
