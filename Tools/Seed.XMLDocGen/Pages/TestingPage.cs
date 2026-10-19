namespace Seed.XMLDocGen;

internal sealed class TestingPage : ISourcePage
{
    public string FileName => "testing.md";

    public string Title => "Native and managed verification";

    public string Render(PageContext context) => """
        # Native and managed verification

        Build Seed.slnx with MSBuild /m for matching x64 configuration outputs.
        Run local PowerShell verification helpers in PowerShell 7: integration uses
        the NativeLibrary API, which Windows PowerShell 5.1 does not provide.
        The solution stages Seed.Starter.dll and its PDB beside Seed.Managed.TestRunner.
        Standard tests run under a non-elevated account; the runner checks its token
        and rejects an elevated session.

        The console tests.run command accepts several exact reflected names or
        wildcard patterns in one run. Quote a selection that contains spaces.
        Overlapping selections run each matching test once; any unmatched selection
        fails before execution. tests.result.store saves the resulting NUnit report.
        Disconnect or caller cancellation stops a command run between complete cases.
        The current case finishes cleanup; a canceled run publishes no complete pack
        or NUnit report. Tests must use bounded IO and waits because cancellation does
        not forcibly terminate a case. The native CancellationBoundaries regression
        checks pre-cancellation, case-boundary cancellation and completion callbacks.

        Tools/Invoke-SeedWorkflow.ps1 -Action Tests -NoBuild runs all shipped native
        self-tests. -Wildcard selects a subset, such as *Crypt*. The managed runner's
        --test option accepts an exact native RTTI test name. Its bootstrap path launches
        an isolated native test run and checks the resulting NUnit file; zero selected
        tests or failed/interrupted results fail the invocation. A test returning false
        is an intentional skip, such as an unavailable hardware instruction path;
        these skips are counted separately and remain visible in NUnit output.
        Native runner invocations under the same Windows account share a file lease
        because the current network self-tests use fixed loopback ports.

        [KMS E2E tests](e2e.md) use the same per-account lease and cover all shipped
        commands, including repeated Core fast reboot through reload.
        [The Visual Studio native adapter](test-adapter.md) provides Test Explorer
        discovery, execution and native debugger attachment with a local NuGet package.
        [Windows Docker builds](docker.md) provide source-build, test, runtime and
        artifact targets; container tests run as ContainerUser and preserve reports.

        Tools/Test-SeedIntegration.ps1 exercises the actual local KMS path, cryptography
        commands, NUnit report saving, live RTTI export, interface queries, HTTP health
        and echo, chunked requests and invalid framing, reflected managed/native JSON
        and binary exchange, configuration
        validation and persistence across reload, live logger masks and log events,
        streaming cancellation and early disposal through Seed.KMS.Verifier, malformed packets,
        reconnect and native loaded-module checks. It shuts down the process it owns.
        DHCP checks exercise idle status and rejected interface selections; they do
        not acquire leases or change network interfaces. Native tests also cover
        maintenance startup/shutdown boundaries and timer normalization.

        Tools/Test-SeedConfigurationStartup.ps1 starts with a valid saved snapshot,
        then checks invalid values, identity/version mismatches, truncation and extra
        bytes. Rejected configuration files remain unchanged.
        Pass -FixturePath with the core.v1.bin path printed by integration. Integration
        accepts -ConfigurationDirectory to select its saved configuration directory;
        Invoke-SeedValidation.ps1 passes a fresh directory and its exact fixture path
        between these steps for each run and build configuration.

        Tools/Test-SeedHostConfiguration.ps1 checks pending versus saved settings,
        completed reload, HTTP port/disable behavior, changed local pipe names and
        persisted logger masks. Valid and malformed reflected host envelopes run
        through ordinary native startup, including oversized lengths/counts, bad
        identities, truncation and invalid ports. Rejected files remain unchanged.

        Tools/Test-SeedRsaSigning.ps1 runs native signing known answers, malformed
        key/digest rejection and concurrent signing with shared identities. It then
        independently verifies 18 native signatures with .NET RSA across 2048,
        3072 and 4096 bits, all three SHA-2 digests and PKCS#1 v1.5/PSS padding;
        modified signatures must fail. The source fixtures contain deliberately
        public test-only keys generated locally with .NET and independent PKCS#1
        signatures. They must never be used as server identities. Private fixtures
        are decoded directly into sensitive storage. This is interoperability
        evidence, not completion of the cryptographic release review.

        Tools/Test-SeedTlsIdentity.ps1 runs four native identity tests and a
        fragmented client/accepted TCP round trip. Identity tests cover key matching,
        all RSA handshake signature schemes, TLS 1.3 PKCS#1 rejection, independent
        snapshot ownership, strict DER/PEM, malformed chains, expired/wrong-purpose
        certificates, file loading, write-sharing denial and sensitive read clearing.
        Its independent .NET verifier checks the fixture chain and matching key.
        Ordinary self-tests also exercise file loading in a fresh temporary directory.

        Tools/Test-SeedTlsClient.ps1 starts independent .NET TLS 1.2 and TLS 1.3 peers.
        It checks fragmented application data, valid RSA chains, DNS and IPv4/IPv6
        identities, redirects, informational/chunked responses, invalid HTTP framing,
        hostname/root/expiry failures and incompatible protocol versions. Successful native HTTPS processes
        report only Seed.Runtime.exe and ntdll.dll as loaded modules. This test tool
        uses .NET/Schannel in its own process, outside the native runtime.
        The peer builds its server chain with its generated root as an explicit
        in-memory trust anchor; it does not install that root in Windows stores.

        The managed round-trip example also checks metadata ownership and rejects
        selected malformed headers, pointer-table addresses and origins. These checks
        do not constitute validation of every nested CTTI structure in arbitrary files;
        use metadata exported from the matching, trusted Seed runtime.

        TLS server interoperability has .NET and OpenSSL checks through
        Tools/Test-SeedTlsServer.ps1, including handshake messages split across TLS
        records and TCP writes, modified protected records and reload with stalled IO.
        Exhaustive hostile TLS/Finished/key-update checks,
        isolated DHCP lifecycle/rollback and clean-machine
        supported-baseline validation remain release acceptance work.
        """;
}
