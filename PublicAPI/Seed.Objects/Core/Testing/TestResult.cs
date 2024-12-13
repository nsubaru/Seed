using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Testing;

[SerializableStruct("Seed::Core::Testing::TestResult")]
public struct TestResult
{
    [SerializableStructField]
    public ETestStatus Status { get; set; }
    
    [SerializableStructField]
    public String Name { get; set; }
    
    [SerializableStructField]
    public String Message { get; set; }

    [SerializableStructField]
    public TimeSpan Duration { get; set; }
}
