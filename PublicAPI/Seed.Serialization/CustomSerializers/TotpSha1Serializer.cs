using Seed.Core.Crypt.Algorithms;

namespace Seed.Serialization.CustomSerializers;

public class TotpSha1Serializer : ICustomClassSerializer
{
    public Type ClrType => typeof(TotpSha1);

    public string TypeName => "Seed::Core::Crypt::Algorithms::TOTP<Seed::Core::Crypt::Hash::SHA1Hash>";

    public Type[] DependableTypes => Array.Empty<Type>();

    /// <summary>
    /// Reconstructs the mapped value using statically dispatched calls to the session serializer.
    /// </summary>
    /// <param name="serializationStream">Stream positioned at the native value payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    /// <returns>The decoded managed value.</returns>
    public object Deserialize(Stream serializationStream, ISeedSerializer serializer)
    {
        object secretHash = new List<UInt8>();
        serializer.Deserialize(ref secretHash, serializationStream);

        object passwordSize = (UInt8)0;
        serializer.Deserialize(ref passwordSize, serializationStream);

        object startTime = (UInt64)0;
        serializer.Deserialize(ref startTime, serializationStream);

        object timeIntervalInSeconds = (SizeT)0;
        serializer.Deserialize(ref timeIntervalInSeconds, serializationStream);

        return new TotpSha1
        {
            SecretHash            = (List<UInt8>)secretHash,
            PasswordSize          = (UInt8)passwordSize,
            StartTime             = (UInt64)startTime,
            TimeIntervalInSeconds = (SizeT)timeIntervalInSeconds
        };
    }

    /// <summary>
    /// Writes the mapped value while keeping nested calls outside the dynamic overload binder.
    /// </summary>
    /// <param name="data">Managed value matching this serializer's CLR type.</param>
    /// <param name="serializationStream">Destination for the native payload.</param>
    /// <param name="serializer">Session serializer used for nested fields.</param>
    public void Serialize(dynamic data, Stream serializationStream, ISeedSerializer serializer)
    {
        var totp = (TotpSha1)data;
        
        object secretHash = totp.SecretHash ?? new List<UInt8>();
        serializer.Serialize(secretHash, serializationStream);

        object passwordSize = totp.PasswordSize;
        serializer.Serialize(passwordSize, serializationStream);

        object startTime = totp.StartTime;
        serializer.Serialize(startTime, serializationStream);

        object timeIntervalInSeconds = totp.TimeIntervalInSeconds;
        serializer.Serialize(timeIntervalInSeconds, serializationStream);
    }
}
