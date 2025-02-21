using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddCollectionSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case Constants.Types.Blob:
            case Constants.Types.SensitiveBlob:

            case Constants.Types.Array: 
            case Constants.Types.SensitiveArray: 
            case Constants.Types.ConcurencyArray: 
            case Constants.Types.ConcurencySensitiveArray:

            case Constants.Types.FixedArray:
            case Constants.Types.SensitiveFixedArray:
            case Constants.Types.ConcurencyFixedArray:
            case Constants.Types.ConcurencySensitiveFixedArray:

            case Constants.Types.FixedStack:
            case Constants.Types.SensitiveFixedStack:
            case Constants.Types.ConcurencyFixedStack:
            case Constants.Types.ConcurencySensitiveFixedStack:

            case Constants.Types.DynamicStack:
            case Constants.Types.SensitiveDynamicStack:
            case Constants.Types.ConcurencyDynamicStack:
            case Constants.Types.ConcurencySensitiveDynamicStack:

            case Constants.Types.FixedQueue:
            case Constants.Types.SensitiveFixedQueue:
            case Constants.Types.ConcurencyFixedQueue:
            case Constants.Types.ConcurencySensitiveFixedQueue:

            case Constants.Types.DynamicQueue:
            case Constants.Types.SensitiveDynamicQueue:
            case Constants.Types.ConcurencyDynamicQueue:
            case Constants.Types.ConcurencySensitiveDynamicQueue:

            case Constants.Types.List:
            case Constants.Types.SensitiveList:
            case Constants.Types.ConcurencyList:
            case Constants.Types.ConcurencySensitiveList:
                {
                    code.AppendLine($"var sz = (ulong) data.Count();");
                    code.AppendLine("this.SerializeImpl(sz, stream);");
                    code.AppendLine("foreach(var item in data) this.SerializeImpl(item, stream);");
                }
                break;
            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }

    private void AddCollectionDer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        code.AppendLine($"var count = (UInt64)0;");
        code.AppendLine($"count = DeserializeImpl(count, stream);");
        code.AppendLine($"if (data == null) data = CreateObject<{GetFriendlyTypeName(typeof(T))}>();");

        switch (typeRecord.TypeName)
        {
            case Constants.Types.Blob:
            case Constants.Types.SensitiveBlob:

            case Constants.Types.Array:
            case Constants.Types.SensitiveArray:
            case Constants.Types.ConcurencyArray:
            case Constants.Types.ConcurencySensitiveArray:

            case Constants.Types.FixedArray:
            case Constants.Types.SensitiveFixedArray:
            case Constants.Types.ConcurencyFixedArray:
            case Constants.Types.ConcurencySensitiveFixedArray:

            case Constants.Types.FixedStack:
            case Constants.Types.SensitiveFixedStack:
            case Constants.Types.ConcurencyFixedStack:
            case Constants.Types.ConcurencySensitiveFixedStack:

            case Constants.Types.DynamicStack:
            case Constants.Types.SensitiveDynamicStack:
            case Constants.Types.ConcurencyDynamicStack:
            case Constants.Types.ConcurencySensitiveDynamicStack:

            case Constants.Types.FixedQueue:
            case Constants.Types.SensitiveFixedQueue:
            case Constants.Types.ConcurencyFixedQueue:
            case Constants.Types.ConcurencySensitiveFixedQueue:

            case Constants.Types.DynamicQueue:
            case Constants.Types.SensitiveDynamicQueue:
            case Constants.Types.ConcurencyDynamicQueue:
            case Constants.Types.ConcurencySensitiveDynamicQueue:
                code.AppendLine("for (UInt64 i = 0; i < count; i++){");
                code.AppendLine($"var item = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}>();");
                code.AppendLine("item = DeserializeImpl(item, stream);");
                code.AppendLine("data.Add(item);}");
                break;

            case Constants.Types.List:
            case Constants.Types.SensitiveList:
            case Constants.Types.ConcurencyList:
            case Constants.Types.ConcurencySensitiveList:
                code.AppendLine("for (UInt64 i = 0; i < count; i++){");
                code.AppendLine($"var item = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}>();");
                code.AppendLine("item = DeserializeImpl(item, stream);");
                code.AppendLine("data.AddLast(item);}");
                break;

            default:
                throw new NotImplementedException($"Serialization of type  ' {typeRecord.TypeName} ' and type class ' {typeRecord.TypeName} ' not implemented");
        }
    }
}
