using System.IO.Pipes;
using System.Runtime.CompilerServices;
using System.Text;

namespace Seed.KMS.Transport;

/// <summary>
/// One text datagram emitted while the runtime executes a console command.
/// </summary>
public sealed record KmsDatagram
{
    /// <summary>
    /// Owning text payload decoded from a complete message-pipe packet.
    /// </summary>
    public required string Text
    {
        get;
        init;
    }

    /// <summary>
    /// Whether the runtime reported this datagram as an error.
    /// </summary>
    public required bool IsError
    {
        get;
        init;
    }
}

/// <summary>
/// Sequential, unauthenticated console transport for a local Seed KMS endpoint.
/// </summary>
public sealed class KmsClient : IAsyncDisposable
{
    /// <summary>
    /// Default local endpoint name, without a machine or directory prefix.
    /// </summary>
    public const string DefaultPipeName = "Seed.KMS.v1";

    /// <summary>
    /// Largest complete message-pipe packet, including its five-byte header, in bytes.
    /// </summary>
    public const int MaximumMessageBytes = 65535;

    private static readonly Encoding TextEncoding = new UnicodeEncoding(false, false, true);
    private readonly SemaphoreSlim commandGate = new(1, 1);
    private NamedPipeClientStream pipe;
    private string pipeName;
    private bool connected;

    /// <summary>
    /// Creates a disconnected client restricted to a local pipe name.
    /// </summary>
    /// <param name="pipeName">Local endpoint name without a path or machine prefix.</param>
    public KmsClient(string pipeName = DefaultPipeName)
    {
        if (string.IsNullOrWhiteSpace(pipeName) || pipeName.IndexOfAny(['\\', '/']) >= 0)
        {
            throw new ArgumentException("A local KMS endpoint name is required.", nameof(pipeName));
        }

        this.pipeName = pipeName;
        pipe = CreatePipe();
    }

    /// <summary>
    /// Connects and verifies the Seed protocol initialization marker without a login exchange.
    /// </summary>
    /// <param name="timeout">Maximum connection and initialization duration.</param>
    /// <param name="cancellationToken">Cancellation that closes this connection attempt.</param>
    /// <returns>A task completing when a version-one Seed session is ready.</returns>
    public async Task ConnectAsync(TimeSpan timeout, CancellationToken cancellationToken = default)
    {
        if (timeout <= TimeSpan.Zero)
        {
            throw new ArgumentOutOfRangeException(nameof(timeout));
        }

        if (connected)
        {
            throw new InvalidOperationException("The Seed KMS session is already connected.");
        }

        using var deadline = CancellationTokenSource.CreateLinkedTokenSource(cancellationToken);
        deadline.CancelAfter(timeout);
        try
        {
            for (; ; )
            {
                try
                {
                    await pipe.ConnectAsync(deadline.Token).ConfigureAwait(false);
                    pipe.ReadMode = PipeTransmissionMode.Message;
                    var marker = await ReadPacketAsync(deadline.Token).ConfigureAwait(false);
                    if (marker.Category != 1 || marker.Subtype != 5 || marker.Text != "SEED_KMS/1")
                    {
                        throw new InvalidDataException("The endpoint did not initialize a Seed KMS version-one session.");
                    }

                    connected = true;

                    return;
                }
                catch (IOException) when (!deadline.IsCancellationRequested)
                {
                    // A restarting runtime can close the old pipe before initialization.
                    // No command has been sent, so opening a fresh session is safe.
                    await pipe.DisposeAsync().ConfigureAwait(false);
                    pipe = CreatePipe();
                    await Task.Delay(TimeSpan.FromMilliseconds(100), deadline.Token).ConfigureAwait(false);
                }
            }
        }
        catch
        {
            await pipe.DisposeAsync().ConfigureAwait(false);
            throw;
        }
    }

    private NamedPipeClientStream CreatePipe() => new(".", pipeName, PipeDirection.InOut, PipeOptions.Asynchronous);

    /// <summary>
    /// Executes one command, collecting datagrams until the runtime requests the next command.
    /// </summary>
    /// <param name="command">Command name and optional argument text.</param>
    /// <param name="argumentProvider">Optional handler for additional runtime argument requests.</param>
    /// <param name="cancellationToken">Cancellation that closes the session and interrupts pending transport IO.</param>
    /// <returns>Ordered command output with its error classifications.</returns>
    public async Task<IReadOnlyList<KmsDatagram>> ExecuteAsync(string command, Func<string, CancellationToken, Task<string>>? argumentProvider = null, CancellationToken cancellationToken = default)
    {
        var result = new List<KmsDatagram>();
        int outputBytes = 0;
        await foreach (var datagram in ExecuteStreamingAsync(command, argumentProvider, cancellationToken).ConfigureAwait(false))
        {
            outputBytes = checked(outputBytes + datagram.Text.Length * 2);
            if (outputBytes > 4 * 1024 * 1024)
            {
                throw new InvalidDataException("The command response exceeds the console collection limit; use ExecuteStreamingAsync for live output.");
            }

            result.Add(datagram);
        }

        return result;
    }

