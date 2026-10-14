using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Testing
{
    /// <summary>
    /// List of test statuses
    /// </summary>
    [SerializableEnum("Seed::Core::Testing::ETestStatus")]
    public enum ETestStatus : UInt8
    {
        Success,
        Failure,
        Exception,
        Canceled,
        Unknown
    };
}
