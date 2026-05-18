namespace Seed.Core.Services.Diagnostic;

[SerializableEnum("Seed::Core::Services::Diagnostic::EDiagnosticProviderFlags")]
[Flags]
public enum EDiagnosticProviderFlags : UInt32
{
    RequireSuspendingOtherThreads,
    ExposeSensitiveData
}
