---
name: seed-export-rtti
description: Export live, fixed-up Seed RTTI for managed interoperability and schema validation.
---

# seed-export-rtti

Run from the Seed checkout root:

```powershell
powershell -ExecutionPolicy Bypass -File Tools/Invoke-SeedWorkflow.ps1 -Action Rtti -Configuration Debug
```

The export comes from the running Seed section after fixups. Never substitute a raw PE section or an unrelated runtime fixture. Confirm the native version, run the managed round-trip example and audit the type catalog before moving a fixture into a tracked test directory.

Native builds use all available cores. Use `-NoBuild` only with matching current
runtime and starter artifacts. Report missing tools, failed tests and unavailable
protocol validation explicitly; do not convert them into successful results.
