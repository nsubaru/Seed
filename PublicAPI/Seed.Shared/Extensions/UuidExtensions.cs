using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Shared.Extensions;

public static class UuidExtensions
{
    /// <summary>
    /// Packs two UInt64 values into a single UUID
    /// </summary>
    /// <param name="first">Left UUID part</param>
    /// <param name="second">Right UUID part</param>
    /// <returns>Combined UUID</returns>
    public unsafe static Guid PackUuid(UInt64 first, UInt64 second)
    {
        ulong* ptr = stackalloc ulong[2];
        
        ptr[0] = first;
        ptr[1] = second;

        return *(Guid*)ptr;
    }

    /// <summary>
    /// Unpacks a UUID into two UInt64 values
    /// </summary>
    /// <param name="guid">Combined UUID</param>
    /// <returns>Left and right parts as two UInt64</returns>
    public unsafe static (UInt64 First, UInt64 Second) UnpackUuid(Guid guid)
    {
        ulong* ptr = stackalloc ulong[2];
        *(Guid*)ptr = guid;

        return (ptr[0], ptr[1]);
    }
}
