# Seed

Seed is an independent extraction of an NTAPI-first Windows runtime. It combines
native containers, async IO, reflection, serialization, DNS, DHCP and cryptography
with HTTP and TLS components and managed reflection/serialization assemblies.

## Source SDK

- `Lib/Seed.MinCore`: primitive types, traits, reference counting and compiler ABI.
- `Lib/Seed.RTTI`: compiler-facing reflection support.
- `Native/Seed.Core.vcxitems`: runtime, collections, exceptions, CTTI/RTTI,
  serialization, JSON, DI, services, threading, async IO, diagnostics, DNS, DHCP
  and core cryptography.
- `Native/Seed.Http.vcxitems`: HTTP/1.1 parser, native client and endpoint handlers.
- `Native/Seed.Tls.vcxitems`: TLS client/server, certificate parsing and TLS-owned
  certificate providers and RSA identities.
- `Native/Seed.KMS.vcxitems`: local message-pipe console transport and selected
  commands; `Seed.KMS.Transport` is the independent managed transport assembly.
- `PublicAPI/`: `Seed.Shared`, `Seed.CTTI`, selected `Seed.Objects`, `Seed.RTTI`
  and `Seed.Serialization`.

Native delivery uses source components and MSBuild shared items. Version 0.1 does
not promise a stable native binary ABI. Reflection data must match the runtime
that produced it.

## Building the current extraction

Use Windows x64, Visual Studio 2026 with MSVC v145, a Windows SDK and .NET 10.
The initial validation targets are Windows 11 24H2 and Windows Server 2025.

```powershell
./Tools/Update-NtdllImportLibrary.ps1
MSBuild Seed.slnx /m /restore /p:Configuration=Debug /p:Platform=x64
./Tools/Invoke-SeedWorkflow.ps1 -Action Tests -NoBuild
$seedConfigurationDirectory = Join-Path $PWD ('Artifacts/integration/configuration-' + [Guid]::NewGuid().ToString('N'))
./Tools/Test-SeedIntegration.ps1 -ConfigurationDirectory $seedConfigurationDirectory
./Tools/Test-SeedConfigurationStartup.ps1 -FixturePath (Join-Path $seedConfigurationDirectory 'core.v1.bin')
./Tools/Test-SeedHostConfiguration.ps1
./Tools/Test-SeedRsaSigning.ps1
./Tools/Test-SeedTlsIdentity.ps1
./Tools/Test-SeedTlsClient.ps1
./Tools/Test-SeedTlsServer.ps1
./Tools/Test-SeedE2E.ps1 -NoBuild
./Tools/Test-SeedTestAdapter.ps1
./Tools/Invoke-SeedWorkflow.ps1 -Action Documentation
```

Native builds use all available cores and may require substantial memory and
pagefile capacity. Debug, Release and Coverage configurations are defined.
Use `./Tools/Invoke-SeedValidation.ps1 -Configuration Release` for the complete
local validation sequence after building the matching solution. Hardware-dependent
test skips remain visible separately in NUnit reports; failed and interrupted tests
fail verification.
Outputs go to `Artifacts/`. Foundational managed libraries target
`netstandard2.0`; serialization targets .NET 10.

`Testing/Seed.E2ETests` and `Lib/Seed.E2E.Infrastructure` provide ordinary-user
KMS E2E tests for all shipped commands, including repeated fast reboot through
`reload`, save/reload behavior and reconnect. Each test owns its native process
and retains its TRX report and command transcript. Use `Test-SeedE2E.ps1 -Class
CommandTests -Method <method> -NoBuild` for a focused run.

`Tools/Seed.Testing.TestAdapter` adapts native C++ tests into Visual Studio Test
Explorer through a separate .NET 10 host. It supports discovery, selected execution,
result reporting, cancellation and debugger attachment before test execution.
Use `Tools/Pack-SeedTestAdapter.ps1` for a local NuGet package. Generated
documentation contains E2E authoring, package updates, ntdll import updates,
WinDbg boot debugging and code-style guides. AGENTS.md references the same
canonical style and runtime guidance. Actual boot execution remains a test-VM
validation target; post-boot ordinary startup is the standard runtime workflow.

