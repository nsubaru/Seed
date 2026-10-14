using System.Text;

namespace Seed.Core;

public readonly struct Utf8String : IEquatable<Utf8String>
{
    private static readonly UTF8Encoding Encoding = new(false, true);

    public Utf8String(byte[] bytes)
    {
        if (bytes == null)
        {
            throw new ArgumentNullException(nameof(bytes));
        }

        // Validate once on ingress; storage and LLM paths keep the bytes after that.
        Encoding.GetString(bytes);
        Bytes = bytes.ToArray();
    }

    public byte[] Bytes
    {
        get;
    }

    public int ByteCount => Bytes?.Length ?? 0;

    public bool IsEmpty => ByteCount == 0;

    public static Utf8String Empty
    {
        get;
    } = new(Array.Empty<byte>());

    public static Utf8String FromString(string? value)
    {
        return new Utf8String(Encoding.GetBytes(value ?? string.Empty));
    }

    public byte[] ToArray()
    {
        return Bytes?.ToArray() ?? Array.Empty<byte>();
    }

    public override string ToString()
    {
        return Encoding.GetString(Bytes ?? Array.Empty<byte>());
    }

    public bool Equals(Utf8String other)
    {
        return (Bytes ?? Array.Empty<byte>()).SequenceEqual(other.Bytes ?? Array.Empty<byte>());
    }

    public override bool Equals(object? obj)
    {
        return obj is Utf8String other && Equals(other);
    }

    public override int GetHashCode()
    {
        unchecked
        {
            var hash = 17;
            foreach (var item in Bytes ?? Array.Empty<byte>())
            {
                hash = hash * 31 + item;
            }

            return hash;
        }
    }

    public static explicit operator string(Utf8String value)
    {
        return value.ToString();
    }

    public static explicit operator Utf8String(string? value)
    {
        return FromString(value);
    }

    public static bool operator ==(Utf8String left, Utf8String right)
    {
        return left.Equals(right);
    }

    public static bool operator !=(Utf8String left, Utf8String right)
    {
        return !left.Equals(right);
    }
}
