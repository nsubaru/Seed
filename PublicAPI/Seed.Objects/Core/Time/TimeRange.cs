using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Time;

[SerializableStruct("Seed::Core::Time::TimeRange")]
public struct TimeRange()
{
    /// <summary>
    /// Range start
    /// </summary>
    [SerializableStructField]
    public DateTime StartTime = DateTime.MinValue;

    /// <summary>
    /// Duration
    /// </summary>
    [SerializableStructField]
    public TimeSpan Duration = TimeSpan.Zero;


}
