using Seed.Core.RTTI;
using Seed.Core.Threading;

namespace Seed.Core.Services.Diagnostic;

[SerializableStruct("Seed::Core::Services::Diagnostic::ContextRecordDiagnosticData")]
public struct ContextRecordDiagnosticData
{
    [SerializableStructField]
    public UniversalTypeIdentifier ContextType { get; set; }

    [SerializableStructField]
    public UInt64 ContextId { get; set; }
}

[SerializableStruct("Seed::Core::Services::Diagnostic::ContextBlockViewDiagnosticData")]
public struct ContextBlockViewDiagnosticData
{
    [SerializableStructField]
    public ID ContextBlockId { get; set; }

    [SerializableStructField]
    public ID ParentContextBlockId { get; set; }

    [SerializableStructField]
    public Boolean IsManual { get; set; }

    [SerializableStructField]
    public EThreadableTypes ThreadableType { get; set; }

    [SerializableStructField]
    public List<ContextRecordDiagnosticData> Contexts { get; set; }
}

[SerializableStruct("Seed::Core::Services::Diagnostic::ServicesThreadableContextDiagnosticProviderData")]
public struct ServicesThreadableContextDiagnosticProviderData
{
    [SerializableStructField]
    public ID Id { get; set; }

    [SerializableStructField]
    public ID DependencyInjectionId { get; set; }

    [SerializableStructField]
    public String ScopeName { get; set; }
}

[SerializableStruct("Seed::Core::Services::Diagnostic::DependencyInjectionServiceDiagnosticData")]
public struct DependencyInjectionServiceDiagnosticData
{
    [SerializableStructField]
    public UInt64 ServiceAddress { get; set; }

    [SerializableStructField]
    public UniversalTypeIdentifier ServiceType { get; set; }

    [SerializableStructField]
    public UniversalTypeIdentifier ResolverType { get; set; }

    [SerializableStructField]
    public List<UniversalTypeIdentifier> Dependencies { get; set; }

    [SerializableStructField]
    public UniversalTypeIdentifier InterfaceType { get; set; }

    [SerializableStructField]
    public Boolean IsStoredAsDynamic { get; set; }

    [SerializableStructField]
    public Boolean IsInitialized { get; set; }

    [SerializableStructField]
    public Boolean IsReady { get; set; }
}

[SerializableStruct("Seed::Core::Services::Diagnostic::DepencyInjectionDiagnosticData")]
public struct DepencyInjectionDiagnosticData
{
    [SerializableStructField]
    public ID DepencyInjectionId { get; set; }

    [SerializableStructField]
    public ID ParrentDepencyInjectionId { get; set; }

    [SerializableStructField]
    public UInt64 ConstructedServicesCount { get; set; }

    [SerializableStructField]
    public UInt64 ConstructibleServicesCount { get; set; }

    [SerializableStructField]
    public UInt64 ReadyServicesCount { get; set; }

    [SerializableStructField]
    public UInt64 ReadyabilityServicesCount { get; set; }

    [SerializableStructField]
    public List<DependencyInjectionServiceDiagnosticData> Services { get; set; }
}
