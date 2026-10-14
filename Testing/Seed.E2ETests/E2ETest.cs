using Seed.E2E.Infrastructure;

[assembly: CollectionBehavior(DisableTestParallelization = true)]
namespace Seed.E2ETests;
// Each test owns its runtime. This adapts the original generic E2E lifecycle
// pattern to Seed's local unauthenticated transport and ordinary-user startup.
public abstract class E2ETest<T> : IAsyncLifetime
{
    protected RuntimeSession Runtime
    {
        get;
        private set;
    } = null!;

    public async ValueTask InitializeAsync()
    {
        string runtime = Environment.GetEnvironmentVariable("SEED_E2E_RUNTIME") ?? throw new InvalidOperationException("Use Tools/Test-SeedE2E.ps1 or set SEED_E2E_RUNTIME to the matching native image.");
        string output = Environment.GetEnvironmentVariable("SEED_E2E_OUTPUT") ?? Path.Combine(Path.GetTempPath(), "Seed.E2E");
        Runtime = await RuntimeSession.StartAsync(runtime, Path.Combine(output, typeof(T).Name), TestContext.Current.CancellationToken);
    }

    public ValueTask DisposeAsync() => Runtime is null ? ValueTask.CompletedTask : Runtime.DisposeAsync();
}
