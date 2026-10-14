# Seed development instructions

Seed is a custom NTAPI-first native platform with managed reflection and
serialization libraries. Read the current file and nearby implementation first,
then the relevant companion guidance. Preserve the runtime's own abstractions.
The current subsystem governs design and behavior; mandatory formatting rules
apply even when neighboring code differs.

- `copilot-context.md`: shipped source components and build boundaries.
- `code-convention.md`: native formatting, encoding and XML documentation.
- `copilot-runtime-patterns.md`: services, exceptions and linker registrations.
- `copilot-collections.md`: collections and ownership rules.
- `copilot-async-model.md`: tasks, threadables, context and interruption.
- `copilot-anti-patterns.md`: changes that violate runtime assumptions.

Use `.github/skills` for Copilot workflows and `.codex/skills` for Codex workflows.
Generate documentation into ignored `Documentation/`. Only shipped components
may appear in catalogs, examples, scripts and links.

Document every handwritten public type/member with meaningful multiline XML
summaries. CTTI metadata is part of the public surface, including inherited
records and helpers reachable through TypeTraits. Fields/properties need value
descriptions; callable declarations need applicable parameter and return tags.

Native runtime changes use Seed primitives, containers and NTAPI wrappers. Win32
dependencies belong to explicit external adapters. Keep each service and its
instantiation carriers in the component that owns it.