    /// <summary>
    /// Executes a command and yields each datagram as it arrives, without accumulating its output.
    /// Disposing an unfinished enumeration or canceling closes the session.
    /// Reload completes after a new runtime composition is ready and this client reconnects.
    /// </summary>
    /// <param name="command">Command name and optional argument text, including logging.follow for a live subscription.</param>
    /// <param name="argumentProvider">Optional handler for additional runtime argument requests.</param>
    /// <param name="cancellationToken">Cancellation that closes the session and interrupts pending transport IO.</param>
    /// <returns>Ordered, individually bounded output packets until command completion or disconnection.</returns>
    public async IAsyncEnumerable<KmsDatagram> ExecuteStreamingAsync(string command, Func<string, CancellationToken, Task<string>>? argumentProvider = null, [EnumeratorCancellation] CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(command))
        {
            throw new ArgumentException("A command is required.", nameof(command));
        }

        await commandGate.WaitAsync(cancellationToken).ConfigureAwait(false);
        bool completed = false;
        bool reload = command == "reload" || command.StartsWith("reload ", StringComparison.Ordinal);
        using var reloadDeadline = reload ? CancellationTokenSource.CreateLinkedTokenSource(cancellationToken) : null;
        reloadDeadline?.CancelAfter(TimeSpan.FromMinutes(1));
        var operationToken = reloadDeadline?.Token ?? cancellationToken;
        try
        {
            if (!connected)
            {
                throw new InvalidOperationException("The Seed KMS session is not connected.");
            }

            string? previousComposition = null;
            string nextPipeName = pipeName;
            if (reload)
            {
                previousComposition = Composition(await ExchangeTextAsync("status.kernel", operationToken).ConfigureAwait(false));
                string settings = await ExchangeTextAsync("configurations.get host.kms.pipeName", operationToken).ConfigureAwait(false);
                const string prefix = "Next KMS endpoint: ";
                string[] endpoints = settings.Split('\n').Where(line => line.StartsWith(prefix, StringComparison.Ordinal)).ToArray();
                if (endpoints.Length != 1)
                {
                    throw new InvalidDataException("The runtime did not identify its saved KMS reload endpoint.");
                }

                nextPipeName = endpoints[0][prefix.Length..].Trim();
                if (string.IsNullOrWhiteSpace(nextPipeName) || nextPipeName.IndexOfAny(['\\', '/']) >= 0)
                {
                    throw new InvalidDataException("The runtime advertised an invalid local KMS reload endpoint.");
                }
            }

            await SendPacketAsync(6, command, operationToken).ConfigureAwait(false);
            if (command == "exit")
            {
                connected = false;
                await pipe.DisposeAsync().ConfigureAwait(false);
                completed = true;
                yield break;
            }

            bool rejected = false;
            for (; ; )
            {
                var packet = await ReadPacketAsync(operationToken).ConfigureAwait(false);
                if (packet.Category == 1 && packet.Subtype == 5)
                {
                    if (packet.Text.Length != 0)
                    {
                        throw new InvalidDataException("Unexpected initialization marker in a command response.");
                    }

                    if (reload && !rejected)
                    {
                        await ReconnectAfterReloadAsync(previousComposition!, nextPipeName, operationToken).ConfigureAwait(false);
                    }

                    completed = true;
                    yield break;
                }

                if (packet.Category == 1 && packet.Subtype == 3 && argumentProvider is not null)
                {
                    var argument = await argumentProvider(packet.Text, operationToken).ConfigureAwait(false);
                    await SendPacketAsync(4, argument, operationToken).ConfigureAwait(false);
                    continue;
                }

                if (packet.Category != 2 || packet.Subtype is < 1 or > 4)
                {
                    throw new InvalidDataException("Unexpected KMS packet in a command response.");
                }

                rejected |= packet.Subtype == 4;
                yield return new KmsDatagram
                {
                    Text = packet.Text,
                    IsError = packet.Subtype == 4
                };
            }
        }
        finally
        {
            try
            {
                if (!completed)
                {
                    connected = false;
                    await pipe.DisposeAsync().ConfigureAwait(false);
                }
            }
            finally
            {
                commandGate.Release();
            }
        }
    }

    private async Task<string> ExchangeTextAsync(string command, CancellationToken token)
    {
        await SendPacketAsync(6, command, token).ConfigureAwait(false);
        var output = new StringBuilder();
        bool rejected = false;
        for (; ; )
        {
            var packet = await ReadPacketAsync(token).ConfigureAwait(false);
            if (packet.Category == 1 && packet.Subtype == 5 && packet.Text.Length == 0)
            {
                if (rejected)
                {
                    throw new InvalidOperationException($"Cannot prepare or confirm reload: {output}");
                }

                return output.ToString();
            }

            if (packet.Category != 2 || packet.Subtype is < 1 or > 4)
            {
                throw new InvalidDataException("Unexpected KMS packet while preparing or confirming reload.");
            }

            if (output.Length + packet.Text.Length > 128 * 1024)
            {
                throw new InvalidDataException("Reload status exceeds its bounded response limit.");
            }

            output.Append(packet.Text);
            rejected |= packet.Subtype == 4;
        }
    }

    private static string Composition(string status)
    {
        const string prefix = "Composition: ";
        string[] identifiers = status.Split('\n').Where(line => line.StartsWith(prefix, StringComparison.Ordinal)).ToArray();
        if (identifiers.Length != 1)
        {
            throw new InvalidDataException("The runtime did not identify one composition.");
        }

        string identifier = identifiers[0][prefix.Length..].Trim();
        if (identifier.Length == 0 || identifier.Any(character => character is < '0' or > '9'))
        {
            throw new InvalidDataException("The runtime composition identifier is malformed.");
        }

        return identifier;
    }

    private async Task ReconnectAfterReloadAsync(string previousComposition, string nextPipeName, CancellationToken token)
    {
        connected = false;
        await pipe.DisposeAsync().ConfigureAwait(false);
        pipeName = nextPipeName;
        pipe = CreatePipe();
        for (; ; )
        {
            token.ThrowIfCancellationRequested();
            await ConnectAsync(TimeSpan.FromMinutes(1), token).ConfigureAwait(false);
            try
            {
                string status = await ExchangeTextAsync("status.kernel", token).ConfigureAwait(false);
                if (Composition(status) != previousComposition)
                {
                    return;
                }
            }
            catch (IOException) when (!token.IsCancellationRequested)
            {
                // The old listener may disappear after accepting this connection.
                // Only status is retried; the reload command is never replayed.
            }

            connected = false;
            await pipe.DisposeAsync().ConfigureAwait(false);
            pipe = CreatePipe();
            await Task.Delay(TimeSpan.FromMilliseconds(100), token).ConfigureAwait(false);
        }
    }

    private async Task SendPacketAsync(byte subtype, string text, CancellationToken token)
    {
        if (text.Contains('\0'))
        {
            throw new ArgumentException("KMS text cannot contain a null character.", nameof(text));
        }

        int length = checked(TextEncoding.GetByteCount(text) + 5);
        if (length > MaximumMessageBytes)
        {
            throw new ArgumentException("KMS packet exceeds the message limit.", nameof(text));
        }

        var bytes = new byte[length];
        bytes[0] = 1;
        bytes[1] = subtype;
        TextEncoding.GetBytes(text, bytes.AsSpan(5));
        await pipe.WriteAsync(bytes, token).ConfigureAwait(false);
    }

    private async Task<(byte Category, byte Subtype, string Text)> ReadPacketAsync(CancellationToken token)
    {
        var bytes = new byte[MaximumMessageBytes];
        int count = 0;
        do
        {
            int read = await pipe.ReadAsync(bytes.AsMemory(count), token).ConfigureAwait(false);
            if (read == 0)
            {
                throw new EndOfStreamException("The Seed KMS session disconnected.");
            }

            count += read;
            if (count == bytes.Length && !pipe.IsMessageComplete)
            {
                throw new InvalidDataException("KMS packet exceeds the message limit.");
            }
        }
        while (!pipe.IsMessageComplete);
        if (count < 5 || (count - 5) % 2 != 0 || bytes[3] != 0 || bytes[4] != 0 || (bytes[0] == 1 ? bytes[2] != 0 : bytes[1] != 0))
        {
            throw new InvalidDataException("Malformed KMS packet header or UTF-16 payload size.");
        }

        string text = TextEncoding.GetString(bytes, 5, count - 5);
        if (text.Contains('\0'))
        {
            throw new InvalidDataException("KMS text contains a null character.");
        }

        return (bytes[0], bytes[0] == 1 ? bytes[1] : bytes[2], text);
    }

    /// <summary>
    /// Closes this local session and releases its pipe handle.
    /// </summary>
    /// <returns>A task completing after the pipe handle is released.</returns>
    public async ValueTask DisposeAsync()
    {
        connected = false;
        await pipe.DisposeAsync().ConfigureAwait(false);
    }
}
