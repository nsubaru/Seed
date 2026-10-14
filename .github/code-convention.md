# Seed native conventions

Preserve UTF-16LE BOM and CRLF for C++ sources. Follow `Native/.editorconfig` and
the closest existing file. Use four spaces and indent namespace contents.

These formatting requirements are mandatory for handwritten Seed-owned native
C++. Existing inconsistent formatting is not an exemption. Namespaces, control
flow, switch, try/catch and standalone lifetime scopes open on the next line.
Types, functions, methods and lambdas open on the signature's final line, with
one space before `{`. Every if/else, for/range-for, while and do body has explicit
braces, including single-statement and empty bodies and bodies inside lambdas.
An else-if chain is permitted, with braces on each branch body.

Access specifiers align with their class; members indent one level. Preprocessor
directives and labels stay at the left margin. Keep empty types/functions/lambdas
as `{}` when appropriate. Keep else and catch on separate lines from the preceding
closing brace; the closing do/while form is `} while (...);`.

For multiline function or method parameter lists, put `(` after the name and
start the first parameter on the next line. Put one parameter per line, indented
one level, and put `)` on its own line aligned with the declaration. A definition
opens its body on that closing signature line; a declaration ends with `;`.
Trailing qualifiers remain between `)` and `{`. Preserve valid constructor
initializers and put the body brace after the final initializer.

```cpp
Core::String DescribeListener(
    const Core::String& address,
    Core::UInt16 port
) {
    if (port == 0)
    {
        return address;
    }

    return Core::String::Combine(address, L":", port);
}
```

Keep Seed's literal suffixes adjacent to the literal (`L"text"S`, `L"text"C`).
Generic C++ formatters can incorrectly insert spaces before these MSVC-supported
suffixes. Verify their spelling, continued macros and UTF-16 encoding after any
formatter pass; formatter settings alone do not cover the complete runtime style.
Verify compliance explicitly, including scope braces, multiline signatures,
logical groups, macro continuations and encoding.

Use PascalCase for types and functions, camelCase for ordinary parameters,
private fields and locals. Follow NTAPI handle naming at the platform boundary.
Use repeated access sections and regions for logical groups. Keep one blank
line between completed methods/groups. Preserve intentional table alignment.

Prefer aggregate/designated initialization in declaration order. Align `=` in
one logical initializer group. Use assignments for subsequent/staged updates.
Do not change initialization semantics while formatting.

Outside macros, public XML documentation uses separate `/// <summary>`, body and
`/// </summary>` lines. Put complete param/typeparam/returns tags on individual
lines after the summary; omit empty tags. Fields/properties have summaries only.
Separate documented declarations with a blank line except enum entries and the
last member. Macro separator lines retain the continuation backslash.

## File shape and ownership

Use dotted component names that mirror the native namespace: for example,
`Seed.Core.IO.ByteStream.h` belongs to `Seed::Core::IO`. A component's `Base.h`
contains common includes and declarations, focused `.h`/`.cpp` pairs own their
implementation, and its public `.h` collects the supported surface. Exceptions
for template specializations must follow C++ lookup rules. Keep HTTP, TLS, KMS
and runtime composition in their owning components.

Only low-level Core facilities use the staged Init/Stop/Stage1/Stage2 hooks.
Other functionality uses services and the owning component's registration policy.
Keep heavy instantiations and boot-up registration carriers in that component's
Instantiations.cpp unit. Verify one retained linker descriptor per registration.

## Logical groups and expressions

Repeat public/protected/private sections to separate groups. Use regions where
the local subsystem uses them. Do not pad after opening or before closing markers;
consecutive opening markers need no intervening blank line. Related private fields
and friends may stay consecutive. Split long inheritance lists into continuations.

Keep call parentheses tight, spaces after control keywords and commas, and pointer
or reference markers beside their types. Lambda parameter lists use `[&] (const
Value& value)`. Put else and catch on separate lines; do/while closes as
`} while (...);`. Spaces surround binary/assignment/ternary operators. Preserve
intentional multiline alignment in expressions, parameters and CTTI/RTTI macros.

For records, prefer designated initialization in declaration order. Retain defaults
and ownership semantics. Use later assignments only for genuine staged updates,
aligning related assignments as one group. Never merge unrelated groups to align them.

## Public contracts

