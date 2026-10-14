namespace Seed.XMLDocGen;

internal sealed class TestAdapterPage : ISourcePage
{
    public string FileName => "test-adapter.md";

    public string Title => "Visual Studio native test adapter and NuGet updates";

    public string Render(PageContext context) => """
        # Visual Studio native test adapter and NuGet updates

        Seed.Testing.TestAdapter adapts the native test discovery/executor from the
        source codebase. A netstandard2.0 adapter runs inside VSTest; the separate
        Seed.Testing.Host .NET 10 process owns the runtime and uses Seed KMS and RTTI.
        Test Explorer can discover, select, run and debug native C++ ITest classes.
        The native runtime still loads only its intended NTAPI dependencies.

        Discovery starts the matching image, exports live fixed-up RTTI, verifies its
        ITest registrations against tests.list, then shuts it down. This is executable
        discovery: only use trusted Seed images. Execution groups cases by image,
        batches exact selections within KMS limits, runs tests.run, and collects every
        NUnit result through tests.result.store. Missing cases fail the run. Failure
        messages and available native stack details appear in Test Explorer.

        Cancel requests host cleanup and stops the owned runtime. The verification
        script also cancels before execution and requires process exit without a
        completed report. Native tests that
        deliberately return false remain skipped; interruption and unexpected failures
        fail. Debug selected tests attaches to the native process before execution.
        Early initialization requires the [boot debugging workflow](boot-debugging.md).
        DIA source navigation and a VSIX extension are outside the current package.

        ## Create or update the package

        1. Build Seed.slnx in Release with MSBuild /m and matching x64 starter outputs.
        2. Run ordinary native tests, E2E tests and Test-SeedTestAdapter.ps1 in Release.
        3. Generate documentation and the dependency inventory. The archive includes
           the adapter, complete host files and starter, native MSBuild props, README,
           Apache license, notices and dependency inventory.
        4. Get NuGet.exe from its [official distribution](https://www.nuget.org/downloads).
           Choose a new semantic version and run the helper from the repository root:

        ```powershell
        ./Tools/Pack-SeedTestAdapter.ps1 -Version 0.1.0-local -NuGetExe C:/Tools/nuget.exe -NoBuild
        ./Tools/Test-SeedTestAdapter.ps1 -Configuration Release
        ```

        The nuspec uses the supplied version, so no assembly version or private feed
        name needs to be edited. The helper creates Artifacts/packages and prints a
        SHA-256 hash. Inspect the package and test its extracted tools/adapter directory
        before distributing it. Install in a native project through NuGet, or use
        VSTest's /TestAdapterPath option with the extracted directory. The native props
        mark the built TargetPath as a test container. Restore/build and refresh Test
        Explorer after updating the version.

        For an explicitly authorized feed test, use:

        ```powershell
        ./Tools/Test-SeedTestAdapterFeed.ps1 -Source '<NuGet v3 feed URL>' -Version '<unique prerelease>' -NuGetExe C:/Tools/nuget.exe
        ```

        This publishes that exact package, restores it into a fresh local directory
        without cache reuse, checks its bytes and runs native discovery/execution from
        the restored contents. Supply credentials through the installed NuGet credential
        provider or normal authenticated environment. Credentials and private endpoint
        configuration do not belong in Seed's tracked nuget.config. Package versions
        are immutable; use a new prerelease for each feed test.

        The public repository has no required private feed. Publishing a release to
        nuget.org is a separate authorized action after release/rights clearance:

        ```powershell
        nuget.exe push Artifacts/packages/Seed.Testing.TestAdapter.<version>.nupkg -Source https://api.nuget.org/v3/index.json -ApiKey <your-api-key>
        ```

        Enter credentials locally; avoid storing them in scripts or transcripts.
        [Microsoft's adapter model](https://github.com/microsoft/vstest/blob/main/docs/RFCs/0004-Adapter-Extensibility.md)
        describes discovery and executor integration.
        """;
}
