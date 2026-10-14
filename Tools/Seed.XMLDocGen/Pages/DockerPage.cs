namespace Seed.XMLDocGen;

internal sealed class DockerPage : ISourcePage
{
    public string FileName => "docker.md";

    public string Title => "Windows Docker builds and tests";

    public string Render(PageContext context) => """
        # Windows Docker builds and tests

        Use a Docker Windows-container engine on a compatible x64 Windows host.
        The default images use Windows Server Core 2025. Check Microsoft's
        [host/image compatibility matrix](https://learn.microsoft.com/en-us/virtualization/windowscontainers/deploy-containers/version-compatibility)
        before selecting process isolation. Use -Isolation hyperv when supported
        and needed by the host. Allocate adequate Docker/VM memory, CPU and disk
        space; native builds use MSBuild /m and compiler /MP with all available cores.
        Hyper-V builds default to a 32 GiB container memory limit and test containers
        to 8 GiB; -BuildMemory and -TestMemory override these limits. Process isolation
        uses the engine's default resource limits unless explicitly overridden.
        The source context excludes Git history, build output and local private keys.

        Docker/BuildTools.Dockerfile installs VS 2026 Build Tools, MSVC v145,
        the Windows 26100 SDK, managed SDK/NuGet build integration and test tools
        over Microsoft's .NET 10 SDK image. A managed compilation probe checks
        Visual Studio MSBuild SDK resolution before the source build starts.
        Its inputs are public Microsoft downloads. Docker/Seed.Dockerfile regenerates
        ntdll.lib from the shipped definitions and builds Seed.slnx from source.
        The ordinary native subsystem and custom runtime settings are retained.
        The external starter launches the native image in the container; no Win32
        networking or TLS library is added to the native process.

        ```powershell
        ./Tools/Build-SeedDocker.ps1 -Configuration Release -Target runtime
        ./Tools/Build-SeedDocker.ps1 -Configuration Release -Target test -Image seed:tests-local -SkipBuildTools
        ./Tools/Test-SeedDocker.ps1 -Configuration Release -NoBuild
        ```

        The first build creates seed-build-tools:vs2026. Subsequent source builds
        can use -SkipBuildTools with that image. Build tools and downloaded packages
        remain in build/test stages; the runtime target contains the runtime,
        console/starter, required managed dependencies and license notices.
        The build copies the x64 MSVC runtime DLLs beside each external starter
        and into the console payload. No redistributable installer runs in the
        runtime image. These DLLs remain outside Seed's native executable, which
        imports only ntdll. Rebuild images when updating vendor runtime dependencies.
        -SdkImage, -RuntimeImage and -BuildToolsImage allow explicitly selected
        public versions or digests. Record these inputs for reproducible release
        validation; moving default tags alone do not pin a toolchain.

        The MSBuild property SeedSubsystem selects Native (the default), Console
        (Windows CUI), or Windows (Windows GUI). It changes linker metadata while
        retaining EntryPoint and the custom compiler/runtime settings:

        ```powershell
        MSBuild Seed.slnx /m /restore /p:Platform=x64 /p:Configuration=Release /p:SeedSubsystem=Console
        ./Tools/Build-SeedDocker.ps1 -Subsystem Console -Target runtime -Image seed:cui-local -SkipBuildTools
        ./Tools/Build-SeedDocker.ps1 -Subsystem Windows -Target runtime -Image seed:gui-local -SkipBuildTools
        ```

        The starter uses NT process creation for all three image settings. A GUI
        subsystem does not add a Seed UI. Keep the native default for boot/native
        process validation, and validate the chosen subsystem with its matching
        test image. The build records the selection in container-build.json.
        Windows loads KERNEL32/KERNELBASE for CUI/GUI images even when their
        explicit import table contains only ntdll. Use Native for the validation
        requirement that the process loads only Seed.Runtime.exe and ntdll.dll.

        The test target runs as ContainerUser and verifies that its token is not
        elevated. The default run executes the KMS E2E suite and all shipped native
        self-tests. Fast reboot uses reload and must retain the process while changing
        the composition. Configuration/logging, reconnect, reflection and available
        commands are exercised. DHCP tests remain idle or reject invalid selections;
        they do not change the host or container interfaces.

        ```powershell
        ./Tools/Test-SeedDocker.ps1 -NoBuild -Class CommandTests -Method StatusHasStableLabelsAndFastRebootKeepsTheProcess -NativeWildcard '*Crypt*'
        ```

        Tests own their native processes and use named pipes inside the container.
        No host KMS pipe is mapped. The wrapper creates a uniquely named container,
        copies TRX/NUnit reports, command transcripts, account identity and import
        evidence and toolchain versions into Artifacts/docker/<configuration>/<run>, then removes only
        that container. Reports are copied after failures too. No host bind mount
        is required, which also avoids Dev Drive/minifilter mount restrictions.

        The runtime target starts as ContainerUser with writable C:\Seed.Data.
        Its external supervisor waits for the native process and exits with it.
        The demonstration HTTP listener binds to loopback on port 8080 by default;
        HTTPS is disabled until explicit certificate/key configuration is supplied.

        ```powershell
        docker run --detach --name seed-demo --isolation process seed:runtime-local
        docker exec seed-demo C:\Seed\Console\Seed.Console.exe status.kernel
        docker exec seed-demo powershell -NoProfile -Command "[Text.Encoding]::UTF8.GetString((Invoke-WebRequest -UseBasicParsing http://127.0.0.1:8080/health).RawContentStream.ToArray())"
        docker exec seed-demo C:\Seed\Console\Seed.Console.exe reload
        docker exec seed-demo C:\Seed\Console\Seed.Console.exe shutdown
        docker wait seed-demo
        docker rm seed-demo
        ```

        Check the supervisor startup message before executing commands. To reach
        HTTP from outside the container, deliberately configure host.http.bindAddress,
        save and reload, and create the container with the chosen port mapping.
        HTTPS additionally needs a locally generated development identity or the
        user's certificate/key and trust settings. Keep private keys outside image
        build contexts and copied artifacts. Container writable data is ephemeral
        unless explicitly persisted. Use KMS shutdown for orderly service cleanup;
        forcibly stopping/removing a container does not guarantee orderly shutdown.

        The artifacts target exports complete build outputs, generated documentation
        and matching native PDB/MAP files without running tests:

        ```powershell
        ./Tools/Build-SeedDocker.ps1 -Target artifacts -Image seed:artifacts-local -SkipBuildTools
        docker create --name seed-export seed:artifacts-local
        docker cp seed-export:C:/out ./Artifacts/container-export
        docker rm seed-export
        ```

        The optional Docker CI workflow is manually dispatched on a dedicated
        self-hosted Windows x64 runner labeled seed-docker. Configure that runner's
        Windows Docker engine and adequate resources. It builds and runs the tests
        and uploads reports; it does not publish an image or use private feeds.
        Do not enable untrusted pull-request execution on a privileged Docker runner.

        Windows/.NET/Visual Studio container prerequisites and redistribution terms
        remain those of their respective vendors. See Microsoft's
        [Build Tools container guide](https://learn.microsoft.com/en-us/visualstudio/install/build-tools-container).
        The external starter's C++ runtime remains subject to Microsoft's
        [redistribution terms](https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files).
        Seed's contribution and third-party rights audit remains a release gate.
        See [E2E tests](e2e.md), [native testing](testing.md) and [configuration](configuration.md).
        """;
}