Meaningful XML summaries are required for handwritten public types, enum values,
aliases, constructors, destructors, operators, methods, constants and fields.
Document at the declaration. Describe fields/properties as values, including units,
sentinel values, scope and ownership where relevant. Callable declarations receive
matching parameter names, applicable type parameters and returns for non-void
operations. Constructors, destructors and void operations have no returns tag.
Use multiline remarks for additional explanation; no empty documentation tags.

CTTI is mandatory for reflected surfaces. CttiClassRecord belongs at the top of
the public class surface; StructCttiDef follows the fields in serialization order.
Use EnumCttiDefinition for reflected enums. Document inherited metadata and helpers
reachable through TypeTraits::CTTITypeMetaData, including Details records.

## Runtime behavior while editing

Use Seed collections, allocators, strings, smart pointers, delegates, Result,
Optional and Either. Seed does not use the standard C++ runtime or compiler RTTI
as substitutes. Use NTAPI/Seed IO inside the runtime; Win32 dependencies belong
in explicit external adapters. Dynamic exception text lives on the exception heap
as ExceptionString. Keep custom Clone/ThrowMe and RawException flows intact.

Do not add noexcept to ordinary native methods: threadable interruption can throw
at any instruction. Preserve the narrowly documented compiler/coroutine requirements
and their interruption boundaries. Refcount disposal enters at zero; cleanup that
can reenter disposal uses RefCountT::InvalidRefCountValue under the owning state gate.
Use RefCountClassOutsideHeader/RefCountClassOutside for disposable implementations
that can live in .cpp, or RefCountClass for an intentionally inline shape.

Async code preserves payload ownership, context, diagnostics and MarkFinished
ordering. Await raw overlapped tasks to submit their IO; GetValue alone is not
submission. Cancel and drain a timed-out native request before releasing buffers
or the IOSB. Collection null iterators and explicit materialization have deliberate
lifetime semantics; follow the collection and async companion guides.

Add natvis for new debugger-heavy exception, collection, pointer or result types.
Update shared-item/project inputs and matching `.vcxitems.filters` /
`.vcxproj.filters` for new native sources and visualizers. Group files by their
owning component, including headers, implementations and debugger visualizers.
Keep native/shared project identifiers consistent with `Seed.slnx`. The runtime
project has an explicit file index for Visual Studio, matching the SDK component
shared-item inventories listed in its `SeedSourceComponents` group. Update both
indexes when component files change, preserving per-file compiler metadata. Run
`Tools/Test-SeedSource.ps1` to check SDK coverage, filters and project inputs. Apply
these rules to changed code without a formatting sweep of unrelated files.

## Managed tooling and verification

Managed code may use the .NET patterns at its adapter boundary. Use PascalCase
for public types/members, camelCase for parameters/private fields and four spaces.
Keep namespaces aligned with the shipped component and meaningful XML summaries
on public non-test APIs. Prefer small source files for independent documentation
pages and test topics. Do not export excluded domain contracts through helper DTOs.

Build native changes with MSBuild /m and compiler /MP, then run focused meaningful
tests. Keep runtime source edits outside an active native build. Standard startup,
E2E and native tests run without elevation. Isolate actual DHCP lease/interface
changes in a dedicated network fixture. Reports, generated docs and package files
belong under ignored output directories.

## PowerShell scripts

Use four spaces and opening/closing braces on separate lines for functions,
classes, conditionals, loops, try/catch/finally and script blocks. Expand compact
one-line blocks. Keep PowerShell's required `@{` token together, placing the
hashtable on its own line when assigning it. Preserve required pipeline/argument
continuation syntax when moving a script-block brace to the next line.

Use blank lines for logical separation, as in
[DependencyInjection::Resolve](../Native/Seed.Core.DependencyInjection.DI.h):
separate parameters/setup, input validation, dependency discovery, a command and
its exit-code check, report inspection, and cleanup. Keep a command adjacent to
its exit-code check. Group related assignments and related argument construction;
do not put a blank line between every statement or merge unrelated groups.

```powershell
param([string] $Image = 'seed:tests-local')

$ErrorActionPreference = 'Stop'

if (-not $Image)
{
    throw 'An image name is required.'
}

docker image inspect $Image
if ($LASTEXITCODE -ne 0)
{
    throw 'The selected test image is unavailable.'
}

Write-Output "Selected image: $Image"
```

Retain Windows PowerShell 5.1 compatibility in Docker entry/helper scripts.
Validate parsing after formatting and preserve script behavior, argument quoting,
environment restoration and owned-process/container cleanup.
