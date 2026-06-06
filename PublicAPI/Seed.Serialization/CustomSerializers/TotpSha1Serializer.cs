using Seed.Core.Crypt.Algorithms;

namespace Seed.Serialization.CustomSerializers;

public class TotpSha1Serializer : ICustomClassSerializer
{
    public Type ClrType => typeof(TotpSha1);

    public string TypeName => "Seed::Core::Crypt::Algorithms::TOTP<Seed::Core::Crypt::Hash::SHA1Hash>";

    public Type[] DependableTypes => Array.Empty<Type>();

    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        dynamic secretHash = new List<UInt8>();
        serializer.Deserialize(ref secretHash, serializationStream);

        dynamic passwordSize = (UInt8)0;
        serializer.Deserialize(ref passwordSize, serializationStream);

        dynamic startTime = (UInt64)0;
        serializer.Deserialize(ref startTime, serializationStream);

        dynamic timeIntervalInSeconds = (SizeT)0;
        serializer.Deserialize(ref timeIntervalInSeconds, serializationStream);

        return new TotpSha1
        {
            SecretHash = (List<UInt8>)secretHash,
            PasswordSize = (UInt8)passwordSize,
            StartTime = (UInt64)startTime,
            TimeIntervalInSeconds = (SizeT)timeIntervalInSeconds
        };
    }

    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var totp = (TotpSha1)data;

        dynamic secretHash = totp.SecretHash ?? new List<UInt8>();
        serializer.Serialize(secretHash, serializationStream);

        dynamic passwordSize = totp.PasswordSize;
        serializer.Serialize(passwordSize, serializationStream);

        dynamic startTime = totp.StartTime;
        serializer.Serialize(startTime, serializationStream);

        dynamic timeIntervalInSeconds = totp.TimeIntervalInSeconds;
        serializer.Serialize(timeIntervalInSeconds, serializationStream);
    }
}
