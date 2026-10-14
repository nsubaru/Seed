using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializerFactory
{
    private void AddCollectionSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case Constants.Types.Blob:
            case Constants.Types.BaseBlob:
            case Constants.Types.SensitiveBlob:
                {
                    code.AppendLine("var sz = (ulong)data.Length;"); // Get byte array size
                    code.AppendLine("this.SerializeImpl(sz, stream);");
                    // Use Span<byte> for small blobs
                    code.AppendLine("const int MaxStackAllocSize = 512;"); // 1KB stack limit
                    code.AppendLine("if (sz <= MaxStackAllocSize) {");
                    code.AppendLine("    Span<byte> buffer = stackalloc byte[(int)sz];");
                    code.AppendLine("    buffer = data;"); // Copy bytes to stackalloc
                    code.AppendLine("    stream.Write(buffer);");
                    code.AppendLine("} else {");
                    code.AppendLine("    stream.Write(data, 0, (int)sz);"); // Direct write for large blobs
                    code.AppendLine("}");
                    break;
                }

            case Constants.Types.BaseArray:
            case Constants.Types.Array:
            case Constants.Types.SensitiveArray:
            case Constants.Types.ConcurrencyArray:
            case Constants.Types.ConcurrencySensitiveArray:
            case Constants.Types.FixedArray:
            case Constants.Types.SensitiveFixedArray:
            case Constants.Types.ConcurrencyFixedArray:
            case Constants.Types.ConcurrencySensitiveFixedArray:
            case Constants.Types.FixedStack:
            case Constants.Types.SensitiveFixedStack:
            case Constants.Types.ConcurrencyFixedStack:
            case Constants.Types.ConcurrencySensitiveFixedStack:
            case Constants.Types.DynamicStack:
            case Constants.Types.SensitiveDynamicStack:
            case Constants.Types.ConcurrencyDynamicStack:
            case Constants.Types.ConcurrencySensitiveDynamicStack:
            case Constants.Types.FixedQueue:
            case Constants.Types.SensitiveFixedQueue:
            case Constants.Types.ConcurrencyFixedQueue:
            case Constants.Types.ConcurrencySensitiveFixedQueue:
            case Constants.Types.DynamicQueue:
            case Constants.Types.SensitiveDynamicQueue:
            case Constants.Types.ConcurrencyDynamicQueue:
            case Constants.Types.ConcurrencySensitiveDynamicQueue:
            case Constants.Types.BaseList:
            case Constants.Types.List:
            case Constants.Types.SensitiveList:
            case Constants.Types.ConcurrencyList:
            case Constants.Types.ConcurrencySensitiveList:
                {
                    code.AppendLine("var sz = (ulong) data.Count();");
                    code.AppendLine("this.SerializeImpl(sz, stream);");
                    code.AppendLine("foreach(var item in data) this.SerializeImpl(item, stream);");
                    break;
                }

            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }

    private void AddCollectionDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var count = (UInt64)0;");
        code.AppendLine($"count = DeserializeImpl(count, stream);");
        code.AppendLine($"if (data == null || count == 0) {{");
        code.AppendLine($"    data = CreateObject<{GetFriendlyTypeName(typeof(T))}>();");
        code.AppendLine($"}}");
        switch (typeRecord.TypeName)
        {
            case Constants.Types.Blob:
            case Constants.Types.BaseBlob:
            case Constants.Types.SensitiveBlob:
                {
                    code.AppendLine("const int MaxStackAllocSize = 512;"); // 1KB stack limit
                                                                           // **Small Blobs**: Use stackalloc
                    code.AppendLine("if (count <= MaxStackAllocSize) {");
                    code.AppendLine("    Span<byte> buffer = stackalloc byte[(int)count];");
                    code.AppendLine("    stream.Read(buffer);");
                    code.AppendLine("    data = buffer.ToArray();");
                    code.AppendLine("} else {");
                    code.AppendLine("    byte[] buffer = new byte[count];");
                    code.AppendLine("    stream.Read(buffer, 0, (int)count);"); // Direct read for large blobs
                    code.AppendLine("    data = buffer;");
                    code.AppendLine("}");
                    break;
                }

            case Constants.Types.BaseArray:
            case Constants.Types.Array:
            case Constants.Types.SensitiveArray:
            case Constants.Types.ConcurrencyArray:
            case Constants.Types.ConcurrencySensitiveArray:
            case Constants.Types.FixedArray:
            case Constants.Types.SensitiveFixedArray:
            case Constants.Types.ConcurrencyFixedArray:
            case Constants.Types.ConcurrencySensitiveFixedArray:
            case Constants.Types.FixedStack:
            case Constants.Types.SensitiveFixedStack:
            case Constants.Types.ConcurrencyFixedStack:
            case Constants.Types.ConcurrencySensitiveFixedStack:
            case Constants.Types.DynamicStack:
            case Constants.Types.SensitiveDynamicStack:
            case Constants.Types.ConcurrencyDynamicStack:
            case Constants.Types.ConcurrencySensitiveDynamicStack:
            case Constants.Types.FixedQueue:
            case Constants.Types.SensitiveFixedQueue:
            case Constants.Types.ConcurrencyFixedQueue:
            case Constants.Types.ConcurrencySensitiveFixedQueue:
            case Constants.Types.DynamicQueue:
            case Constants.Types.SensitiveDynamicQueue:
            case Constants.Types.ConcurrencyDynamicQueue:
            case Constants.Types.ConcurrencySensitiveDynamicQueue:
                {
                    code.AppendLine("for (UInt64 i = 0; i < count; i++){");
                    code.AppendLine("    var item = CreateObject<" + GetFriendlyTypeName(typeof(T).GetGenericArguments().First()) + ">();");
                    code.AppendLine("    item = DeserializeImpl(item, stream);");
                    code.AppendLine("    data.Add(item);");
                    code.AppendLine("}");
                    break;
                }

            case Constants.Types.BaseList:
            case Constants.Types.List:
            case Constants.Types.SensitiveList:
            case Constants.Types.ConcurrencyList:
            case Constants.Types.ConcurrencySensitiveList:
                {
                    code.AppendLine("for (UInt64 i = 0; i < count; i++){");
                    code.AppendLine("    var item = CreateObject<" + GetFriendlyTypeName(typeof(T).GetGenericArguments().First()) + ">();");
                    code.AppendLine("    item = DeserializeImpl(item, stream);");
                    code.AppendLine("    data.AddLast(item);");
                    code.AppendLine("}");
                    break;
                }

            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }
}
