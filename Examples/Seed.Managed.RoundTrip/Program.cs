using Seed.CTTI;
using Seed.RTTI;

var path     = args.Length > 0 ? args[0] : "Artifacts/tests/seed-rtti.dat";
var metadata = File.ReadAllBytes(path);

using var section = new RTTISection(metadata);

var types     = section.AllRegisteredTypes.Where(type => type.IsValid).ToArray();
var roundTrip = types.Single(type => type.TypeName == "Seed::Runtime::RoundTripMessage");

Console.WriteLine($"Read {types.Length} live Seed RTTI records; round-trip contract: {roundTrip.TypeName}.");

using var serializer = new SeedBinarySerializer(section, new EmptyServices());

var expected = new RoundTripMessage
{
    Sequence = 42,
    Message = "Seed managed round trip"
};

using var stream = new MemoryStream();

serializer.Serialize(stream, expected, roundTrip);
stream.Position = 0;

var actual = serializer.Deserialize<RoundTripMessage>(stream, roundTrip);

if (actual.Sequence != expected.Sequence || actual.Message != expected.Message)
{
    throw new InvalidDataException("Binary round trip did not preserve the reflected fields.");
}

Console.WriteLine($"Managed binary round trip passed: {stream.Length} bytes.");

var leaseStatus = types.Single(type => type.TypeName == "Seed::Core::Services::Dhcp::LeaseMaintenanceStatus");
var phaseField  = leaseStatus.Fields.Single(field => field.FieldName == "Phase");

if (phaseField.TypeClass != ETypes.String)
{
    throw new InvalidDataException("DHCP phase metadata no longer exposes a serialized string.");
}

var leaseExpected = new LeaseStatusProbe
{
    InterfaceIndex = 17,
    Phase          = "bound",
    Address        = "192.0.2.17",
    LastError      = "",
    IsRunning      = true
};

using var leaseStream = new MemoryStream();
serializer.Serialize(leaseStream, leaseExpected, leaseStatus);
leaseStream.Position = 0;

var leaseActual = serializer.Deserialize<LeaseStatusProbe>(leaseStream, leaseStatus);

if (leaseActual.InterfaceIndex != leaseExpected.InterfaceIndex || leaseActual.Phase != leaseExpected.Phase || leaseActual.Address != leaseExpected.Address || leaseActual.LastError != leaseExpected.LastError || leaseActual.IsRunning != leaseExpected.IsRunning || leaseActual.CleanupPending != leaseExpected.CleanupPending)
{
    throw new InvalidDataException("The reflected DHCP status round trip did not preserve its fields.");
}

Console.WriteLine("Managed DHCP status/UniString metadata and binary round trip passed.");

if (args.Skip(1).Contains("--verify-malformed", StringComparer.Ordinal))
{
    void Reject(byte[] bytes)
    {
        try
        {
            using var invalid = new RTTISection(bytes);
            throw new InvalidOperationException("Malformed metadata was accepted.");
        }
        catch (InvalidDataException)
        {
        }
    }

    Reject(Array.Empty<byte>());
    Reject(metadata.Take(32).ToArray());

    var marker  = System.Text.Encoding.ASCII.GetBytes("SEED_RXX_RTTI\0");
    int first   = metadata.AsSpan().IndexOf(marker);
    int header  = first + 1 + metadata.AsSpan(first + 1).IndexOf(marker);
    var damaged = (byte[])metadata.Clone();

    damaged[first] = 0;
    Reject(damaged);
    damaged = (byte[])metadata.Clone();

    Array.Fill(damaged, (byte)0xFF, header + 30, 8);
    Reject(damaged);
    damaged = (byte[])metadata.Clone();

    Array.Fill(damaged, (byte)0, header + 22, 8);
    Reject(damaged);
    Array.Fill(metadata, (byte)0);

    if (section.AllRegisteredTypes.Single(type => type.TypeName == roundTrip.TypeName).FullTypeId != roundTrip.FullTypeId)
    {
        throw new InvalidOperationException("External byte-array changes affected the owning metadata snapshot.");
    }

    Console.WriteLine("Malformed header, pointer table, origin and owning-copy checks passed.");
}

var serverUrl = args.Skip(1).FirstOrDefault(argument => !argument.StartsWith("--", StringComparison.Ordinal));

