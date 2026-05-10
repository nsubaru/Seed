using Seed.Core.Crypt.Algorithms;

namespace Seed.Serialization.CustomSerilizators;

public class TotpSha1Serializator : ICustomClassSerializator
{
    public Type ClrType => typeof(TotpSha1);

    public string TypeName => "Seed::Core::Crypt::Algorithms::TOTP<Seed::Core::Crypt::Hash::SHA1Hash>";

    public Type[] DependableTypes => Array.Empty<Type>();

    public object Deserialize(Stream serializationStream, ISeedSerializator serializator)
    {
        dynamic secretHash = new List<UInt8>();
        serializator.Deserialize(ref secretHash, serializationStream);

        dynamic passwordSize = (UInt8)0;
        serializator.Deserialize(ref passwordSize, serializationStream);

        dynamic startTime = (UInt64)0;
        serializator.Deserialize(ref startTime, serializationStream);

        dynamic timeIntervalInSeconds = (SizeT)0;
        serializator.Deserialize(ref timeIntervalInSeconds, serializationStream);

        return new TotpSha1
        {
            SecretHash = (List<UInt8>)secretHash,
            PasswordSize = (UInt8)passwordSize,
            StartTime = (UInt64)startTime,
            TimeIntervalInSeconds = (SizeT)timeIntervalInSeconds
        };
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializator serializator)
    {
        var totp = (TotpSha1)data;

        dynamic secretHash = totp.SecretHash ?? new List<UInt8>();
        serializator.Serialize(secretHash, serializationStream);

        dynamic passwordSize = totp.PasswordSize;
        serializator.Serialize(passwordSize, serializationStream);

        dynamic startTime = totp.StartTime;
        serializator.Serialize(startTime, serializationStream);

        dynamic timeIntervalInSeconds = totp.TimeIntervalInSeconds;
        serializator.Serialize(timeIntervalInSeconds, serializationStream);
    }
}
