# Seed.Testing.TestAdapter

The adapter brings Seed native C++ `ITest` cases into Visual Studio Test Explorer
and VSTest. It adapts the original test adapter's native case discovery, grouped
execution, debugger attachment and result mapping to Seed's current KMS protocol.

Use Windows x64, Visual Studio 2026 and the .NET 10 runtime. Install the local
NuGet package in the native project, or pass its extracted `tools/adapter` directory
to `vstest.console.exe /TestAdapterPath:<directory>`. The package's native props
mark the project as a test container. Build the native image before discovery.
The caller must be non-elevated; discovery starts and stops an owned runtime.

Discovery cross-checks `tests.list` against live RTTI. Execution sends explicit
selections through `tests.run` and reads complete NUnit reports. Native failures
and interrupted cases fail; deliberate unsupported-hardware skips remain skipped.
Cancel closes the owned test session and stops its native process; the verification
workflow checks cancellation before execution leaves no process or completed report.
Test runs are
serialized per Windows account because native self-tests use fixed loopback ports.

Debug selected tests attaches to the native process before test execution. This
does not stop early Core startup. Use the boot-debugging guide for early startup.
Source navigation through DIA and a VSIX extension are not included in this package;
native failure stack information remains in the NUnit report and Test Explorer.

From the Seed checkout, use `Tools/Pack-SeedTestAdapter.ps1` after Release builds
and `Tools/Test-SeedTestAdapter.ps1` to validate discovery and execution. Generated
guides cover E2E authoring, adapter packaging, ntdll updates and boot debugging.
Apache-2.0; retain the package's notices and dependency inventory.