if (serverUrl is not null)
{
    using var client = new HttpClient();
    using var response = await client.PostAsync(new Uri(new Uri(serverUrl), "/json"), new StringContent(System.Text.Json.JsonSerializer.Serialize(expected), System.Text.Encoding.UTF8, "application/json"));

    response.EnsureSuccessStatusCode();
    var payload = await response.Content.ReadAsStringAsync();
    var returned = System.Text.Json.JsonSerializer.Deserialize<RoundTripMessage>(payload)!;

    if (returned.Sequence != expected.Sequence || returned.Message != expected.Message)
    {
        throw new InvalidDataException("Native JSON round trip did not preserve the reflected fields.");
    }

    Console.WriteLine("Native HTTP JSON round trip passed.");

    foreach (string message in new[] { expected.Message, "",  "Seed \U0001F338\0 binary" })
    {
        var binaryExpected = new RoundTripMessage
        {
            Sequence = -42,
            Message = message
        };
        
        using var binaryStream = new MemoryStream();

        serializer.Serialize(binaryStream, binaryExpected, roundTrip);

        byte[] wire = binaryStream.ToArray();

        using var binaryResponse = await client.PostAsync(new Uri(new Uri(serverUrl), "/binary"), new ByteArrayContent(wire));

        binaryResponse.EnsureSuccessStatusCode();

        using var returnedStream = new MemoryStream(await binaryResponse.Content.ReadAsByteArrayAsync());

        var binaryReturned = serializer.Deserialize<RoundTripMessage>(returnedStream, roundTrip);

        if (binaryReturned.Sequence != binaryExpected.Sequence || binaryReturned.Message != binaryExpected.Message)
        {
            throw new InvalidDataException("Native binary round trip did not preserve the reflected fields.");
        }

        using var invalidResponse = await client.PostAsync(new Uri(new Uri(serverUrl), "/binary"), new ByteArrayContent(wire[..^1]));

        if (invalidResponse.IsSuccessStatusCode)
        {
            throw new InvalidDataException("Truncated native binary input was accepted.");
        }

        wire[0] ^= 0xFF;

        using var wrongIdentity = await client.PostAsync(new Uri(new Uri(serverUrl), "/binary"), new ByteArrayContent(wire));

        if (wrongIdentity.IsSuccessStatusCode)
        {
            throw new InvalidDataException("Incompatible native binary metadata identity was accepted.");
        }
    }

    Console.WriteLine("Native/C# binary round trips and malformed-input rejection passed.");
}

internal sealed class EmptyServices : IServiceProvider
{
    public object? GetService(Type type) => null;
}

/// <summary>
/// Demonstration mapping for reflected DHCP status; constructing it never starts a lease operation.
/// </summary>
[SerializableStruct("Seed::Core::Services::Dhcp::LeaseMaintenanceStatus")]
public sealed class LeaseStatusProbe
{
    /// <summary>
    /// Selected Windows IPv4 interface index; the demonstration uses a synthetic value.
    /// </summary>
    [SerializableStructField]
    public ulong InterfaceIndex { get; set; }

    /// <summary>
    /// Current maintenance phase represented as Unicode text by native UniString metadata.
    /// </summary>
    [SerializableStructField]
    public string Phase { get; set; } = "";

    /// <summary>
    /// Assigned IPv4 address, or empty text before acquisition.
    /// </summary>
    [SerializableStructField]
    public string Address { get; set; } = "";

    /// <summary>
    /// Most recent native failure text, or empty text when no failure is recorded.
    /// </summary>
    [SerializableStructField]
    public string LastError { get; set; } = "";

    /// <summary>
    /// Whether the selected maintenance worker is running.
    /// </summary>
    [SerializableStructField]
    public bool IsRunning { get; set; }

    /// <summary>
    /// Whether the operation still owns transient interface configuration awaiting cleanup.
    /// </summary>
    [SerializableStructField]
    public bool CleanupPending { get; set; }
}

/// <summary>
/// Message mapped to the native demonstration's reflected contract.
/// </summary>
[SerializableStruct("Seed::Runtime::RoundTripMessage")]
public sealed class RoundTripMessage
{
    /// <summary>
    /// Caller sequence number preserved by native and managed serialization.
    /// </summary>
    [SerializableStructField]
    public long Sequence { get; set; }

    /// <summary>
    /// Unicode text preserved by the round-trip endpoints.
    /// </summary>
    [SerializableStructField]
    public string Message { get; set; } = "";
}