Windows Docker builds use public .NET 10 and VS 2026 Build Tools images. Run
`./Tools/Build-SeedDocker.ps1 -Target runtime` for the console/HTTP runtime image,
or `./Tools/Test-SeedDocker.ps1` to build and run KMS E2E and native tests as
`ContainerUser`. Reports are copied into `Artifacts/docker/` before cleanup.
The runtime retains its native NTAPI subsystem and binds HTTP to loopback by
default. Generated `Documentation/docker.md` covers build/test/artifact targets,
host compatibility, runtime configuration and the optional Docker CI runner.

The native HTTP/TLS runtime uses Seed networking and cryptography. Development
tools and managed packages are separate dependencies. Ordinary-user runtime
startup has been verified locally. Interface queries use direct NSI IOCTLs;
lease acquisition and interface mutation need separate isolated-network validation.

Core RSA signing supports unencrypted PKCS#1/PKCS#8 DER and single-block PEM
inputs in sensitive memory. SHA-256/384/512 signatures use PKCS#1 v1.5 or PSS
with 2048-, 3072- or 4096-bit keys and exponent 65537. Independent .NET checks
verify native signatures and reject tampering. Cryptographic release review and
TLS release acceptance remain pending; shipped test keys are deliberately public
and must never be used as server identities.

TLS-owned server identities can now load explicit certificate and private-key
files, validate their match and supplied chain, and sign TLS handshake inputs.
Private files are read directly into sensitive memory. The shared `TcpByteStream`
supports connected clients and accepted server sockets with actual receive counts
and complete writes. TlsServerStream authenticates TLS 1.2/1.3 using X25519 and RSA signatures.
HTTP listeners use the shared transport for plaintext or protected requests.

## Demonstrations and console

The runtime exposes loopback HTTP endpoints at `http://127.0.0.1:8080`: GET
`/health`, POST `/echo`, POST `/json`, and POST `/binary`. The JSON endpoint reflects
`Seed::Runtime::RoundTripMessage` with `Sequence` and `Message` fields.
`Tools/Test-SeedIntegration.ps1` launches the native process through the starter,
exercises these endpoints and the console, then shuts down its process.
The binary endpoint checks the matching type identity and bounded message envelope
before native deserialization. It supports the demonstration DTO only.

HTTPS starts only when enabled with explicit identity files. Generate fresh development
files with `./Tools/New-SeedDevelopmentIdentity.ps1 -Directory ./Artifacts/dev-identity`.
The helper creates a one-day localhost identity and does not install its trust root.
Configure a running runtime, then save and reload:

```powershell
$console = './Artifacts/x64/Debug/Seed.Console/Seed.Console.exe'
& $console configurations.set host.tls.certificateFile 'P:/Sources/Seed/Artifacts/dev-identity/chain.pem'
& $console configurations.set host.tls.privateKeyFile 'P:/Sources/Seed/Artifacts/dev-identity/key.pem'
& $console configurations.set host.https.enabled true
& $console configurations.save
& $console reload
```

Use paths for your own checkout. The HTTPS default is `https://localhost:8443`,
on the same loopback address as HTTP, with the same four endpoints. Clients must
explicitly trust the generated root; production users supply their own identities.
The listener supports HTTP/1.1 only, with 16 active connections, 16 MiB requests,
5-second transport operations and a 30-second overall connection deadline.
Shutdown cancels and joins connections; a racing operation may reach its timeout.

The native client demonstration runs through a thin managed launcher:

```powershell
./Artifacts/x64/Debug/Seed.Native.HttpClient/Seed.Native.HttpClient.exe --url https://example.com/ --output ./Artifacts/response.txt
```

Use `--trust-root <DER-file>` for an explicit development trust root and `--tls
1.2` or `--tls 1.3` to restrict negotiation. The launcher starts the native process
through `Seed.Starter`; Seed performs the HTTP, TLS, DNS and cryptography work.
Response bodies, headers, loaded-module inventories and failure diagnostics are
written next to the selected output file.

While a runtime is running, use
`Artifacts/x64/Debug/Seed.Console/Seed.Console.exe` interactively or supply a
single command. Sessions use the local `Seed.KMS.v1` message pipe, reject remote
clients, and connect without a login exchange. Disconnecting a console leaves
the runtime running; `shutdown` stops it and `reload` performs an in-process fast
reboot. The client waits for reinitialization and reconnects before returning to
the prompt or completing a one-command invocation. Saved pipe-name changes are
followed automatically; unsaved changes are discarded. The reconnect wait is
bounded to one minute and can be canceled with Ctrl+C.
Use `Seed.Console.exe --start <path-to-Seed.Runtime.exe>` to launch a runtime through
the starter before connecting. An optional command after the path runs once;
the runtime remains available after that console exits.
Use `--pipe <local-name>` before `--start` or a command after configuring a different
local endpoint. `status.kernel` includes a composition identifier that changes on
completed reload.

