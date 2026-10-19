namespace Seed.XMLDocGen;

internal sealed class DemonstrationsPage : ISourcePage
{
    public string FileName => "demonstrations.md";

    public string Title => "Run the native and managed demonstrations";

    public string Render(PageContext context) => """
        # Run the native and managed demonstrations

        Build Seed.slnx first. Run these examples from the checkout root in an
        ordinary, non-elevated PowerShell session. Use PowerShell 7 for the local
        integration workflow described below. The paths below select Debug;
        substitute Release consistently when using that build. Start only one
        demonstration host with the default pipe and ports at a time.

        ## Start a runtime and connect its console

        ```powershell
        ./Artifacts/x64/Debug/Seed.Console/Seed.Console.exe --start ./Artifacts/x64/Debug/Runtime/Seed.Runtime.exe status.kernel
        ```

        Seed.Console invokes the external Seed.Starter adapter, waits for the local
        KMS endpoint, prints status and disconnects. The native process owns its
        services independently of the console. To open an interactive prompt on
        the running host, invoke `Seed.Console.exe` without arguments. `exit` disconnects;
        `shutdown` stops the runtime. KMS serves one session at a time: close an
        interactive console before using the one-command examples below.

        Connect to an existing host by omitting `--start`. Select a configured endpoint
        with `--pipe <local-Seed-name>` before `--start` or the command. The default is
        `Seed.KMS.v1`. See [console transport](kms.md) for reload, cancellation and
        streaming behavior, and [configuration](configuration.md) for persistence.

        ## File, process and registry commands

        The compact [system command walkthrough](system-commands.md) demonstrates
        native file creation/read/copy/rename, process snapshots and Windows
        registry values. Use a disposable directory and registry key to try the
        mutation commands. The console preserves quoted arguments containing
        spaces and Unicode.

        A bounded native demonstration can also run without starting a KMS or
        HTTP listener. Select a new output directory:

        ```powershell
        ./Artifacts/x64/Debug/Seed.Console/Seed.Console.exe --boot-demo ./Artifacts/x64/Debug/Runtime/Seed.Runtime.exe ./Artifacts/native-demo
        ```

        It creates example files, lists processes, reads the Windows product type,
        exports live RTTI and exits. The launcher prints boot-demo.txt after
        completion and enforces a one-minute process deadline. This post-boot
        command verifies the bounded demo; an actual BootExecute run requires
        separate [test-VM boot verification](boot-debugging.md).

        ## HTTP endpoints

        The default host binds to 127.0.0.1:8080. In another PowerShell session:

        ```powershell
        Invoke-RestMethod http://127.0.0.1:8080/health
        Invoke-RestMethod http://127.0.0.1:8080/echo -Method Post -ContentType 'text/plain' -Body 'Hello from Seed'
        Invoke-RestMethod http://127.0.0.1:8080/json -Method Post -ContentType 'application/json' -Body '{"Sequence":42,"Message":"Hello from Seed"}'
        ```

        GET /health returns a runtime health message. POST /echo returns the raw
        request bytes. POST /json deserializes and serializes the reflected
        Seed::Runtime::RoundTripMessage record with Sequence and Message fields.
        POST /binary accepts that demonstration DTO only: it checks the matching
        type identity, bounded envelope and UTF-16 before native deserialization.
        It is not a general endpoint for arbitrary reflected types.

        ## Managed metadata and binary round trip

        Export RTTI from the running native process, then pass that file and the
        listener URL to the managed example:

        ```powershell
        $seedConsole = './Artifacts/x64/Debug/Seed.Console/Seed.Console.exe'
        $seedRtti = Join-Path $PWD 'Artifacts/demo-rtti.dat'
        & $seedConsole di.rtti.save $seedRtti
        ./Artifacts/x64/Debug/Seed.Managed.RoundTrip/Seed.Managed.RoundTrip.exe $seedRtti http://127.0.0.1:8080/ --verify-malformed
        ```

        The example reads live native RTTI, maps the demonstration record to C#,
        performs managed serialization and exercises the native JSON and binary
        endpoints. It checks Unicode and empty strings, rejects truncated or
        mismatched binary messages, and probes selected malformed metadata headers.
        Omit the URL for the local metadata/serialization checks only. Metadata must
        come from the matching trusted runtime; these probes do not validate every
        possible malformed CTTI graph. See [managed interoperability](managed.md).

        ## Enable HTTPS with a development identity

        Generate new files for this checkout and configure the running host:

        ```powershell
        ./Tools/New-SeedDevelopmentIdentity.ps1 -Directory ./Artifacts/dev-identity
        $seedConsole = './Artifacts/x64/Debug/Seed.Console/Seed.Console.exe'
        $seedCertificate = (Resolve-Path './Artifacts/dev-identity/chain.pem').Path
        $seedPrivateKey = (Resolve-Path './Artifacts/dev-identity/key.pem').Path
        & $seedConsole configurations.set host.tls.certificateFile $seedCertificate
        & $seedConsole configurations.set host.tls.privateKeyFile $seedPrivateKey
        & $seedConsole configurations.set host.https.enabled true
        & $seedConsole configurations.save
        & $seedConsole reload
        ```

        The helper creates a one-day localhost identity without installing its root
        into Windows trust. HTTPS defaults to https://localhost:8443 on the same
        loopback address and exposes the same four endpoints. Clients must explicitly
        trust the generated root. Supply your own certificate/key files for other
        identities; the checked-in test keys are deliberately public.

        Both listeners serve HTTP/1.1, with 16 active connections per listener,
        16 MiB requests, 5-second transport operations and a 30-second connection
        deadline. Shutdown cancels and joins connections; a racing operation may
        reach its bounded timeout. See [TLS](tls.md) for protocol and acceptance limits.

        ## Native outbound HTTP and HTTPS

        ```powershell
        ./Artifacts/x64/Debug/Seed.Native.HttpClient/Seed.Native.HttpClient.exe --url https://example.com/ --output ./Artifacts/response.txt
        ```

        This managed launcher starts a short-lived native client through Seed.Starter.
        Seed performs DNS, TCP, HTTP, TLS and cryptography. Response bodies, headers,
        loaded-module inventories and failure diagnostics are written beside the
        selected output file. `--trust-root <DER-file>` selects an explicit development
        trust root; `--tls 1.2` or `--tls 1.3` restricts negotiation. `--runtime <path>`
        selects another matching native build; the launcher otherwise selects Debug.

        ## Other execution and verification paths

        [Windows Docker](docker.md) describes building the runtime image, running its
        console and HTTP listener as ContainerUser, selecting isolation and preserving
        test reports. [Boot debugging](boot-debugging.md) describes the separate manual
        test-VM workflow; full networking during Windows boot remains unvalidated.

        Tools/Test-SeedIntegration.ps1 owns its native process, exercises HTTP, console
        commands and managed interoperability, and shuts its process down. Run it
        separately from a manually started default host. [Verification](testing.md)
        lists the focused native, managed, TLS, E2E and adapter workflows.
        """;
}
