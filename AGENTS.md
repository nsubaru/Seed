# Seed contributor and agent instructions

Seed is a custom NTAPI-first runtime. Read the current file, neighboring subsystem
patterns and these companion files before changing code:

- [.github/copilot-instructions.md](.github/copilot-instructions.md)
- [.github/copilot-context.md](.github/copilot-context.md)
- [.github/code-convention.md](.github/code-convention.md)
- [.github/copilot-runtime-patterns.md](.github/copilot-runtime-patterns.md)
- [.github/copilot-collections.md](.github/copilot-collections.md)
- [.github/copilot-async-model.md](.github/copilot-async-model.md)
- [.github/copilot-anti-patterns.md](.github/copilot-anti-patterns.md)

The current file and neighboring subsystem remain the closest implementation
patterns for design and behavior. Mandatory formatting requirements still apply
when existing code differs. Documentation/code-style.md is generated from the same canonical
code-convention.md source, so update that source when conventions change.

## Source ownership

`Lib/Seed.MinCore` owns primitive types and compiler-facing foundations.
`Native/Seed.Core.*` owns the runtime, collections, async execution, reflection,
serialization, diagnostics, networking, DNS, DHCP and cryptography.
HTTP and TLS belong to their corresponding source components. Runtime composition
belongs to `Seed.Runtime.*`. Managed assemblies live under `PublicAPI/`.
Keep services and their registration carriers in their owning components.

## Runtime invariants

- Use Seed primitives, allocators, containers, delegates and Result/Optional/Either.
- Use NTAPI and Seed IO. The native runtime must not load Win32 DLLs.
- Preserve UTF-16LE BOM and CRLF in native C++ files. Use the native editorconfig.
- Preserve the custom exception ABI, diagnostics, context and interruption flow.
- Do not introduce `noexcept` on ordinary native methods: interruption can throw
  at any instruction. Coroutine/compiler-required specifications need a documented
  reason and an interruption boundary.
- Refcounted disposal enters with count zero. Before cleanup that can reenter
  disposal, set `RefCountT::InvalidRefCountValue` inside the owning state gate.
- Keep explicit instantiations in component instantiation units. Check the linked
  MAP for exactly one retained descriptor per registered service, command and test.
- Use CTTI declarations for reflected records and callable public surfaces.
- Document handwritten public types and members with meaningful multiline XML
  summaries. Add parameter and return documentation to public callable declarations.
- Use four spaces, repeated access groups and the established lifetime scopes.
  Preserve logical alignment and grouping; avoid unrelated formatting changes.
- Every native branch and loop body has explicit next-line scope braces, even
  for one statement or an empty body. Types/functions/methods/lambdas use
  same-line opening braces; namespaces/control flow/lifetime scopes use next-line
  braces. Multiline function parameters start on the next line, one per line,
  followed by an aligned closing signature line. Verify these rules explicitly.

## Verification

Build native projects with all available CPU cores (`MSBuild /m`, compiler `/MP`).
Build and run focused managed and native tests for the changed component. Ordinary
runtime and test startup must work without elevation. DHCP interface mutation is
an explicit operation and requires isolated-network validation.

Run Tools/Test-SeedE2E.ps1 for KMS behavior and repeated Core fast reboot through
reload. Run Tools/Test-SeedTestAdapter.ps1 for Visual Studio native discovery and
selected execution. These workflows own their runtime and retain reports. Do
not add a separate reload reconnect loop: KmsClient completes reload after a new
composition is ready, including saved pipe changes. Command cancellation is
cooperative; native cases finish cleanup before the next case is rejected. Do
not use real DHCP interface mutation in ordinary E2E tests. Native adapter and
E2E tooling may use .NET/Win32 in their external processes.

Windows Docker build/test/runtime targets live in Docker/. Use
Tools/Test-SeedDocker.ps1 for container verification; it runs as ContainerUser
and copies reports before removing its owned container. Read generated
Documentation/docker.md for host compatibility and image/configuration options.
Keep compilation parallel and preserve the NTAPI boundary. SeedSubsystem selects
Native (default), Console/CUI or Windows/GUI without changing the custom entry point.
MSVC adapter DLLs are copied app-locally; do not install a runtime redistributable.
Do not add private package feeds or host KMS pipe mappings.

Generated documentation goes under ignored `Documentation/`. Agent skills live
in `.codex/skills`; Copilot skills live in `.github/skills`. Their scripts must use
only projects and files shipped in this checkout.

Do not publish, tag a release or describe an unverified capability as supported.
The release acceptance report must identify missing builds, tests and rights
clearance explicitly.
