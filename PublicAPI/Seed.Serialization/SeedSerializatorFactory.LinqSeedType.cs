using System;
using System.Collections.Generic;
using System.Text;

namespace Seed.Serialization;

public partial class SeedSerializatorFactory
{
    Type? TryGetAsLinqSeedType(RTTI.Record.RttiTypeRecord record)
    {
        switch (record.TemplateTypeName)
        {
            case "Seed::Core::Containers::List":
            case "Seed::Core::Containers::SensitiveList":
            case "Seed::Core::Containers::ConcurencyList":
            case "Seed::Core::Containers::ConcurencySensitiveList":
                {
                    var innerType = GetTypeWithOffset(0, record);

                    if (innerType == null)
                        return null;

                    return typeof(LinkedList<>).MakeGenericType(innerType);
                }
            case "Seed::Core::Containers::Dictionary":
            case "Seed::Core::Containers::ConcurencyDictionary":

            case "Seed::Core::Containers::Map":
            case "Seed::Core::Containers::ConcurencyMap":
                {
                    var innerTypeFirst = GetTypeWithOffset(1, record);
                    var innerTypeSecond = GetTypeWithOffset(2, record);

                    if (innerTypeFirst == null || innerTypeSecond == null)
                    {
                        return null;
                    }

                    Type[] innerTypes = { innerTypeFirst, innerTypeSecond };

                    return typeof(Dictionary<,>).MakeGenericType(innerTypes);
                }
            case "Seed::Core::Containers::Array":
            case "Seed::Core::Containers::SensitiveArray":
            case "Seed::Core::Containers::ConcurencyArray":
            case "Seed::Core::Containers::ConcurencySensitiveArray":

            case "Seed::Core::Containers::FixedArray":
            case "Seed::Core::Containers::SensitiveFixedArray":
            case "Seed::Core::Containers::ConcurencyFixedArray":
            case "Seed::Core::Containers::ConcurencySensitiveFixedArray":

            case "Seed::Core::Containers::DynamicStack":
            case "Seed::Core::Containers::SensitiveDynamicStack":
            case "Seed::Core::Containers::ConcurencyDynamicStack":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicStack":

            case "Seed::Core::Containers::FixedStack":
            case "Seed::Core::Containers::SensitiveFixedStack":
            case "Seed::Core::Containers::ConcurencyFixedStack":
            case "Seed::Core::Containers::ConcurencySensitiveFixedStack":

            case "Seed::Core::Containers::DynamicQueue":
            case "Seed::Core::Containers::SensitiveDynamicQueue":
            case "Seed::Core::Containers::ConcurencyDynamicQueue":
            case "Seed::Core::Containers::ConcurencySensitiveDynamicQueue":

            case "Seed::Core::Containers::FixedQueue":
            case "Seed::Core::Containers::SensitiveFixedQueue":
            case "Seed::Core::Containers::ConcurencyFixedQueue":
            case "Seed::Core::Containers::ConcurencySensitiveFixedQueue":
                {
                    var innerType = GetTypeWithOffset(0, record);

                    if (innerType == null)
                        return null;

                    return typeof(List<>).MakeGenericType(innerType);
                }
            case "Seed::Core::Containers::Pair":
            case "Seed::Core::Containers::SensitivePair":
                {
                    var innerTypeFirst = GetTypeWithOffset(0, record);
                    var innerTypeSecond = GetTypeWithOffset(1, record);

                    if (innerTypeFirst == null || innerTypeSecond == null)
                    {
                        return null;
                    }

                    Type[] innerTypes = { innerTypeFirst, innerTypeSecond };

                    return typeof(KeyValuePair<,>).MakeGenericType(innerTypes);
                }

            default: return null;
        }
    }
}
