namespace Seed.Core.Threading;

/// <summary>
/// Threadable entity types
/// </summary>
[SerializableEnum("Seed::Core::Threading::EThreadableTypes")]
public enum EThreadableTypes : UInt8
{
    Thread, JobTask, ScheduledTask, IoTask, IOverlapped, AlpcTask, EventTask, AsyncSleep, ExternalCallback, Unknown = 255
};
