using Seed.CTTI;

namespace Seed.Core.Time;

/// <summary>
/// Time class, optimized for quick arithmetic
/// </summary>
[SerializableStruct("Seed::Core::Time::DateTime")]
public struct DateTime
{
    [SerializableStructFieldManual("Ticks", "Seed::Core::SizeT")]
    public ID Ticks { get; set; }

    public DateTime() { Ticks = 0; }

    public DateTime(ID ticks) { Ticks = ticks; }

    public DateTime(System.DateTime dateTime) { Ticks = (ulong)dateTime.ToFileTime(); }

    public static implicit operator System.DateTime(DateTime time)
    {
        return System.DateTime.FromFileTime((long)time.Ticks);
    }

    public static implicit operator DateTime(System.DateTime time)
    {
        return new DateTime(time);
    }

    public override string ToString()
    {
        return System.DateTime.FromFileTime((long)Ticks).ToString();
    }
}

/// <summary>
/// Time class, optimized for quick arithmetic
/// </summary>
[SerializableStruct("Seed::Core::Time::TimeSpan")]
public struct TimeSpan
{
    [SerializableStructFieldManual("Ticks", "Seed::Core::SizeT")]
    public ID Ticks { get; set; }

    public TimeSpan() { Ticks = 0; }

    public TimeSpan(ID ticks) { Ticks = ticks; }

    public TimeSpan(System.TimeSpan dateTime) { Ticks = (ulong)dateTime.Ticks; }

    public static implicit operator System.TimeSpan(TimeSpan time)
    {
        return new System.TimeSpan((long)time.Ticks);
    }

    public static implicit operator TimeSpan(System.TimeSpan time)
    {
        return new TimeSpan(time);
    }

    public override string ToString()
    {
        return new System.TimeSpan((long)Ticks).ToString();
    }
}
