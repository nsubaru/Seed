# Seed development instructions

Seed is a custom NTAPI-first native platform with managed reflection and
serialization libraries. Read the current file and nearby implementation first,
then the relevant companion guidance. Preserve the runtime's own abstractions.
The current subsystem governs design and behavior; mandatory formatting rules
apply even when neighboring code differs.

- `copilot-context.md`: shipped source components and build boundaries.
- [Code conventions](code-convention.md) for formatting, encoding and XML documentation.
- `copilot-runtime-patterns.md`: services, exceptions and linker registrations.
- `copilot-collections.md`: collections and ownership rules.
- `copilot-async-model.md`: tasks, threadables, context and interruption.
- `copilot-anti-patterns.md`: changes that violate runtime assumptions.

Use `.github/skills` for Copilot workflows and `.codex/skills` for Codex workflows.
Generate documentation into ignored `Documentation/`. Only shipped components
may appear in catalogs, examples, scripts and links.

Use the code conventions for formatting and verification. In particular, check
multiline signature closers and the constructor decision order. Exclude operator
and constructor signatures from assignment alignment, and preserve protected
cryptographic data byte for byte.

Document handwritten public native and managed declarations with multiline XML
summaries. Use one tag per line for named parameters, type parameters and return
values. Match names to the declaration and describe the task returned by an async
method. CTTI metadata reachable through `TypeTraits::CTTITypeMetaData` is part of
the public surface.

Native runtime changes use Seed primitives, containers and NTAPI wrappers. Win32
dependencies belong to explicit external adapters. Keep each service and its
instantiation carriers in the component that owns it.
