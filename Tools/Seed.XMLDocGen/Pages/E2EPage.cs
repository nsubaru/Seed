namespace Seed.XMLDocGen;

internal sealed class E2EPage : ISourcePage
{
    public string FileName => "e2e.md";

    public string Title => "KMS E2E test authoring";

    public string Render(PageContext context) => """
        # KMS E2E test authoring

        Testing/Seed.E2ETests adapts the generic E2E lifecycle pattern to the
        shipped local KMS protocol. Lib/Seed.E2E.Infrastructure owns runtime startup,
        command completion, artifact directories, deadlines, fast reboot and shutdown.
        The tests use xUnit v3 with Microsoft Testing Platform, independently of the
        native self-test runner and the Visual Studio native test adapter.

        ```powershell
        ./Tools/Test-SeedE2E.ps1 -Configuration Debug
        ./Tools/Test-SeedE2E.ps1 -Configuration Release -NoBuild -Class CommandTests -Method StatusHasStableLabelsAndFastRebootKeepsTheProcess
        ```

        Each test starts an owned native process with a fresh configuration directory.
        A per-account file lease shared with the native test runner serializes the
        fixed loopback ports. No existing developer runtime is attached or stopped.
        Startup rejects an already existing default KMS endpoint rather than taking
        ownership of another session; run in an isolated account/session if needed.
        Commands have bounded deadlines, consume the terminal command-ready marker
        and retain complete output in commands.log. Disposal requests shutdown and
        terminates only its owned process if the cleanup deadline expires.

        Derive a test class from E2ETest<YourTestClass>, place each independent topic
        in its own source file, and use Runtime.CommandAsync. Supply expectError only
        for an intentionally rejected request. Assert stable labels and semantic
        values rather than exact times, memory counts or machine-specific interfaces.
        Use Runtime.Directory for generated files; read the file and assert its
        contents after a command says it was saved.

        Fast reboot is exposed as reload. KmsClient waits for a changed status.kernel
        composition and reconnects before command completion. Runtime.FastRebootAsync
        asserts that this happened while retaining the process.
        The repeated-reboot regression also verifies that unsaved changes disappear,
        explicit saves survive, and KMS is usable in the new composition. The test
        checks behavior rather than making a performance claim.

        Coverage includes all 26 shipped commands: discovery/help/version, exit,
        status/shutdown/reload, configuration list/get/set/save/reset, log view/get/
        set/follow, native test list/run/report, RTTI list/export and networking.
        Native selections include overlapping wildcard and exact-name requests and
        rejected unmatched/unterminated selections. Logging follow verifies bounded
        cancellation and subsequent connection usability.

        CancellationTests closes or cancels an executing full native suite, reconnects
        to the same composition, rejects a partial/stale report and completes a fresh
        selected run. A native cancellation-boundary test verifies that a pre-canceled
        run starts no cases and cancellation from one case prevents the next case,
        without publishing a pack-completion callback. ReloadTests covers real
        interactive and one-command console processes and saved/unsaved pipe-name
        changes. Premature pipelined input closes an active command session instead
        of dispatching the extra request. HttpLifecycleTests fills the listener with
        incomplete requests, confirms its connection limit, completes that work and
        requires the next request to succeed. Completed work must be reclaimed after
        accept as well as before waiting for another connection. Add independent test
        topics in separate source files.

        DNS checks missing-argument rejection without an external resolver dependency;
        controlled resolution is covered by native DNS tests. DHCP
        checks idle status, invalid interface selection and a stop with no operation.
        These checks do not validate real lease acquisition or mutate interfaces;
        lease lifecycle and rollback belong in a separately isolated network fixture.

        Reports, transcripts and configuration snapshots remain under Artifacts/e2e.
        The workflow fails an empty selection and preserves TRX output on failures.
        CI runs E2E and native adapter verification under its ordinary test account.
        Tools/Test-SeedDocker.ps1 runs the same suite in an ordinary-user Windows
        container, with reports copied to Artifacts/docker before cleanup.
        See [Docker builds](docker.md), [native testing](testing.md) and [test adapter](test-adapter.md).
        """;
}
