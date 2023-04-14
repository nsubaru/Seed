using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    private void AddCollectionSer<T>(ref StringBuilder code, TypeRecord typeRecord)
    {
        switch (typeRecord.TypeName)
        {
            case "Seed::Core::Containers::Array":
            case "Seed::Core::Containers::SensitiveArray":
            case "Seed::Core::Containers::FixedArray":
            case "Seed::Core::Containers::SensitiveFixedArray":
            case "Seed::Core::Containers::ConcurencyArray":
            case "Seed::Core::Containers::ConcurencySensitiveArray":

            case "Seed::Core::Containers::DynamicStack":
            case "Seed::Core::Containers::SensitiveDynamicStack":
            case "Seed::Core::Containers::ConcurencyDynamicStack":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicStack":

            case "Seed::Core::Containers::FixedQueue":
            case "Seed::Core::Containers::DynamicQueue":
            case "Seed::Core::Containers::SensitiveDynamicQueue":
            case "Seed::Core::Containers::ConcurencyDynamicQueue":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicQueue":

            case "Seed::Core::Containers::List":
            case "Seed::Core::Containers::SensitiveList":
            case "Seed::Core::Containers::ConcurencyList":
            case "Seed::Core::Containers::ConcurencySensitiveList":
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

        switch (typeRecord.TypeName)
        {
            case "Seed::Core::Containers::Array":
            case "Seed::Core::Containers::ConcurencyArray":
            case "Seed::Core::Containers::ConcurencySensitiveArray":
            case "Seed::Core::Containers::SensitiveArray":

            case "Seed::Core::Containers::FixedArray":
            case "Seed::Core::Containers::SensitiveFixedArray":
            case "Seed::Core::Containers::ConcurencyFixedArray":
            case "Seed::Core::Containers::ConcurencySensitiveFixedArray":

            case "Seed::Core::Containers::FixedQueue":
            case "Seed::Core::Containers::SensitiveFixedQueue":
            case "Seed::Core::Containers::ConcurencyFixedQueue":
            case "Seed::Core::Containers::ConcurencySensitiveFixedQueue":

            case "Seed::Core::Containers::DynamicQueue":
            case "Seed::Core::Containers::SensitiveDynamicQueue":
            case "Seed::Core::Containers::ConcurencyDynamicQueue":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicQueue":

            case "Seed::Core::Containers::DynamicStack":
            case "Seed::Core::Containers::SensitiveDynamicStack":
            case "Seed::Core::Containers::ConcurencyDynamicStack":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicStack":
                code.AppendLine("for (UInt64 i = 0; i < count; i++){");
                code.AppendLine($"var item = CreateObject<{GetFriendlyTypeName(typeof(T).GetGenericArguments().First())}>();");
                code.AppendLine("item = DeserializeImpl(item, stream);");
                code.AppendLine("data.Add(item);}");
                break;

            case "Seed::Core::Containers::List":
            case "Seed::Core::Containers::SensitiveList":
            case "Seed::Core::Containers::ConcurencyList":
            case "Seed::Core::Containers::ConcurencySensitiveList":
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
