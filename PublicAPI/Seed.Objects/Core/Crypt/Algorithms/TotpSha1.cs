namespace Seed.Core.Crypt.Algorithms;

public struct TotpSha1
{
    public List<UInt8> SecretHash { get; set; }

    public UInt8 PasswordSize { get; set; }

    public UInt64 StartTime { get; set; }

    public SizeT TimeIntervalInSeconds { get; set; }
}
