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

Close named and anonymous namespaces with `}` alone. Omit namespace-ending
comments such as `} // namespace Seed::KMS::Commands::Process`.

For multiline function or method parameter lists, put `(` after the name and
start the first parameter on the next line. Put one parameter per line, indented
one level, and attach the closing `)` to the last parameter. A definition opens
its body on that signature line after trailing qualifiers. A declaration ends
with `;`. Keep the closer and suffix ahead of a trailing parameter comment. For
a complex default object, keep `= Type{` with its parameter and attach `)` after
the last parameter's closing brace when the object spans lines.

For initialized constructors, keep an empty body and the initializer list on one
line if the compact signature before `{}` fits 160 columns and has no intrinsically
multiline content or comments that prevent compaction. Otherwise, put each
initializer on its own line when the signature exceeds 160 columns, more than
three initializers are nonempty, or at least five initializers are written. Count
outer entries. `member()` and `member{}` are empty, while `member(0)` and
`member(Type{})` are nonempty. A wrapped list ends the heading at `:` and indents
every initializer one level. Start the body on a separate constructor-aligned
line when the list wraps or the body contains statements.

```cpp
Core::String DescribeListener(
    const Core::String& address,
    Core::UInt16 port) {
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

## Width and wrapping

Use a contextual 160-column target. Count indentation, alignment, suffixes and
trailing comments. Measure a template header independently of its declaration.
Keep fitting signatures, calls, aliases, inheritance lists, assignments and
ternaries compact. Preserve intrinsically multiline content and indivisible types
or names, even when a complete unit exceeds the target.

Keep complete leading and trailing return types intact. If the leading prefix
exceeds the target, break between the return type and the complete qualified
name. Then measure parameters on the name line independently. Move an overlong
leading `requires` clause below its template header before wrapping the template
list. A wrapped template or alias argument list starts each complete outer item
on its own continuation line. Wrap long inheritance at complete base types.

For long constraints, concepts and concept-derived `constexpr` predicates, put
each outer operand on its own continuation line. Keep each joining operator with
its preceding operand. Preserve grouping and recursively expand overlong inner
groups. Keep a short inner group intact.

## File shape and ownership

Use dotted component names that mirror the native namespace. For example,
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
Value& value)`. Write C-style casts as `(UInt64) count`. Spaces surround binary,
assignment and ternary operators. Preserve intentional multiline alignment in
expressions, parameters and CTTI/RTTI macros.

Place `Seed.Core.Base.NTAPI.h` last among includes, normally in the implementation
file. Prefer multiline calls for NTAPI operations with more than four arguments.
Keep `InitializeObjectAttributes` and calls with at most four arguments compact
when they fit. Put one argument on each continuation line of a multiline NTAPI
call and attach `)` to the last argument.

Exclude constructor and overloaded operator declarations from assignment
alignment. Write `operator=` and `operator+=` with tight punctuation. Use
one space on each side of `=` in `= default`, `= delete` and `= 0` suffixes.
Check these suffixes after clang-format, which may pad them.

Treat cryptographic algorithm constants as formatter-exempt data. Preserve the
literal spelling, order, row breaks and column alignment of substitution boxes,
lookup tables, round constants and fixed byte sequences. Use `// clang-format off`
and `// clang-format on` around the affected declaration and initializer. Keep the
region limited to the constants and preserve it in any custom formatting pass.

For records, prefer designated initialization in declaration order. Retain defaults
and ownership semantics. Use later assignments only for genuine staged updates,
aligning related assignments as one group. Indent multiline designated fields one
level and align their `=` signs using the longest field name in that group. Give
each nested initializer its own alignment and close at the containing statement's
indentation. Recheck the width after alignment. Keep unrelated groups separate.

Leave a blank line before and after a multiline result assignment. Follow a
multiline call with a blank line when another call or assignment follows. Separate
sibling control blocks and lifetime scopes without padding scope edges or runs of
closing braces. Separate `break`, `continue`, `return` and `co_return` from
preceding work in the same scope. Keep attached explanations with the transfer
and share any existing separator. Align one-statement switch mappings on single
rows. Preserve ordinary layout for longer or multi-statement cases.

Label native and managed test blocks `Arrange`, `Act` and `Assert` by their
purpose. Keep each comment next to its first statement and separate phases with
a blank line. Keep consecutive assertions together, including expected-exception
assertions with a lambda. Preserve domain explanations and skip labels for a
final framework return.

## Public contracts

Document handwritten public types, enum values, aliases, constructors,
destructors, operators, methods, constants, fields and properties at their
declarations. Use separate `/// <summary>`, body and `/// </summary>` lines.
Describe fields and properties as values, including units, sentinel values and
ownership where relevant. Fields and properties need only their summary.

Place each complete `<typeparam>`, `<param>` and `<returns>` on its own line after
the summary. Match parameter and type parameter names exactly. Describe the
result of every non-void method, including a task representing completion for
`Task<void>`. Omit `<returns>` for constructors, destructors and void methods.
Use multiline `<remarks>` for extra context and omit empty tags. Leave a blank
line after a documented declaration or its inline body, except at the end of a
scope and between enum entries. Private implementation and test-only declarations
are exempt from the public API coverage requirement.

CTTI is mandatory for reflected surfaces. CttiClassRecord belongs at the top of
the public class surface; StructCttiDef follows the fields in serialization order.
Use EnumCttiDefinition for reflected enums. Document inherited metadata and helpers
reachable through TypeTraits::CTTITypeMetaData, including Details records.
Preserve macro argument grouping and the established XML block comments for
macro-generated declarations. Blank lines in continued macros retain their
continuation backslash.

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
on public non-test APIs. Follow the applicable `.editorconfig` for C# layout and
the public XML documentation rules above for comments. Prefer small source files
for independent documentation pages and test topics. Do not export excluded
domain contracts through helper DTOs.

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
