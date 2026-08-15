namespace Seed.Core.Threading;

/// <summary>
/// Threadable entity states
/// </summary>
[SerializableEnum("Seed::Core::Threading::EThreadableStates")]
public enum EThreadableStates : UInt8
{
    NotStarted, Starting, Executing, Paused, ExceptionProcessing, Interrupted, Ended, Killed, Unknown = 255
};
