---
name: seed-xml-docgen
description: Build and generate Seed documentation from the shipped component manifest and compiler XML.
---

# seed-xml-docgen

Run from the Seed checkout root:

```powershell
powershell -ExecutionPolicy Bypass -File Tools/Invoke-SeedWorkflow.ps1 -Action Documentation -Configuration Debug
```

Build the native and managed projects first for the complete API reference. Output belongs in ignored Documentation. Check generated links and keep one page implementation per independent topic.

Native builds use all available cores. Use `-NoBuild` only with matching current
runtime and starter artifacts. Report missing tools, failed tests and unavailable
protocol validation explicitly; do not convert them into successful results.
