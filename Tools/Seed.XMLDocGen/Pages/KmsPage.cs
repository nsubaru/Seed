namespace Seed.XMLDocGen;

internal sealed class KmsPage : ISourcePage
{
    public string FileName => "kms.md";

    public string Title => "Console transport and commands";

    public string Render(PageContext context) => """
        # Console transport and commands

        Seed.KMS.Server owns the local Seed.KMS.v1 message pipe and a Core transport
        thread. The runtime composition starts it after Core services and joins it
        before service teardown. Console disconnection closes the session; runtime
        ownership is independent of a console connection.

        The endpoint serves one session at a time. Exit an interactive console
        before connecting another client or invoking a separate one-command console.

        Connections use ordinary Windows object permissions and reject remote clients.
        The server first sends a command-request packet containing SEED_KMS/1. There
        is no login exchange. Packets contain five header bytes followed by bounded
        UTF-16LE text without a terminator. The maximum complete packet is 65,535 bytes.
        Malformed headers, odd payload sizes, invalid surrogate pairs, null characters
        and oversized messages close the session. Datagram output can span packets.

        Seed.KMS.Transport provides a sequential managed KmsClient. Seed.Console supports
        an interactive prompt and one-command execution. Ctrl+C cancels transport IO
        and closes that console session. A new KmsClient creates a new session after
        a disconnect. The managed transport retains argument-request/answer mechanics;
        current commands take arguments directly on the command line.
        --start followed by the runtime executable path launches Seed through its
        NT startup adapter before connecting. The optional remaining arguments form
        one command. Console exit leaves the started runtime running.
        --pipe selects a configured local endpoint before --start or a command.

        Fast reboot uses reload. KmsClient keeps its command gate until the old
        session closes, a new Seed initialization marker arrives and status.kernel
        reports a changed composition. The console prints its next prompt, or exits
        a one-command reload successfully, only after this reconnect completes.
        The command is never replayed during reconnect. A one-minute reload deadline
        and caller cancellation bound the wait; failure closes the incomplete session.
        configurations.get host.kms.pipeName includes a Next KMS endpoint line naming
        the saved startup endpoint. Reload follows that name, including a saved pipe
        rename; unsaved pipe edits leave the saved endpoint unchanged.

        Implemented commands:

        | Purpose | Commands |
        |---|---|
        | Session | help, commands, version, exit |
        | Runtime | status.kernel, shutdown, reload |
        | Configuration | configurations.list, .get, .set, .save, .reset |
        | Logs | logging.view, .get, .set, .follow |
        | Tests | tests.list, tests.run, tests.result.store |
        | Reflection | di.rtti.list, di.rtti.save |
        | Networking | network.interfaces, network.dns.resolve |
        | DHCP | network.dhcp.start, .status, .stop |
        | Files | fs.list, .info, .read, .write, .directory.create, .copy, .move, .delete |
        | Processes | process.list, process.terminate |
        | Windows registry | registry.list, .get, .set, .key.create, .key.delete, .value.delete |

        See [system command examples](system-commands.md) for syntax, quoting,
        filesystem limits and registry types. These commands call Seed IO and NTAPI
        using the native process's existing Windows token. They do not request elevation.

        tests.run accepts several wildcard patterns or exact native test type names.
        With no selection it runs all registered tests.
        All selections must match; overlapping selections execute each test once.
        A successful run reports pass and skip counts; a failed run sends an error
        datagram. tests.result.store
        writes the most recent run's NUnit XML to an explicit ordinary-file path.
        Reports preserve status, duration, exception type and module-relative stack
        addresses when the test captured them. di.rtti.save exports the live, fixed-up
        Seed RTTI section. logging.view returns the last 100 buffered log lines.
        logging.get lists dynamic logger names and numeric masks, or reads one named
        logger. logging.set takes a mask followed by the full logger name, including
        spaces. Bits 0..4 enable Debug, Verbose, Warning, Error and Critical respectively;
        masks range from 0 to 31. Live changes remain pending host overrides until
        configurations.save; reload applies the saved overrides.

        logging.follow takes no arguments and streams new output after a ready
        datagram. Core log events wake the transport thread; the event callback does
        no pipe IO or log reading. The thread reads bounded log pages and monitors a
        pending pipe read to detect disconnection. Canceling or leaving the managed
        enumeration closes the session, drains pending IO and releases the subscription.
        Use logging.view separately to read historical output.
        ExecuteStreamingAsync yields output immediately without collecting it;
        ExecuteAsync collects a finite command response with a 4 MiB limit. Abandoning
        an unfinished stream requires a new KmsClient connection.

        Finite commands execute on an owned Core worker while the transport monitors
        a pending read. Disconnect, premature pipelined input or endpoint shutdown
        signals the worker's cancellation token. The endpoint joins the worker and
        drains the native read before admitting another session. Command callbacks
        observe cancellation at safe boundaries and retain owned IO until completion.
        tests.run checks before and after each case: the current case finishes its
        cleanup and no following case starts. A canceled run has no completed NUnit
        report and clears the previous report once valid execution starts. DNS finishes
        its owned request through the existing IO deadline before reporting cancellation.
        Applied configuration edits and completed file writes are not rolled back by
        disconnect. Runtime-owned service lifetimes never use the session token.

        network.dhcp.start takes one explicit interface index and returns after the
        runtime requests acquisition. network.dhcp.status reports every operation,
        including acquisition errors and retained cleanup. network.dhcp.stop takes an
        interface index, cancels its worker and joins it before releasing the operation.
        Console disconnection does not stop a requested lease. The runtime stops all
        maintenance workers before tearing down their DHCP/network dependencies.
        No worker starts automatically; ordinary integration checks use only rejected
        interface selections and idle status.

        configurations.list includes reflected host settings alongside early Core
        controls. Host edits identify their save/reload requirements. status.kernel
        supplies a composition identifier that changes after reload has completed.
        Independent privileged DHCP integration remains acceptance work.
        """;
}
