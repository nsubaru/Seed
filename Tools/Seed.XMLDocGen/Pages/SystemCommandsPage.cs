namespace Seed.XMLDocGen;

internal sealed class SystemCommandsPage : ISourcePage
{
    public string FileName => "system-commands.md";

    public string Title => "File, process and Windows registry demonstrations";

    public string Render(PageContext context) => """
        # File, process and Windows registry demonstrations

        These small KMS commands demonstrate Seed IO and direct NT system calls.
        They run with the native process's existing Windows access rights. Start
        the ordinary-user host as described in [demonstrations](demonstrations.md).
        Use a disposable directory and registry key for the mutation examples.

        ## Quoting

        At the interactive `seed>` prompt, double-quote arguments containing spaces.
        Backslashes are literal. Inside quotes, write `""` for a literal quote.
        `""` represents an empty argument, including the default registry value name.
        Unmatched quotes and incorrect argument counts produce command errors.

        For one-command Seed.Console calls, let PowerShell quote each argument as
        usual. The console preserves fs.* and registry.* argument boundaries when
        constructing the KMS request. KMS input remains bounded by its 65,535-byte
        packet limit; a large file is copied through fs.copy rather than fs.write.

        ## Files and directories

        File paths must be absolute DOS paths such as C:\SeedDemo\note.txt or
        native NT paths such as \??\C:\SeedDemo\note.txt. Parent directories must
        already exist when creating a directory or file.

        ```powershell
        $seedConsole = './Artifacts/x64/Debug/Seed.Console/Seed.Console.exe'
        $seedDemo = Join-Path $PWD ('Artifacts/files-' + [Guid]::NewGuid().ToString('N'))
        & $seedConsole fs.directory.create $seedDemo
        & $seedConsole fs.write (Join-Path $seedDemo 'hello.txt') 'Hello "Seed" 🌱'
        & $seedConsole fs.read (Join-Path $seedDemo 'hello.txt')
        & $seedConsole fs.info (Join-Path $seedDemo 'hello.txt')
        & $seedConsole fs.copy (Join-Path $seedDemo 'hello.txt') (Join-Path $seedDemo 'copy.txt')
        & $seedConsole fs.move (Join-Path $seedDemo 'copy.txt') (Join-Path $seedDemo 'renamed.txt')
        & $seedConsole fs.list $seedDemo
        & $seedConsole fs.delete (Join-Path $seedDemo 'hello.txt')
        & $seedConsole fs.delete (Join-Path $seedDemo 'renamed.txt')
        & $seedConsole fs.delete $seedDemo
        ```

        fs.list enumerates immediate entries, omitting . and .., in an aligned
        Type / Size (bytes) / Name table. Sizes are right-aligned and directories
        show a dash instead of a file size. fs.info reports a file's native path and size.
        fs.read accepts strict UTF-8 without null characters, at most 65,536 bytes.
        fs.write creates a new UTF-8 file without a BOM and flushes the contents.
        Existing destinations cause fs.write, fs.copy and fs.move to fail.

        fs.copy copies files in 64 KiB chunks and observes cancellation between
        chunks. A failed or canceled copy can leave a partial newly created
        destination; the source is retained. fs.move uses native same-volume rename
        for files and directories. fs.delete removes one file or an empty directory;
        it does not recurse. Rename/delete open the final reparse point itself.
        These are individual operations, with no transaction or undo facility.

        ## Processes

        `process.list` reports PID, parent PID and image name in an aligned native
        process snapshot table. Numeric columns are right-aligned; names appear last.
        `process.terminate <pid>` opens only the supplied PID, checks its
        critical-process flag and requests termination. Use the PID of a disposable
        process you started yourself. The command rejects PID 0, PID 4, the current
        Seed runtime and critical processes. Windows access checks still apply.
        Process names are informational; termination selects a PID only.

        ## Windows registry

        Keys use uppercase `HKCU\...`, `HKLM\...` aliases or an absolute
        `\Registry\...` native path. HKCU resolves the native process's current user.
        Key and value lookup is case-insensitive after resolving the alias.

        ```powershell
        $seedConsole = './Artifacts/x64/Debug/Seed.Console/Seed.Console.exe'
        $seedKey = 'HKCU\Software\SeedDemo-' + [Guid]::NewGuid().ToString('N')
        & $seedConsole registry.key.create $seedKey
        & $seedConsole registry.set $seedKey Greeting string 'Hello from Seed 🌱'
        & $seedConsole registry.set $seedKey Count dword 42
        & $seedConsole registry.set $seedKey Large qword 18446744073709551615
        & $seedConsole registry.list $seedKey
        & $seedConsole registry.get $seedKey Greeting
        & $seedConsole registry.get $seedKey Large
        & $seedConsole registry.value.delete $seedKey Count
        & $seedConsole registry.key.delete $seedKey
        ```

        registry.list lists immediate subkeys and value names with native numeric
        type codes. registry.get returns REG_SZ/REG_EXPAND_SZ text without expanding
        environment variables, and unsigned decimal DWORD/QWORD values. Other types
        return a type-and-byte-count summary. Malformed string data is rejected.
        Individual registry query buffers are limited to 64 KiB.

        registry.set accepts string (REG_SZ), dword (0..4294967295) and qword
        (0..18446744073709551615), with unsigned decimal input. The key must exist;
        a matching value is replaced. registry.key.create creates or opens one key
        under an existing parent. registry.key.delete deletes its values too, but
        fails while subkeys exist. registry.value.delete removes only one value.
        These commands interact with the Windows registry, independently of Seed's
        reflected configuration store.

        ## Bounds and cancellation

        Collected listing output is limited to 524,288 UTF-16 characters. Process
        snapshot buffers are bounded at 16 MiB and retry at most eight times if the
        snapshot changes. Large directory enumerations still use Seed's existing
        materialized directory-query API. Cancellation is cooperative: listings
        check between entries, copies between chunks, and mutations before the NT
        call. Completed mutations are retained if the console disconnects.
        """;
}
