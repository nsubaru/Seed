using Microsoft.Extensions.Logging;
using Seed.CTTI;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? GetCustomSerializatorType(RTTI.Record.RttiTypeRecord record)
    {
        return AppDomain.CurrentDomain
            .GetAssemblies()
            .SelectMany(x => x.GetTypes()
                              .Where(x => Attribute.GetCustomAttribute(x, typeof(SerializableStructAttribute)) != null
                        ))
            .FirstOrDefault(x => x.FullName == record.TypeName);
    }

    private void AddCustomTypeSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.Append($"/*Type: {typeRecord.TypeName}*/");
        code.Append($"ulong typeId = {typeRecord.RttiType.FullTypeId.TypeId};");
        code.Append($"var schema = customTypeMappingSchema[typeId];");
        code.Append("schema.Serialize(data, stream, this);");
    }

    private void AddCustomTypeDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.Append($"/*Type: {typeRecord.TypeName}*/");
        code.Append($"ulong typeId = {typeRecord.RttiType.FullTypeId.TypeId};");
        code.Append($"var schema = customTypeMappingSchema[typeId];");
        code.Append($"data = ({GetFriendlyTypeName(typeof(T))}) schema.Deserialize(stream, this);");
    }

    public void SerializeImpl(System.Security.SecureString data, Stream stream)
    {
        logger?.LogTrace("Serialize type System.Security.SecureString");
        ulong count = data.Length != 0 ? ((ulong)data.Length) + 1ul : 0ul;
        //SerializeImpl(count, stream);
        if (data.Length == 0)
            return;
        const int MaxStackAllocSize = 512;
        var ptr = IntPtr.Zero;
        try
        {
            ptr = Marshal.SecureStringToBSTR(data);
            int length = data.Length;

            unsafe {
                Span<byte> chars = new Span<byte>(ptr.ToPointer(), length * sizeof(char));
                stream.Write(chars);
            }
        }
        finally
        {
            if (ptr != IntPtr.Zero)
                Marshal.ZeroFreeBSTR(ptr);
        }

        stream.Write(Encoding.Unicode.GetBytes(new char[] { '\0' }), 0, sizeof(char));
    }
}