Currently implemented commands are `help`, `commands`, `version`, `exit`,
`status.kernel`, `shutdown`, `reload`, `configurations.list`, `configurations.get`,
`configurations.set`, `configurations.save`, `configurations.reset`, `logging.view`,
`logging.get`, `logging.set`, `logging.follow`,
`tests.list`, `tests.run`, `tests.result.store`, `di.rtti.list`, `di.rtti.save`,
`network.interfaces`, `network.dns.resolve`, `network.dhcp.start`,
`network.dhcp.status`, and `network.dhcp.stop`. Use `tests.run *Crypt*` or an
exact native test type name, followed by `tests.result.store <file>` for NUnit
output. RTTI export and report commands take an explicit ordinary-file path.

Core configuration uses a version-one Seed envelope. `SEED_CONFIG_DIRECTORY`
selects its writable directory; otherwise the runtime uses adjacent `Seed.Data`.
Absent configuration uses compiled defaults. Changes persist only on explicit
save. `configurations.list` shows the supported Core controls, units and ranges;
set/get/reset take a listed name. Settings apply to future allocations, timers,
cache operations or shutdown as indicated. Reflected host options use a separate
`host.v1.bin` file, containing envelope version 2 and its new schema identity.
Earlier host envelopes are rejected without implicit migration. `host.http.enabled`, `.bindAddress` and `.port` control the
IPv4 demonstration listener; `host.kms.pipeName` selects a local `Seed.` endpoint.
`host.tls.trustBundleFile` supplies an additional DER root for native client requests.
`host.https.enabled` and `.port` configure HTTPS. Certificate/key locations and
`host.tls.privateKeyFormat` select its TLS-owned identity. Host edits take effect after explicit save and reload;
unsaved edits are discarded on reload. Host envelopes are checked for bounded valid
input before deserialization and saved through a flushed temporary-file replacement.
Core and host files are saved separately.
`logging.set <mask> <module-name>` changes a live dynamic
logger: bits 0..4 enable Debug, Verbose, Warning, Error and Critical. Logger changes
persist only when `configurations.save` saves the host overrides. Reload applies
saved masks after service initialization; a full reset/save/reload restores defaults.
`logging.follow` streams new Core log output until Ctrl+C or disconnect. It does
not replay history; use `logging.view` for the buffered tail. The subscription uses
Core events and bounded log pages, and releases its pending IO when the session closes.
Managed clients can use `ExecuteStreamingAsync` for live output or `ExecuteAsync`
to collect a finite response of at most 4 MiB.

DHCP maintenance is idle at startup. `network.dhcp.start <interface-index>` requests
acquisition on one connected, eligible interface without a usable IPv4 address.
The runtime owns the worker after the console disconnects. Read acquisition errors
and cleanup status with `network.dhcp.status`; `network.dhcp.stop <interface-index>`
cancels and joins that operation and releases its owned configuration. Runtime
shutdown stops all workers. Lease acquisition, renewal, rebinding, expiry and Windows
interface rollback still require isolated-network validation before release.

HTTP/1.1 is the protocol scope. Independent client tests cover TLS 1.2/1.3,
fragmented responses, wrong hostnames, untrusted chains, expired certificates and
incompatible protocol versions. Full failure acceptance
remains a release requirement. The server supports HTTP and explicitly configured
HTTPS.

## Licensing and contribution

Apache-2.0 is selected for Seed-owned code, subject to the pending contribution
and historical rights audit. Imported support headers in this local extraction
are audit inputs and are not yet cleared for redistribution. Upstream phnt is
[MIT licensed](https://github.com/winsiderss/phnt/blob/master/LICENSE); its
attribution must remain with any approved derived material.

See [AGENTS.md](AGENTS.md) for runtime invariants and documentation rules.
Contributions must include a Developer Certificate of Origin sign-off
(`git commit -s`) confirming the right to contribute the submitted material.
