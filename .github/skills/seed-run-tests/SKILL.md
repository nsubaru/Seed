---
name: seed-run-tests
description: Run Seed native tests and collect NUnit results for all tests, a wildcard or one reflected type name.
---

# seed-run-tests

Run from the Seed checkout root:

```powershell
powershell -ExecutionPolicy Bypass -File Tools/Invoke-SeedWorkflow.ps1 -Action Tests -Configuration Debug
```

Use `-Wildcard "*String*"` for a focused set or pass one full reflected test name. The runner starts an owned native process through the matching Seed starter, rejects elevation and validates the resulting report. The current bootstrap path runs before the console shell is attached.

Native builds use all available cores. Use `-NoBuild` only with matching current
runtime and starter artifacts. Report missing tools, failed tests and unavailable
protocol validation explicitly; do not convert them into successful results.
