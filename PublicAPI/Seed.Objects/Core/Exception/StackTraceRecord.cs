using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Exception;

[SerializableStruct("Seed::Core::Exception::StackTraceRecord")]
public struct StackTraceRecord
{
    /// <summary>
    /// The actual number of function addresses
    /// </summary>
    [SerializableStructField]
	public SizeT Count { get; set; }

    /// <summary>
    /// Function addresses
    /// </summary>
    [SerializableStructField]
    public SizeT[] FunctionAddress {get; set; }
};
