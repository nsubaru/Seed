using LanguageExt.TypeClasses;
using Seed.CTTI;

namespace Seed.Core.Time;

/// <summary>
/// Time class, optimized for quick arithmetic
/// </summary>
[SerializableStruct("Seed::Core::Time::DateTime")]
public struct DateTime
{
    [SerializableStructField]
    public ID Ticks { get; set; }

    public DateTime()
    {
        Ticks = 0;
    }

    public DateTime(ID ticks)
    {
        Ticks = ticks;
    }

    public DateTime(System.DateTime dateTime)
    {
        Ticks = dateTime != System.DateTime.MinValue ? (ulong)dateTime.ToFileTime() : 0;
    }

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

    public static DateTime MinValue = new DateTime(0);
}

/// <summary>
/// Time class, optimized for quick arithmetic
/// </summary>
[SerializableStruct("Seed::Core::Time::TimeSpan")]
public struct TimeSpan
{
    [SerializableStructField]
    public ID Ticks { get; set; }

    public TimeSpan()
    {
        Ticks = 0;
    }

    public TimeSpan(ID ticks)
    {
        Ticks = ticks;
    }

    public TimeSpan(System.TimeSpan dateTime)
    {
        Ticks = (ulong)dateTime.Ticks;
    }

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

    public static TimeSpan Zero = new TimeSpan(0);
}
