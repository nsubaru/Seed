using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Core.Logger;

[SerializableEnum("Seed::Core::Logger::ELoggerLevel")]
[Flags]
public enum ELoggerLevel : UInt16
{
    Debug,
    Verbose,
    Warning,
    Error,
    Critical
}