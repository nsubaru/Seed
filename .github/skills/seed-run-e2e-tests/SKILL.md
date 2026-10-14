---
name: seed-run-e2e-tests
description: Build and run Seed's owned-runtime KMS E2E suite, including fast reboot, configuration, logs and command behavior, with retained TRX reports.
---

Run from the clean Seed checkout with Windows x64, Visual Studio v145 and .NET 10:

```powershell
./Tools/Test-SeedE2E.ps1 -Configuration Debug
./Tools/Test-SeedE2E.ps1 -Configuration Release -NoBuild -Class CommandTests -Method StatusHasStableLabelsAndFastRebootKeepsTheProcess
```

Use NoBuild only when the matching runtime, starter and test executable exist.
The workflow owns every process it starts, uses a fresh configuration directory,
and shares a per-account lease with native tests because of fixed loopback ports.
Run under a non-elevated account. Do not attach to an unrelated developer runtime.

Read Artifacts/e2e/<configuration>/<run>/ TRX and commands.log files for failures.
An empty selection fails. Fast reboot uses reload and requires a changed composition
in the same process. Standard DHCP tests reject invalid selections without changing
interfaces; real lease tests require a separately isolated network.

Reload completion includes a ready new composition and KMS reconnect, following
only saved pipe-name changes. Run ReloadTests for real console and pipe-change
checks, and CancellationTests for disconnect/token cancellation and rejected
pipelining. Native test cancellation stops between complete cases and never
publishes a partial NUnit report. Keep current-case IO and cleanup bounded.

For Windows Docker verification, use ./Tools/Test-SeedDocker.ps1. It builds a
source/test image with public toolchain inputs and runs as ContainerUser. Use
-NoBuild only with a matching test image; -Class/-Method select the same E2E
tests, while -NativeWildcard selects the additional native self-tests.
Reports remain under Artifacts/docker after the owned container is removed.
Read Documentation/docker.md for host/image compatibility and build targets.

For test authoring, read Testing/Seed.E2ETests/E2ETest.cs and the nearest test,
then the generated Documentation/e2e.md when available. Public helper changes
belong in Lib/Seed.E2E.Infrastructure and need meaningful XML documentation.
