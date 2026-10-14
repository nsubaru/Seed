# Seed source context

| Location | Responsibility |
|---|---|
| `Lib/Seed.MinCore` | Types, traits, atomics, ownership and compiler foundations |
| `Lib/Seed.RTTI` | Compiler-facing type-info ABI |
| `Native/Seed.Core.*` | Runtime, IO, services, async, CTTI/RTTI, serialization, DNS, DHCP and cryptography |
| `Native/Seed.Http.*` | HTTP/1.1 parsing, client and endpoint handling |
| `Native/Seed.Tls.*` | TLS protocol, signatures, certificate validation/provider |
| `Native/Seed.Runtime.*` | Standalone application composition |
| `PublicAPI` | Managed metadata, selected contracts and serialization |
| `Tools/Seed.XMLDocGen` | Source-scoped Markdown documentation |
| `Lib/Seed.E2E.Infrastructure` | Owned runtime lifecycle and bounded KMS E2E exchanges |
| `Testing/Seed.E2ETests` | Ordinary-user KMS behavior and fast-reboot regressions |
| `Tools/Seed.Testing.TestAdapter` | Visual Studio native test discovery/execution |
| `Tools/Seed.Testing.Host` | Separate .NET 10 KMS/RTTI host for the native adapter |
| `Docker` | Windows source build, ordinary-user tests and runtime images |

Use `Seed.slnx`. Native builds use MSVC v145, x64 and all available cores.
Foundational managed projects target netstandard2.0; tools and serialization use
.NET 10. Source SDK consumers import shared items and preserve the custom runtime
compiler/linker configuration. Generated output belongs under `Artifacts/`.

The native runtime uses NTAPI without loading Win32 DLLs. Platform security
descriptors and TLS peer validation are required facilities. Normal startup and
standard tests must work as an ordinary user; DHCP mutations are explicit.

Tools/Build-SeedDocker.ps1 builds Windows images; Tools/Test-SeedDocker.ps1
runs container E2E/native verification and preserves its reports. Docker builds
use public toolchain inputs and all available cores, without a private feed.
