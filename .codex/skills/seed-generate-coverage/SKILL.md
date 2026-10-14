---
name: seed-generate-coverage
description: Build the Seed Coverage profile and collect attributed native source coverage through an owned test process.
---

# seed-generate-coverage

Run from the Seed checkout root:

```powershell
powershell -ExecutionPolicy Bypass -File Tools/Invoke-SeedWorkflow.ps1 -Action Coverage -Configuration Debug
```

OpenCppCoverage must cover child processes and continue after C++ exceptions. Preserve the exact executable/PDB/MAP with the report. The collector is a development tool. Verify it attributes Seed.Runtime source lines; a managed-only or empty capture is a failed collection. This collector path is not yet release-validated.

Native builds use all available cores. Use `-NoBuild` only with matching current
runtime and starter artifacts. Report missing tools, failed tests and unavailable
protocol validation explicitly; do not convert them into successful results.
