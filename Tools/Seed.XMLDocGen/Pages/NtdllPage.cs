namespace Seed.XMLDocGen;

internal sealed class NtdllPage : ISourcePage
{
    public string FileName => "ntdll.md";

    public string Title => "Update the ntdll import surface";

    public string Render(PageContext context) => """
        # Update the ntdll import surface

        Lib/NtApi/ntdll.def is the reviewed source of native import names. The matching
        x64 ntdll.lib and ntdll.exp are generated build inputs. Use an x64 Visual Studio
        developer environment or pass explicit LibExe and DumpbinExe tool paths.

        ```powershell
        # Rebuild import artifacts from the approved definition.
        ./Tools/Update-NtdllImportLibrary.ps1
        # Add a verified export required by the changed native component.
        ./Tools/Update-NtdllImportLibrary.ps1 -AddExport NtCancelIoFileEx
        # Review a complete definition refresh against this machine's Windows build.
        ./Tools/Update-NtdllImportLibrary.ps1 -RefreshDefFromSystem -SkipLib
        ```

        The refresh extracts names from System32/ntdll.dll through dumpbin /exports,
        removes the table metadata and writes EXPORTS followed by unique function
        names. Review the definition diff before generating the library. A machine's
        export list does not prove that every name exists on the supported Windows
        baseline. Prefer a narrow AddExport when only one new import is required.

        The underlying library command is lib.exe /def:Lib/NtApi/ntdll.def
        /machine:x64 /out:Lib/NtApi/ntdll.lib. Do not copy a random SDK import library
        over it. Confirm declarations and calling conventions in the approved NtApi
        headers; update their provenance notices when importing upstream definitions.

        Build the owning component and matching starter with all cores. Inspect the
        runtime's dumpbin /imports output and loaded modules, then run focused native
        tests and ordinary-user startup. If the added API participates in early startup,
        also perform the [boot debugger check](boot-debugging.md) in a test VM. Keep
        Windows 11 24H2 / Server 2025 compatibility checks separate from a local refresh.
        """;
}
