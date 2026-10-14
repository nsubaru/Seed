# Third-party notices

Seed-owned code uses Apache-2.0. Retain LICENSE and NOTICE with distributions.
The current extraction is not cleared for a public release.

phnt-derived declarations retain the upstream MIT terms and attribution in
Lib/NtApi/LICENSE.phnt. A compiled test-adapter package includes that notice.
SDK/compiler support-header redistribution and retained certificate data remain
separate provenance audits for the source SDK and runtime release.

Windows Docker images use Microsoft Windows and .NET base images. The runtime
image carries app-local x64 MSVC runtime DLLs from VS Build Tools for the
external Seed.Starter adapter. These vendor files retain Microsoft's licensing;
Apache-2.0 does not cover them. See the [Microsoft redistribution guidance](https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files)
and the [Visual Studio license directory](https://visualstudio.microsoft.com/license-terms/).

Managed development tools use packages with their own licenses. Generate
Artifacts/dependencies.json through Tools/Write-SeedDependencyInventory.ps1 for
the restored package names, versions, content hashes and declared license metadata.
The adapter package carries this inventory alongside its host dependencies. Its
key dependencies include Microsoft.TestPlatform.ObjectModel and
System.Reflection.Metadata (MIT), CommunityToolkit.HighPerformance (MIT),
MessagePack (MIT) and Microsoft Windows SDK managed support (MIT). Consult each
package's recorded license and source notices when redistributing it.

Seed's native HTTP/TLS/cryptography runtime does not use external implementations.
xUnit, Microsoft Testing Platform, .NET TLS peers and OpenSSL are test tools;
their licenses do not replace the licensing of Seed's native implementation.
