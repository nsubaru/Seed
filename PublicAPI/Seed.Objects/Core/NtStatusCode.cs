namespace Seed.Core;

[SerializableStruct("Seed::Core::NtStatusCode")]
public struct NtStatusCode
{
    [SerializableStructField]
    public Int32 Code
    {
        get;
        set;
    }
}
