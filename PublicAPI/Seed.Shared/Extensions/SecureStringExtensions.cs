using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Shared.Extensions;

public static class SecureStringExtensions
{
    /// <summary>
    /// Converts a secure string to a plain string
    /// </summary>
    /// <param name="secureStr">Secure string</param>
    /// <returns>Plain string</returns>
    public static String ToPlainString(this System.Security.SecureString secureStr)
    {
        String plainStr = new System.Net.NetworkCredential(string.Empty, secureStr).Password;

        return plainStr;
    }

    /// <summary>
    /// Converts a plain string to a secure string
    /// </summary>
    /// <param name="plainStr">Plain string</param>
    /// <returns>Secure string</returns>
    public static System.Security.SecureString ToSecureString(this String plainStr)
    {
        var secStr = new System.Security.SecureString(); secStr.Clear();
        
        foreach (char c in plainStr.ToCharArray())
        {
            secStr.AppendChar(c);
        }
        
        return secStr;
    }
}
