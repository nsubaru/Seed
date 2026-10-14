---
name: seed-update-ntdll
description: Update the Seed ntdll definition and regenerate x64 import artifacts using installed Visual Studio tools.
---

# Seed ntdll import surface

Confirm required declarations and exports, then run from the checkout root:

```powershell
powershell -ExecutionPolicy Bypass -File Tools/Update-NtdllImportLibrary.ps1
```

Use `-AddExport` for a deliberately added function. Use `-RefreshDefFromSystem`
only for an intentional OS-specific inventory; it can add APIs absent from the
supported baseline. Build and inspect imports after any change. Generated .lib
and .exp files are ignored and are regenerated from the shipped definition.

Read Documentation/ntdll.md after documentation generation for the reviewed
dumpbin/definition/library flow. Early-startup imports additionally need the
test-VM WinDbg checks in Documentation/boot-debugging.md; do not change host boot
settings for an import refresh.
